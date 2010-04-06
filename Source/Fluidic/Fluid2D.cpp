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

#include "Fluid2D.h"

#include "Debug.h"
#include "GPUProgram.h"
#include "GPUProgramLoader2D.h"
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

Fluid2D::Fluid2D(std::string cgHomeDir) :
Fluid(cgHomeDir)
{
}

Fluid2D::~Fluid2D(void)
{
	DeletePrograms();
}
void Fluid2D::DeletePrograms(void)
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
FluidOptions Fluid2D::DefaultOptions()
{
	FluidOptions options;
	options.Size = Vector(1,1);
	options.SolverResolution = Vector(200, 200);
	options.RenderResolution = Vector(400, 400);
	options.Viscosity = ViscosityAir;
	options.SolverOptions = RS_NICE | RS_DOUBLE_PRECISION;
	options.RenderOptions = RR_INK | RR_BOUNDARIES;
	options.DiffuseSteps = 30;
	options.FixedTimeInterval = 0.005f;
	return options;
}
void Fluid2D::InitTextures() {
	mTextures = new GLuint[9];

	//initialise and set up textures
	glDeleteTextures(9, &mTextures[0]);
	glGenTextures(9, &mTextures[0]);

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

	bool doublePrecision = mOptions.GetOption(RS_DOUBLE_PRECISION);
	int bits = doublePrecision ? 8 : 4;

	//4-component textures (RGBA) GL_RGBA16F_ARB | GL_RGBA16F_ARB
	GLuint rgbaFormat = doublePrecision ? GL_RGBA32F_ARB : GL_RGBA16F_ARB;

	int size = 4*bits*mOptions.SolverResolution.xi()*mOptions.SolverResolution.yi();
	char *zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);

	//output
	SetupTexture(mTextures[outputSolver], rgbaFormat, mOptions.SolverResolution, 4, zeroData);
	SetupTexture(mTextures[velocity], rgbaFormat, mOptions.SolverResolution, 4, zeroData);
	SetupTexture(mTextures[boundaries], rgbaFormat, mOptions.SolverResolution, 4, zeroData);
	SetupTexture(mTextures[offset], rgbaFormat, mOptions.SolverResolution, 4, zeroData);
	delete []zeroData;

	size = 4*bits*mOptions.RenderResolution.xi()*mOptions.RenderResolution.yi();
	zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);
	
	SetupTexture(mTextures[data], rgbaFormat, mOptions.RenderResolution, 4, zeroData);
	SetupTexture(mTextures[outputRender], rgbaFormat, mOptions.RenderResolution, 4, zeroData);
	delete []zeroData;

	//1-component textures (LUMINANCE) GL_LUMINANCE16F_ARB | GL_LUMINANCE32F_ARB
	GLuint lumFormat = doublePrecision ? GL_LUMINANCE32F_ARB : GL_LUMINANCE16F_ARB;

	size = bits*mOptions.SolverResolution.xi()*mOptions.SolverResolution.yi();
	zeroData = new char[size];
	memset(zeroData, 0, sizeof(char) * size);
	
	SetupTexture(mTextures[pressure], lumFormat, mOptions.SolverResolution, 1, zeroData);
	SetupTexture(mTextures[divField], lumFormat, mOptions.SolverResolution, 1, zeroData);
	SetupTexture(mTextures[outputSolver1d], lumFormat, mOptions.SolverResolution, 1, zeroData);
	delete []zeroData;
}

void Fluid2D::InitCallLists()
{
	//delete the existing lists
	if (mFluidCallListId) glDeleteLists(mFluidCallListId, 3);
	mFluidCallListId = glGenLists(3);

	//solver list - size=SolverResolution, textures SolverResolution
	glNewList(mFluidCallListId + SolverCallListOffset, GL_COMPILE);
	DrawSolverQuad(mOptions.SolverResolution, mOptions.SolverResolution, 1.f);
	glEndList();
	
	//data list - size=RenderResolution, textures RenderResolution
	glNewList(mFluidCallListId + RenderDataCallListOffset, GL_COMPILE);
	DrawSolverQuad(mOptions.RenderResolution, mOptions.RenderResolution, 1.f);
	glEndList();

	//render list - size=Size; textures RenderResolution
	glNewList(mFluidCallListId + RenderCallListOffset, GL_COMPILE);
	DrawSolverQuad(mOptions.RenderResolution, mOptions.Size, 0.f);
	glEndList();
}

void Fluid2D::InitBuffers()
{
	if (mRenderbufferId) DestroyBuffers();

	CheckGLError("First");

	glGenFramebuffersEXT(1, &mFramebufferId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFramebufferId);
	CheckGLError("FrameBufferBind");

	//init and set render buffer for the solver
	glGenRenderbuffersEXT(1, &mRenderbufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderbufferId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mOptions.SolverResolution.xi(), mOptions.SolverResolution.yi() );

	//init and set render buffer for the data
	glGenRenderbuffersEXT(1, &mRenderbufferDataId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderbufferDataId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mOptions.RenderResolution.xi(), mOptions.RenderResolution.yi() );

	//attach render buffer to frame buffer (duh?)
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Fluid2D::InitPrograms(const std::string &cgHomeDir)
{
	GPUProgramLoader2D loader(cgHomeDir, mCgContext, mCgFragmentProfile);

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
	mRender = loader.Render(mOptions);
}

/** Render and Updates */
void Fluid2D::Update(float time)
{
	if (!ready) return;

	CheckGLError("Before Update");
	PrePostUpdate(true);
	glColor3f(1, 1, 1);

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferDataId);

	//Do the interactiony stuff
	InjectInkStep();
	
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);

	PerturbFluidStep();
	UpdateArbitraryBoundaryStep();
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
		while (mTimeDelta > mOptions.FixedTimeInterval && mLastSolveCount < 10)
		{
			UpdateStep(mOptions.FixedTimeInterval);
			mTimeDelta -= mOptions.FixedTimeInterval;
			mLastSolveCount++;
		}
	}

	PrePostUpdate(false);
	CheckGLError("After Update");
}

void Fluid2D::UpdateStep(float time) 
{
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferId);
	PerturbDensityStep(time);

	BoundaryVelocityStep();
	if (mOptions.GetOption(RS_ADVECT_VELOCITY)) AdvectVelocityStep(time);
	if (mOptions.GetOption(RS_VORTICITY_CONFINEMENT)) VorticityConfinementStep(time);
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

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderbufferDataId);
	if (mOptions.GetOption(RS_ADVECT_DATA)) AdvectDataStep(time);
	if (mOptions.GetOption(RS_DIFFUSE_DATA)) DiffuseDataStep(time);
}

void Fluid2D::Render()
{
	if (!ready) return;	
	
	cgGLEnableProfile(mCgFragmentProfile);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	
	//Bind the program
	mRender->Bind();
	mRender->SetParamTex("velocity", mTextures[velocity]);
	mRender->SetParamTex("data", mTextures[data]);
	mRender->SetParamTex("pressure", mTextures[pressure]);
	mRender->SetParamTex("boundaries", mTextures[boundaries]);
	mRender->SetParam("scale", mOptions.SolverResolution.x / mOptions.RenderResolution.x, mOptions.SolverResolution.y / mOptions.RenderResolution.y);

	glCallList(mFluidCallListId + RenderCallListOffset);

	cgGLDisableProfile(mCgFragmentProfile);
}

void Fluid2D::InjectCheckeredData()
{
	if (!ready) return;
	GLfloat *cpuData = new GLfloat[mOptions.RenderResolution.xi() * mOptions.RenderResolution.yi()*4];

	for (int i=0;i<mOptions.RenderResolution.x;i++)
	{
		for (int j =0;j<mOptions.RenderResolution.y;j++)
		{
			bool trueForCheckeredFalseForRedBlue = true;

			if (trueForCheckeredFalseForRedBlue) {
				cpuData[IndexData(i,j)] = (float)((i % 20 < 10) & (j % 20 < 10));
				cpuData[IndexData(i,j)+1] = (float)((i % 20 > 10) & (j % 20 < 10));
				cpuData[IndexData(i,j)+2] = (float)((i % 20 < 10) & (j % 20 > 10));
			} else {
				cpuData[IndexData(i,j)] = j < mOptions.RenderResolution.y/2.f ? 1.f : 0.f;
				cpuData[IndexData(i,j)+1] = 0;
				cpuData[IndexData(i,j)+2] = j > mOptions.RenderResolution.y/2.f ? 1.f : 0.f;
			}

			//alpha
			cpuData[IndexData(i,j)+3] = 1;
			
		}
	}

	CopyFromCPUtoGPU(GL_TEXTURE_RECTANGLE_ARB, mTextures[data], mOptions.RenderResolution.xi(), mOptions.RenderResolution.yi(), cpuData);
	delete[] cpuData;
	cpuData = 0;
}
void Fluid2D::GenerateCircularVortex()
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
			cpuVelocity[Index(i,j)] = -1*(j - mOptions.SolverResolution.y/2.0f) / (mOptions.SolverResolution.y + 1.0f);
			cpuVelocity[Index(i,j)+1] = 1*(i - mOptions.SolverResolution.x/2.0f) / (mOptions.SolverResolution.x + 1.0f);
			cpuVelocity[Index(i,j)+2] = 0;
			cpuVelocity[Index(i,j)+3] = 0;
		}
	}

	CopyFromCPUtoGPU(GL_TEXTURE_RECTANGLE_ARB, mTextures[velocity], mOptions.SolverResolution.xi(), mOptions.SolverResolution.yi(), cpuVelocity);

	delete[] cpuVelocity;
	cpuVelocity = 0;
}

// Do only if list is not empty, use time based stuff
// It would be good to get the curl here as well, for coolness.
// Should be based time, not frames?
void Fluid2D::Poll(float time)
{
	time=time; // Compiler warning - may be used in future. TODO.
	static float pixels[2];
	// in the velocity texture, and use it when doing the div?
	if (mPollFrame++ % 20 == 0)
	{
		SetOutputTexture(velocity);
		//foreach
		for (VelocityPollerList::iterator it = mVelocityPollers.begin(); it != mVelocityPollers.end(); it++)
		{
			const Vector &position = (*it)->GetPosition() * mOptions.SolverResolution / mOptions.Size;
			glReadPixels(position.xi(), position.yi(), 1, 1, GL_RG, GL_FLOAT, &pixels);
			(*it)->UpdateVelocity(Vector(pixels[0], pixels[1]));
		}
	}
}

/** Steps - Simulation */
void Fluid2D::AdvectDataStep(float time)
{
	if (!ready) return;

	mAdvect->Bind();

	//params
	mAdvect->SetParamTex("velocity", mTextures[velocity]);
	mAdvect->SetParamTex("data", mTextures[data]);
	mAdvect->SetParamTex("boundary", mTextures[boundaries]);
	mAdvect->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, 0, time);
	mAdvect->SetParam("scale", mOptions.SolverToRenderScale.x, mOptions.SolverToRenderScale.y);

	DoCalculationData(data);
}

void Fluid2D::AdvectVelocityStep(float time)
{
	if (!ready) return;

	//params
	mAdvect->Bind();
	mAdvect->SetParamTex("velocity", mTextures[velocity]);
	mAdvect->SetParamTex("data", mTextures[velocity]);
	mAdvect->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, 0, time);
	mAdvect->SetParamTex("boundary", mTextures[boundaries]);
	mAdvect->SetParam("scale", 1, 1);
	
	DoCalculationSolver(velocity);
}

void Fluid2D::VorticityConfinementStep(float time)
{
	if (!ready) return;

	//params
	mVorticity->Bind();
	mVorticity->SetParamTex("velocity", mTextures[velocity]);
	mVorticity->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, 0, time);
	//mVorticity->SetParamTex("boundary", mTextures[boundaries]);

	DoCalculationSolver(velocity);
}

void Fluid2D::DiffuseDataStep(float time)
{
	if (!ready) return;

	float alpha = mOptions.SolverDelta.x*mOptions.SolverDelta.y / (time * mOptions.Viscosity);

	float beta = 4 + alpha;

	mF4Jacobi->Bind();
	mF4Jacobi->SetParam("alpha", alpha);
	mF4Jacobi->SetParam("beta", beta);

	for (int i=0;i<mOptions.DiffuseSteps;i++)
	{
		//if (i != 0) fbo->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, mTextures[output]);
		mF4Jacobi->SetParamTex("x", mTextures[data]);
		mF4Jacobi->SetParamTex("b", mTextures[data]);

		glTranslatef(0, 0, -0.025f);
		DoCalculationData(data);
	}
	glLoadIdentity();
}


void Fluid2D::DiffuseVelocityStep(float time)
{
	if (!ready) return;

	mF4Jacobi->Bind();

	float alpha = mOptions.SolverDelta.x*mOptions.SolverDelta.y / (time * mOptions.Viscosity);

	float beta = 4 + alpha;

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

void Fluid2D::UpdatePressureStep(float time)
{
	if (!ready) return;

	// Calculate Divergence Field 
	mDivField->Bind();

	mDivField->SetParamTex("velocity", mTextures[velocity]);
	mDivField->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, 0, time);

	DoCalculationSolver1D(divField);

	// Find pressure using jacobi iterations
	for (int i=0;i<mOptions.DiffuseSteps;i++)
	{
		mF1Jacobi->Bind();
		mF1Jacobi->SetParam("alpha", -(mOptions.SolverDelta.x * mOptions.SolverDelta.y));
		mF1Jacobi->SetParam("beta", 4.0);
		mF1Jacobi->SetParamTex("b", mTextures[divField]);
		mF1Jacobi->SetParamTex("x", mTextures[pressure]);
		
		glTranslatef(0, 0, -0.025f);
		DoCalculationSolver1D(pressure);

		//bcPressure();
	}
	glLoadIdentity();

}

void Fluid2D::SubtractPressureGradientStep(float time)
{
	if (!ready) return;

	//Subtract the pressure gradient
	mSubtractPressureGradient->Bind();

	mSubtractPressureGradient->SetParamTex("velocity", mTextures[velocity]);
	mSubtractPressureGradient->SetParamTex("pressure", mTextures[pressure]);

	mSubtractPressureGradient->SetParam("d", mOptions.SolverDelta.x, mOptions.SolverDelta.y, 0, time);

	DoCalculationSolver(velocity);
}

/** Steps - Interaction */
void Fluid2D::InjectInkStep()
{
	if (!ready) return;
	if (mInjectors.empty()) return;

	cgGLDisableProfile(mCgFragmentProfile);

	SetOutputTexture(outputRender);

	//render existing ink
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, mTextures[data]);
	glCallList(mFluidCallListId + RenderDataCallListOffset);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	//render ink for each bit
	for (InjectorList::iterator it = mInjectors.begin(); it != mInjectors.end(); ++it)
	{
		const Injector &inj = *it;

		glBlendFunc(GL_ONE, inj.overwrite ? GL_ZERO : GL_ONE);

		Vector d = mOptions.RenderDeltaInv * inj.size ;
		Vector pos = inj.position * mOptions.RenderDeltaInv - d/2;

		glColor4f(inj.color.x, inj.color.y, inj.color.z, 0.8f);
		glBegin(GL_QUADS);
			glVertex3f(pos.x, pos.y, 1);
			glVertex3f(pos.x+d.x, pos.y, 1);
			glVertex3f(pos.x+d.x, pos.y+d.y, 1);
			glVertex3f(pos.x, pos.y+d.y, 1);
		glEnd();
	}
	glColor3f(0,0,0);

	glDisable(GL_BLEND);

	//swap textures
	std::swap(outputRender, data);
	cgGLEnableProfile(mCgFragmentProfile);
	
	mInjectors.clear();
}

void Fluid2D::PerturbFluidStep()
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


void Fluid2D::PerturbDensityStep(float time)
{
	if (!ready) return;

	//params
	mPerturb->Bind();
	mPerturb->SetParamTex("velocity", mTextures[velocity]);
	mPerturb->SetParamTex("data", mTextures[data]);
	mPerturb->SetParam("d", mOptions.SolverToRenderScale.x, mOptions.SolverToRenderScale.y, 0, time);

	DoCalculationSolver(velocity);
}

void Fluid2D::UpdateArbitraryBoundaryStep()
{
	if (!ready) return;
	
	if (mNextBoundaryTexture)
	{
		SetBoundaryTextureStep();
	}

	if (!mBoundaries.empty()) {
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
			Vector d(boundary.size, boundary.size);
			Vector p(boundary.position - d/2);
			d = d / mOptions.Size * mOptions.SolverResolution;
			p = p / mOptions.Size * mOptions.SolverResolution;

			glBegin(GL_QUADS);
				glVertex3f(p.x,		p.y,		1);
				glVertex3f(p.x+d.x,	p.y,		1);
				glVertex3f(p.x+d.x,	p.y+d.y,	1);
				glVertex3f(p.x,		p.y+d.y,	1);
			glEnd();
		}
		glColor3f(0,0,0);

		//swap textures
		std::swap(outputSolver, boundaries);
		cgGLEnableProfile(mCgFragmentProfile);
		
		mBoundaries.clear();
	}
}

/** Steps - Boundaries */
void Fluid2D::BoundaryVelocityStep()
{
	if (!ready) return;

	mF4Boundary->Bind();

	mF4Boundary->SetParamTex("data", mTextures[velocity]);
	mF4Boundary->SetParamTex("boundaries", mTextures[boundaries]);
	mF4Boundary->SetParamTex("offset", mTextures[offset]);
	mF4Boundary->SetParam("scale", -1);

	DoCalculationSolver(velocity);
}

void Fluid2D::BoundaryPressureStep()
{
	if (!ready) return;

	mF1Boundary->Bind();

	mF1Boundary->SetParamTex("data", mTextures[pressure]);
	mF1Boundary->SetParamTex("offset", mTextures[offset]);
	mF1Boundary->SetParam("scale", 1);

	DoCalculationSolver1D(pressure);
}

void Fluid2D::UpdateOffsetStep() {

	if (!ready) return;

	//update offsets
	mOffset->Bind();
	mOffset->SetParam("res", mOptions.SolverResolution.x, mOptions.SolverResolution.y);
	mOffset->SetParamTex("boundaries", mTextures[boundaries]);

	DoCalculationSolver(offset);
}

/** Steps - Optimisation */
void Fluid2D::ZCullStep(bool clearFirst)
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

void Fluid2D::SetColorDensities(float r, float g, float b)
{
	mPerturb->SetParam("densities", r, g, b);
}
void Fluid2D::PrePostUpdate(bool pre)
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
		// FIXME: glOrtho in PreUpdate just seems dodgy...
		if (mOptions.SolverToRenderScale.x > 1)
			glOrtho(0, mOptions.SolverResolution.x, 0, mOptions.SolverResolution.y, -1, 0);
		else
			glOrtho(0, mOptions.RenderResolution.x, 0, mOptions.RenderResolution.y, -1, 0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		// FIXME: glViewport in PreUpdate just seems dodgy...
		if (mOptions.SolverToRenderScale.x > 1)
			glViewport (0, 0, mOptions.SolverResolution.xi(), mOptions.SolverResolution.yi());
		else
			glViewport (0, 0, mOptions.RenderResolution.xi(), mOptions.RenderResolution.yi());
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