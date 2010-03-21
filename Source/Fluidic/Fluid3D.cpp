/*
Copyright (c) 2010 Steven Leigh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "Fluid3D.h"

#include "Debug.h"
#include "GPUProgram.h"
#include "GPUProgramLoader3D.h"
#include "IVelocityPoller.h"

using namespace std;
using namespace Fluidic;

//do not include imdebug normally - just for debugging purposes.
#ifdef _DEBUG
//#define _IM_TEXTURE_DEBUG 1 // Comment this line out if imdebug is not available / wanted
#endif
#ifdef _IM_TEXTURE_DEBUG
	#include <imdebug.h>
	#include <imdebuggl.h>
	#pragma comment(lib, "imdebug.lib")
	#define DEBUGTEX(tex) imdebugTexImagef(GL_TEXTURE_RECTANGLE_ARB, mTextures[tex]);
#else
	#define DEBUGTEX(tex)
#endif

Fluid3D::Fluid3D(std::string cgHomeDir) :
Fluid(cgHomeDir), mSlabs(0,0)
{
	mCgVertexProfile = CG_PROFILE_VP40;
}

Fluid3D::~Fluid3D(void)
{
	delete mAdvect;
	delete mVorticity;
	delete mInject;
	delete mF1Boundary;
	delete mF4Boundary;
	delete mF1Jacobi;
	delete mF4Jacobi;
	delete mDivField;
	delete mRender;
	delete mSubtractPressureGradient;
	delete mOffset;
	delete mPerturb;
	delete mZCull;
}
FluidOptions Fluid3D::DefaultOptions()
{
	FluidOptions options;
	options.Size = Vector(1,1,1);
	options.SolverResolution = Vector(64,64,64);
	options.RenderResolution = Vector(400, 400);
	options.Viscosity = ViscosityAir;
	options.SolverOptions = RS_NICE | RS_DOUBLE_PRECISION;
	options.DiffuseSteps = 10;
	options.FixedTimeInterval = 0.02f;
	return options;
}
void Fluid3D::InitTextures() {

	mTextures = new GLuint[11];

	//initialise and set up textures
	glDeleteTextures(10, &mTextures[0]);
	glGenTextures(10, &mTextures[0]);

	//initial values for read and write textures
	outputSolver = 0;
	velocity = 1;
	pressure = 2;
	offset = 3;
	divField = 4;
	boundaries = 5;
	outputSolver1d = 6;
	
	data = 7;
	outputRender = 8;

	backface = 9;

	bool doublePrecision = mOptions.GetOption(RS_DOUBLE_PRECISION);
	int bits = doublePrecision ? 8 : 4;

	// SOLVER TEXTURES
	//4-component textures (RGBA) GL_RGBA16F_ARB | GL_RGBA32F_ARB
	GLuint rgbaFormat = doublePrecision ? GL_RGBA32F_ARB : GL_RGBA16F_ARB;

	Vector solverTextureSize(Vector(mOptions.SolverResolution.x * mSlabs.xi(), mOptions.SolverResolution.y * mSlabs.yi()));
	int size = 4*bits*solverTextureSize.xi()*solverTextureSize.yi();
	char *zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);

	SetupTexture(mTextures[outputSolver], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[velocity], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[data], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[pressure], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[divField], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[boundaries], rgbaFormat, solverTextureSize, 4, zeroData);
	SetupTexture(mTextures[offset], rgbaFormat, solverTextureSize, 4, zeroData);
	delete []zeroData;

	//1-component textures (LUMINANCE) GL_LUMINANCE16F_ARB | GL_LUMINANCE32F_ARB
	GLuint lumFormat = doublePrecision ? GL_LUMINANCE32F_ARB : GL_LUMINANCE16F_ARB;

	size = bits*solverTextureSize.xi()*solverTextureSize.yi();
	zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);
	
	SetupTexture(mTextures[pressure], lumFormat, solverTextureSize, 1, zeroData);
	SetupTexture(mTextures[divField], lumFormat, solverTextureSize, 1, zeroData);
	SetupTexture(mTextures[outputSolver1d], lumFormat, solverTextureSize, 1, zeroData);
	delete []zeroData;

	/// RENDER TEXTURES
	size = 4*bits*mOptions.RenderResolution.xi()*mOptions.RenderResolution.yi();
	zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);
	
	SetupTexture(mTextures[backface], rgbaFormat, mOptions.RenderResolution, 4, zeroData);
	SetupTexture(mTextures[outputRender], rgbaFormat, mOptions.RenderResolution, 4, zeroData);
	delete []zeroData;

	SetGlobalProgramParams();
}

void Fluid3D::InitCallLists()
{
	mSlabs = Vector((float)SlicesPerRow, (float)mOptions.SolverResolution.zi()/SlicesPerRow);

	//delete the existing lists
	if (mFluidCallListId) glDeleteLists(mFluidCallListId, 3);
	mFluidCallListId = glGenLists(3);

	Vector res = mSlabs * mOptions.SolverResolution;
	//solver list - size=SolverResolution, textures SolverResolution
	glNewList(mFluidCallListId + SolverCallListOffset, GL_COMPILE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); 
			glVertex3f(0.0, 0.0, 1.0);

			glTexCoord2f(res.x, 0.0);
			glVertex3f(res.x, 0.0, 1.0);

			glTexCoord2f(res.x, res.y);
			glVertex3f(res.x, res.y, 1.0);

			glTexCoord2f(0.0, res.y);
			glVertex3f(0.0, res.y, 1.0);
		glEnd();
	glEndList();
	
	//data list - size=RenderResolution, textures RenderResolution
	// not used, methinks
	glNewList(mFluidCallListId + RenderDataCallListOffset, GL_COMPILE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); 
			glVertex3f(0.0, 0.0, 1.0);

			glTexCoord2f(mOptions.RenderResolution.x, 0.0);
			glVertex3f(mOptions.RenderResolution.x, 0.0, 1.0);

			glTexCoord2f(mOptions.RenderResolution.x, mOptions.RenderResolution.y);
			glVertex3f(mOptions.RenderResolution.x, mOptions.RenderResolution.y, 1.0f);

			glTexCoord2f(0.0, mOptions.RenderResolution.y);
			glVertex3f(0.0, mOptions.RenderResolution.y, 1.0);
		glEnd();
	glEndList();

	//render list - 3d, based no size
	float x = mOptions.Size.x;
	float y = mOptions.Size.y;
	float z = mOptions.Size.z;
	// TODO: Can I do this procedurally?
	glNewList(mFluidCallListId + RenderCallListOffset, GL_COMPILE);
		glBegin(GL_QUADS);
		// Back side
		glNormal3f(0.0, 0.0, -1.0);
		GLCubeVertex(0.0, 0.0, 0.0);
		GLCubeVertex(0.0, y, 0.0);
		GLCubeVertex(x, y, 0.0);
		GLCubeVertex(x, 0.0, 0.0);

		// Front side
		glNormal3f(0.0, 0.0, 1.0);
		GLCubeVertex(0.0, 0.0, z);
		GLCubeVertex(x, 0.0, z);
		GLCubeVertex(x, y, z);
		GLCubeVertex(0.0, y, z);

		// Top side
		glNormal3f(0.0, 1.0, 0.0);
		GLCubeVertex(0.0, y, 0.0);
		GLCubeVertex(0.0, y, z);
		GLCubeVertex(x, y, z);
		GLCubeVertex(x, y, 0.0);

		// Bottom side
		glNormal3f(0.0, -1.0, 0.0);
		GLCubeVertex(0.0, 0.0, 0.0);
		GLCubeVertex(x, 0.0, 0.0);
		GLCubeVertex(x, 0.0, z);
		GLCubeVertex(0.0, 0.0, z);

		// Left side
		glNormal3f(-1.0, 0.0, 0.0);
		GLCubeVertex(0.0, 0.0, 0.0);
		GLCubeVertex(0.0, 0.0, z);
		GLCubeVertex(0.0, y, z);
		GLCubeVertex(0.0, y, 0.0);

		// Right side
		glNormal3f(1.0, 0.0, 0.0);
		GLCubeVertex(x, 0.0, 0.0);
		GLCubeVertex(x, y, 0.0);
		GLCubeVertex(x, y, z);
		GLCubeVertex(x, 0.0, z);
		glEnd();
	glEndList();
}

void Fluid3D::InitBuffers()
{
	if (mRenderbufferId) DestroyBuffers();

	CheckGLError("First");

	glGenFramebuffersEXT(1, &mFramebufferId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebufferId);
	CheckGLError("FrameBufferBind");

	//init and set render buffer for the solver
	glGenRenderbuffersEXT(1, &mRenderbufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderbufferId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mSlabs.xi()*mOptions.SolverResolution.xi(), mSlabs.yi()*mOptions.SolverResolution.yi() );

	//init and set render buffer for the data
	glGenRenderbuffersEXT(1, &mRenderbufferDataId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderbufferDataId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mOptions.RenderResolution.xi(), mOptions.RenderResolution.yi() );

	//attach render buffer to frame buffer (duh?)
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Fluid3D::InitPrograms(const std::string &cgHomeDir)
{
	GPUProgramLoader3D loader(cgHomeDir, mCgContext, mCgVertexProfile,  mCgFragmentProfile);

	mAdvect = loader.Advect();
	mVorticity = loader.Vorticity();
	mInject = loader.Inject();
	mPerturb = loader.Perturb();
	mF1Boundary = loader.F1Boundary();
	mF4Boundary = loader.F4Boundary();
	mOffset = loader.Offset();
	mF1Jacobi = loader.F1Jacobi();
	mF4Jacobi = loader.F4Jacobi();
	mDivField = loader.DivField();
	mSubtractPressureGradient = loader.SubtractPressureGradient();
	mZCull = loader.ZCull();
	mRender = loader.Render();

	mRaycastVProgram = loader.RayCastVertex();
	mRaycastFProgram =  loader.RayCastFragment();
}

void Fluid3D::SetGlobalProgramParams()
{
	//preload the stuff
	float resX = mOptions.SolverResolution.x,
	      resY = mOptions.SolverResolution.y,
	      resZ = mOptions.SolverResolution.z;
	mAdvect->SetParam("res", resX, resY, resZ);
	mVorticity->SetParam("res", resX, resY, resZ);
	//mInject->SetParam("res", resX, resY, resZ);
	mF1Boundary->SetParam("res", resX, resY, resZ);
	mF4Boundary->SetParam("res", resX, resY, resZ);
	mF1Jacobi->SetParam("res", resX, resY, resZ);
	mF4Jacobi->SetParam("res", resX, resY, resZ);
	mDivField->SetParam("res", resX, resY, resZ);
	mSubtractPressureGradient->SetParam("res", resX, resY, resZ);
	mOffset->SetParam("res", resX, resY, resZ);
	mRaycastFProgram->SetParam("res", resX, resY, resZ);
	//fixme illuminateProgram->SetParam("res", resX, resY, resZ);
	
	float slabsX = mSlabs.x, slabsY = mSlabs.y;
	mAdvect->SetParam("slabs", slabsX, slabsY);
	mVorticity->SetParam("slabs", slabsX, slabsY);
	//mInject->SetParam("slabs", slabsX, slabsY);
	mF1Boundary->SetParam("slabs", slabsX, slabsY);
	mF4Boundary->SetParam("slabs", slabsX, slabsY);
	mF1Jacobi->SetParam("slabs", slabsX, slabsY);
	mF4Jacobi->SetParam("slabs", slabsX, slabsY);
	mDivField->SetParam("slabs", slabsX, slabsY);
	mSubtractPressureGradient->SetParam("slabs", slabsX, slabsY);
	mOffset->SetParam("slabs", slabsX, slabsY);
	mRaycastFProgram->SetParam("slabs", slabsX, slabsY);
	//fixme illuminateProgram->SetParam("slabs", slabsX, slabsY);
}

/** Render and Updates */
void Fluid3D::Update(float time)
{
	if (!ready) return;

	PrePostUpdate(true);

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);

	//Do the interactiony stuff
	InjectInkStep();

	//PerturbFluidStep();
	//UpdateArbitraryBoundaryStep();
	UpdateOffsetStep();	

	mLastSolveCount = 0;
	if (mOptions.FixedTimeInterval == 0)
	{
		if (time > 0) 
		{
			UpdateStep(time);
			mLastSolveCount++;
		}
	}
	else
	{
		mTimeDelta += time;
		while (mTimeDelta > mOptions.FixedTimeInterval)
		{
			UpdateStep(mOptions.FixedTimeInterval);
			mTimeDelta -= mOptions.FixedTimeInterval;
			mLastSolveCount++;
		}
	}

	PrePostUpdate(false);
	CheckGLError("After Update");
}

void Fluid3D::UpdateStep(float time)
{
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);
	PerturbDensityStep(time);
	BoundaryVelocityStep();
	if (mOptions.GetOption(RS_VORTICITY_CONFINEMENT)) VorticityConfinementStep(time);
	if (mOptions.GetOption(RS_ADVECT_VELOCITY)) AdvectVelocityStep(time);
	
	if (mOptions.GetOption(RS_ZCULL)) 
	{
		glEnable(GL_DEPTH_TEST);
		ZCullStep(false);
	}
	if (mOptions.GetOption(RS_DIFFUSE_VELOCITY)) DiffuseVelocityStep(time);

	UpdatePressureStep(time);
	if (mOptions.GetOption(RS_ZCULL)) glDisable(GL_DEPTH_TEST);
	BoundaryPressureStep();
	SubtractPressureGradientStep(time);

	Poll(time);

	if (mOptions.GetOption(RS_ADVECT_DATA)) AdvectDataStep(time);
	if (mOptions.GetOption(RS_DIFFUSE_DATA)) DiffuseDataStep(time);
}

void Fluid3D::Render()
{
	// push all OpenGL Attributes
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// set up opengl environment
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebufferId);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferDataId);
	
	//push matrix, move camera to appropriate position
	glPushMatrix();
	Vector translateOffset = -GetSize()/2.f;
	glTranslatef(translateOffset.x, translateOffset.y, translateOffset.z);
	
	glViewport(0, 0, mOptions.RenderResolution.xi(), mOptions.RenderResolution.yi());
	
	//render to 'render' texture
	SetOutputTexture(backface);
	
	//clear and disable shaders
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	cgGLDisableProfile(mCgFragmentProfile);
	cgGLDisableProfile(mCgVertexProfile);
	
	//draw the back faces. This section writes the backface coordinates
	// (in [0, 1]) to a texture, for use by the fragment program
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glCallList(mFluidCallListId + RenderCallListOffset);
	glDisable(GL_CULL_FACE);

	//swap output and backface textures
	std::swap(outputRender, backface);

	//enable depth test (for proper occlusion), and disable render-to-texture
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//enable raycast programs
	cgGLEnableProfile(mCgFragmentProfile);
	cgGLEnableProfile(mCgVertexProfile);
	mRaycastVProgram->Bind();
	mRaycastFProgram->Bind();
		
	mRaycastFProgram->SetParam("stepsize", 0.1f); //TODO: variable step size
	mRaycastFProgram->SetParamTex("backface_tex", mTextures[backface]);
	mRaycastFProgram->SetParamTex("volume_tex", mTextures[data]);
	mRaycastFProgram->SetParam("screenRes", mOptions.RenderResolution.x, mOptions.RenderResolution.y);
	mRaycastFProgram->SetParam("res", mOptions.SolverResolution.x, mOptions.SolverResolution.y, mOptions.SolverResolution.z);
	mRaycastFProgram->SetParam("slabs", mSlabs.x, mSlabs.y);
		
	//this part draws the 'front' part of the fluid. This will get the
	//cordinates (in [0,1]) of the front face, passing it to the fragment
	//shader, which then performs the raycasting step
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glCallList(mFluidCallListId + RenderCallListOffset);
	glDisable(GL_CULL_FACE);
	
	cgGLDisableProfile(mCgFragmentProfile);
	cgGLDisableProfile(mCgVertexProfile);
	
	//done
	glPopMatrix();
	glPopAttrib();
}

void Fluid3D::InjectCheckeredData()
{
	if (!ready) return;

	int resX = mOptions.SolverResolution.xi(),
		resY = mOptions.SolverResolution.yi(),
		resZ = mOptions.SolverResolution.zi();
	GLfloat *cpuData = new GLfloat[resX*resY*resZ*4];

	int offsetX = 0;
	int offsetY = 0;
	int offsetZ = 0;

	int type = 30;

	for (int i=offsetX;i<resX-offsetX;i++)
	{
		for (int j =offsetY;j<resY-offsetY;j++)
		{
			for (int k=offsetZ; k<resZ-offsetZ; k++)
			{
				//test
				switch (type) {
					case 0: //test mini slabs
						cpuData[Index(i,j,k)] = (i % 16 < 8) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+1] = ((k+8) % 16 < 2) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+2] = ((k % 16 < 2)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+3] = 1.f;
						break;
						
					case 10: //test mega slabs
						cpuData[Index(i,j,k)] = 0.f;
						cpuData[Index(i,j,k)+1] = ((k+8) % 16 < 8) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+2] = (k % 16 < 8) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+3] = 1.f;
						break;
					
					case 20: //checkered data
						cpuData[Index(i,j,k)] = ((i % 16 < 8) && (j % 16 < 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+1] = ((j % 16 < 8) && (k % 16 < 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+2] = ((k % 16 < 8) && (i % 16 < 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+3] = 1.f;
						break;
					
					case 30: //checkered data
						cpuData[Index(i,j,k)] = ((i % 16 < 8) | (j % 16 > 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+1] = ((j % 16 < 8) | (k % 16 > 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+2] = ((k % 16 < 8) | (i % 16 > 8)) ? 1.f : 0.f;
						cpuData[Index(i,j,k)+3] = 1.f;
						break;
					
					case 40: //gradient
						cpuData[Index(i,j,k)] = 1.0f*i / resX;
						cpuData[Index(i,j,k)+1] = 1.0f*j / resY;
						cpuData[Index(i,j,k)+2] = 1.0f*k / resZ;
						cpuData[Index(i,j,k)+3] = 1.f;
						break;
						
				} //switch
			} //k
		} //j
	} //i
	
	CopyFromCPUtoGPU(GL_TEXTURE_RECTANGLE_ARB, mTextures[data], resX*mSlabs.xi(), resY*mSlabs.yi(), cpuData);
	
	delete[] cpuData;
	cpuData = 0;
}
void Fluid3D::GenerateCircularVortex()
{
	if (!ready) return;
	GLfloat *cpuVelocity = new GLfloat[mOptions.SolverResolution.xi() * mOptions.SolverResolution.yi()*4];

	for (int i=0;i<mOptions.SolverResolution.x;i++)
	{
		for (int j =0;j<mOptions.SolverResolution.y;j++)
		{
			float P = (j) / (mOptions.SolverResolution.y + 1.0f);
			float N = (i) / (mOptions.SolverResolution.x + 1.0f);

			if (N==0) N = 1;

			N = N*0.1f + 0.2525f;
			P = P*0.1f + 0.2525f;

			//circular vortex
			//cpuVelocity[INDX(i,j)] = -1*(j - mOptions.SolverResolution.y/2.0f) / (mOptions.SolverResolution.y + 1.0f);
			//cpuVelocity[INDX(i,j)+1] = 1*(i - mOptions.SolverResolution.x/2.0f) / (mOptions.SolverResolution.x + 1.0f);
			//cpuVelocity[INDX(i,j)+2] = 0;
			//cpuVelocity[INDX(i,j)+3] = 0;
		}
	}

	CopyFromCPUtoGPU(GL_TEXTURE_RECTANGLE_ARB, mTextures[velocity], mOptions.SolverResolution.xi(), mOptions.SolverResolution.yi(), cpuVelocity);

	delete[] cpuVelocity;
	cpuVelocity = 0;
}

// Do only if list is not empty, use time based stuff
// It would be good to get the curl here as well, for coolness.
// Should be based time, not frames?
void Fluid3D::Poll(float time)
{
	time=time; // Compiler warning - may be used in future. TODO.
	static float pixels[3];
	// in the velocity texture, and use it when doing the div?
	if (mPollFrame++ % 20 == 0)
	{
		SetOutputTexture(velocity);

		for (VelocityPollerList::iterator it = mVelocityPollers.begin(); it != mVelocityPollers.end(); it++)
		{
		
			Vector position = Coords2D((*it)->GetPosition() * mOptions.SolverResolution);

			glReadPixels(position.xi(), position.yi(), 1, 1, GL_RGB, GL_FLOAT, &pixels);
			(*it)->UpdateVelocity(Vector(pixels[0], pixels[1], pixels[2]));
		}
	}
}

/** Steps - Simulation */
void Fluid3D::AdvectDataStep(float time)
{
	if (!ready) return;

	mAdvect->Bind();

	//params
	mAdvect->SetParamTex("boundary", mTextures[boundaries]);
	mAdvect->SetParamTex("velocity", mTextures[velocity]);
	mAdvect->SetParamTex("data", mTextures[data]);
	mAdvect->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, mOptions.SolverDelta.z, time);

	DoCalculationSolver(data);
}

void Fluid3D::AdvectVelocityStep(float time)
{
	if (!ready) return;

	mAdvect->Bind();

	//params
	mAdvect->SetParamTex("boundary", mTextures[boundaries]);
	mAdvect->SetParamTex("velocity", mTextures[velocity]);
	mAdvect->SetParamTex("data", mTextures[velocity]);
	mAdvect->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, mOptions.SolverDelta.z, time);

	DoCalculationSolver(velocity);
}

void Fluid3D::VorticityConfinementStep(float time)
{
	if (!ready) return;

	//params
	mVorticity->Bind();
	mVorticity->SetParamTex("velocity", mTextures[velocity]);
	mVorticity->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, mOptions.SolverDelta.z, time);
	//mVorticity->SetParamTex("boundary", mTextures[boundaries]);

	DoCalculationSolver(velocity);
}

void Fluid3D::DiffuseDataStep(float time)
{
	if (!ready) return;

	float alpha = mOptions.SolverDelta.x * mOptions.SolverDelta.y * mOptions.SolverDelta.z / (time * mOptions.Viscosity);
	float beta = 6 + alpha;
	mF4Jacobi->Bind();
	mF4Jacobi->SetParam("alpha", alpha);
	mF4Jacobi->SetParam("beta", beta);

	for (int i=0;i<mOptions.DiffuseSteps;i++)
	{
		mF4Jacobi->SetParamTex("x", mTextures[data]);
		mF4Jacobi->SetParamTex("b", mTextures[data]);

		glTranslatef(0, 0, -0.025f);
		DoCalculationSolver(data);
	}
	glLoadIdentity();
}


void Fluid3D::DiffuseVelocityStep(float time)
{
	if (!ready) return;

	mF4Jacobi->Bind();

	float alpha = mOptions.SolverDelta.x * mOptions.SolverDelta.y * mOptions.SolverDelta.z / (time * mOptions.Viscosity);
	float beta = 6 + alpha;
	mF4Jacobi->SetParam("alpha", alpha);
	mF4Jacobi->SetParam("beta", beta);

	for (int i=0; i<mOptions.DiffuseSteps; i++)
	{
		mF4Jacobi->SetParamTex("x", mTextures[velocity]);
		mF4Jacobi->SetParamTex("b", mTextures[velocity]);

		glTranslatef(0, 0, -0.025f); //for z cull
		DoCalculationSolver(velocity);
	}

	glLoadIdentity(); //restore matrix Z to 0 for z cull
}

void Fluid3D::UpdatePressureStep(float time)
{
	if (!ready) return;

	// Calculate Divergence Field 
	mDivField->Bind();

	mDivField->SetParamTex("velocity", mTextures[velocity]);
	mDivField->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, mOptions.SolverDelta.z, time);

	DoCalculationSolver1D(divField);

	// Find pressure using jacobi iterations
	for (int i=0;i<mOptions.DiffuseSteps;i++)
	{
		mF1Jacobi->Bind();
		//tfsbad what about dz?
		mF1Jacobi->SetParam("alpha", -(mOptions.SolverDelta.x * mOptions.SolverDelta.y));
		mF1Jacobi->SetParam("beta", 6.0);
		mF1Jacobi->SetParamTex("b", mTextures[divField]);
		mF1Jacobi->SetParamTex("x", mTextures[pressure]);
		
		glTranslatef(0, 0, -0.025f);
		DoCalculationSolver1D(pressure);

		//bcPressure();
	}
	glLoadIdentity();
}

void Fluid3D::SubtractPressureGradientStep(float time)
{
	if (!ready) return;
	
	//Subtract the pressure gradient
	mSubtractPressureGradient->Bind();

	mSubtractPressureGradient->SetParamTex("velocity", mTextures[velocity]);
	mSubtractPressureGradient->SetParamTex("pressure", mTextures[pressure]);
	mSubtractPressureGradient->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, mOptions.SolverDelta.z, time);

	DoCalculationSolver(velocity);
}

/** Steps - Interaction */
void Fluid3D::InjectInkStep()
{
	if (!ready) return;
	if (mInjectors.empty()) return;

	cgGLDisableProfile(mCgFragmentProfile);

	SetOutputTexture(outputSolver);

	glColor4f(1, 1, 1, 1);
	//render existing ink
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mTextures[data]);
	glCallList(mFluidCallListId + SolverCallListOffset);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	//render ink for each bit
	for (InjectorList::iterator it = mInjectors.begin(); it != mInjectors.end(); ++it)
	{
		const Injector &inj = *it;

		glBlendFunc(GL_ONE, inj.overwrite ? GL_ZERO : GL_ONE);

		Vector d = mOptions.SolverDeltaInv * inj.size ;
		
		Vector pos = inj.position;
		if (pos.x + inj.size/2 > mOptions.Size.x) pos.x = mOptions.Size.x - inj.size/2;
		if (pos.x < inj.size/2) pos.x = inj.size/2;

		if (pos.y + inj.size/2 > mOptions.Size.y) pos.y = mOptions.Size.y - inj.size/2;
		if (pos.y < inj.size/2) pos.z = inj.size/2;

		if (pos.z + inj.size/2 > mOptions.Size.z) pos.z = mOptions.Size.z - inj.size/2;
		if (pos.z < inj.size/2) pos.z = inj.size/2;


		pos = pos * mOptions.SolverDeltaInv - d/2;

		int y = (pos.yi() - d.yi()/2);
		if (y < 0) y = 0;

		int endY = (pos.yi() + d.yi()/2);
		if (endY > mOptions.SolverResolution.yi()-1) endY = mOptions.SolverResolution.yi();
		
		glColor4f(inj.color.x, inj.color.y, inj.color.z, 1.f);

		for (; y <= endY; y++) {
			float xOffset = (float)(y % SlicesPerRow)*mOptions.SolverResolution.x; //FIXME magic number
			float zOffset = (float)(y / SlicesPerRow)*mOptions.SolverResolution.z; //FIXME magic number. also, confusing

			glBegin(GL_QUADS);
				glVertex3f(xOffset + pos.x,		zOffset + pos.z, 1);
				glVertex3f(xOffset + pos.x+d.x, zOffset + pos.z, 1);
				glVertex3f(xOffset + pos.x+d.x, zOffset + pos.z+d.z, 1);
				glVertex3f(xOffset + pos.x,		zOffset + pos.z+d.z, 1);
			glEnd();
		}

	}
	glColor3f(0,0,0);

	glDisable(GL_BLEND);

	//swap textures
	std::swap(outputSolver, data);
	cgGLEnableProfile(mCgFragmentProfile);
	
	mInjectors.clear();
}

void Fluid3D::PerturbFluidStep()
{
	if (!ready) return;
	if (mPerturbers.empty()) return;

	mInject->Bind();

	//render velocity for each perturbation
	for (PerturberList::iterator it = mPerturbers.begin(); it != mPerturbers.end(); ++it)
	{
		Perturber perturber = *it;
		Vector pos = perturber.position * mOptions.SolverDeltaInv;
		float px = perturber.velocity.x;
		float py = perturber.velocity.y;

		mInject->SetParam("p", pos.x, pos.y);
		mInject->SetParam("color", px, py, 0, 0);
		mInject->SetParam("scale", perturber.size * mOptions.SolverDeltaInv.Length());
		mInject->SetParamTex("data", mTextures[velocity]);

		DoCalculationSolver(velocity);
	}	
	mPerturbers.clear();
}


void Fluid3D::PerturbDensityStep(float time)
{
	if (!ready) return;

	//params
	mPerturb->Bind();
	mPerturb->SetParamTex("velocity", mTextures[velocity]);
	mPerturb->SetParamTex("data", mTextures[data]);
	mPerturb->SetParam("d", mOptions.SolverToRenderScale.x, mOptions.SolverToRenderScale.y, 0, time);

	DoCalculationSolver(velocity);
}

void Fluid3D::UpdateArbitraryBoundaryStep()
{
	if (!ready) return;
	if (mBoundaries.empty()) return;

	cgGLDisableProfile(mCgFragmentProfile);

	SetOutputTexture(outputSolver);

	//render existing boundaries
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mTextures[boundaries]);
	glCallList(mFluidCallListId + SolverCallListOffset);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	//render boundary for each bit
	glColor3f(1, 1, 1);
	for (BoundaryList::iterator it = mBoundaries.begin(); it != mBoundaries.end(); ++it)
	{
		Boundary boundary = *it;
		float dx = boundary.size;
		float dy = boundary.size;
		float x = mOptions.SolverResolution.x * boundary.position.x - dx/2;
		float y = mOptions.SolverResolution.y * boundary.position.y - dy/2;

		glBegin(GL_QUADS);
			glVertex3f(x, y, 1);
			glVertex3f(x+dx, y, 1);
			glVertex3f(x+dx, y+dy, 1);
			glVertex3f(x, y+dy, 1);
		glEnd();
	}
	glColor3f(0,0,0);

	//swap textures
	std::swap(outputSolver, boundaries);
	cgGLEnableProfile(mCgFragmentProfile);
	
	mBoundaries.clear();
}

/** Steps - Boundaries */
void Fluid3D::BoundaryVelocityStep()
{
	if (!ready) return;

	mF4Boundary->Bind();

	mF4Boundary->SetParamTex("data", mTextures[velocity]);
	mF4Boundary->SetParamTex("offset", mTextures[offset]);
	mF4Boundary->SetParam("scale", -1);

	DoCalculationSolver(velocity);
}

void Fluid3D::BoundaryPressureStep()
{
	if (!ready) return;

	mF1Boundary->Bind();

	mF1Boundary->SetParamTex("data", mTextures[pressure]);
	mF1Boundary->SetParamTex("offset", mTextures[offset]);
	mF1Boundary->SetParam("scale", 1);

	DoCalculationSolver1D(pressure);
}

void Fluid3D::UpdateOffsetStep() {

	if (!ready) return;

	//update offsets
	mOffset->Bind();
	mOffset->SetParamTex("boundaries", mTextures[boundaries]);

	DoCalculationSolver(offset);
}

/** Steps - Optimisation */
void Fluid3D::ZCullStep(bool clearFirst)
{
	if (!ready) return;

	SetOutputTexture(outputSolver);
	glLoadIdentity();

	//set up buffers etc.
	glDepthMask(GL_TRUE);
	if (clearFirst) {
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	glDepthFunc(GL_LESS);
	
	//run the program
	mZCull->Bind();
	mZCull->SetParamTex("data", mTextures[data]);
	mZCull->SetParamTex("velocity", mTextures[velocity]);
	mZCull->SetParamTex("boundaries", mTextures[boundaries]);

	glCallList(mFluidCallListId + SolverCallListOffset);

	glDepthMask(GL_FALSE);
}

void Fluid3D::SetColorDensities(float r, float g, float b)
{
	mPerturb->SetParam("densities", r, g, b);
}
void Fluid3D::PrePostUpdate(bool pre)
{
	if (pre) 
	{
		CheckGLError("Pre Update");
		//glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		//enable framebuffer and fragment program
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebufferId);
		cgGLEnableProfile(mCgFragmentProfile);

		//set up matrices for simulation
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		
		Vector solverRenderArea = mOptions.SolverResolution * mSlabs;

		// FIXME: glOrtho in PreUpdate just seems dodgy...
		glOrtho(0, solverRenderArea.x, 0, solverRenderArea.y, -1, 0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		// FIXME: glViewport in PreUpdate just seems dodgy...
		glViewport (0, 0, solverRenderArea.xi(), solverRenderArea.yi());
	} 
	else 
	{
		CheckGLError("Post Update");
		cgGLDisableProfile(mCgFragmentProfile);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		//restore matrices
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPopAttrib();

	}
}