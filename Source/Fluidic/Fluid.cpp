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

#include "Fluid.h"
#include "Debug.h"
#include "GPUProgram.h"
//fixme I don't need the definition of the class for this? CRAAZY! #include "IVelocityPoller.h"

using namespace std;
using namespace Fluidic;

Fluid::Fluid(std::string cgHomeDir) :
mFramebufferId(0), mRenderbufferId(0), mCurrentBoundTexture(-1), mFluidCallListId(0), 
mPollFrame(0), mCgHomeDir(cgHomeDir)
{
	mCgContext = cgCreateContext();
	mCgFragmentProfile = CG_PROFILE_FP40;
	ready = 0;
}

Fluid::~Fluid(void)
{
	DestroyBuffers();
	cgDestroyContext(mCgContext);
}

/** Initialization Stuff */
void Fluid::Init(const FluidOptions &options)
{
	mOptions = options;

	//update the deltas
	mOptions.RenderDelta = mOptions.Size / mOptions.RenderResolution;
	mOptions.SolverDelta = mOptions.Size / mOptions.SolverResolution;
	mOptions.RenderDeltaInv = mOptions.RenderResolution / mOptions.Size;
	mOptions.SolverDeltaInv = mOptions.SolverResolution / mOptions.Size;
	mOptions.SolverToRenderScale = mOptions.SolverResolution / mOptions.RenderResolution;

	if (!ready) InitPrograms(mCgHomeDir);
	CheckGLError("");

	ready = 0;

	InitCallLists();
	CheckGLError("");

	InitTextures();
	CheckGLError("");

	InitBuffers();
	CheckGLError("");

	CheckFramebufferStatus();
	ready = 1;
}

void Fluid::DestroyBuffers()
{
  glDeleteRenderbuffersEXT(1, &mRenderbufferId);
  glDeleteFramebuffersEXT(1, &mFramebufferId);
  mRenderbufferId = mFramebufferId = 0;
}

void Fluid::SetupTexture(GLuint texId, GLuint internalFormat, Vector resolution, int components, char *initialData)
{
	// Set up OpenGL Formats
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);

	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Initialise it
	GLuint format = 
		components == 1 ? GL_RED : 
		components == 2 ? GL_RG : 
		components == 3 ? GL_RGB : 
		components == 4 ? GL_RGBA : 0;

	if (format == 0) return;

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat, resolution.xi(), resolution.yi(), 0, format, GL_FLOAT, &initialData[0]);
}

/** Interactions */
void Fluid::Inject(const Vector &position, float r, float g, float b, float size, bool overwrite)
{
	Injector injector;
	injector.color = Vector(r, g, b);
	injector.position = position;
	injector.size = size;
	injector.overwrite = overwrite;
	mInjectors.push_back(injector);
}

void Fluid::Perturb(const Vector &position, const Vector &velocity, float size)
{
	
	Perturber perturber;
	perturber.velocity = velocity;
	perturber.position = position;
	perturber.size = size;
	mPerturbers.push_back(perturber);
}

void Fluid::AddArbitraryBoundary(const Vector &position, float size) 
{
	Boundary boundary;
	boundary.position = position;
	boundary.size = size;
	mBoundaries.push_back(boundary);
}

void Fluid::AttachPoller(IVelocityPoller *poller)
{
	mVelocityPollers.push_back(poller);
}

void Fluid::DetachPoller(IVelocityPoller *poller)
{
	mVelocityPollers.remove(poller);
}

// FIXME: Fluid::Poll 
// Do only if list is not empty, use time based stuff
// It would be good to get the curl here as well, for coolness.
// Should be based time, not frames?
void Fluid::Poll(float time)
{
	time=time; // Compiler warning - may be used in future. TODO.
/*
	static float pixels[3];
	// in the velocity texture, and use it when doing the div?
	if (mPollFrame++ % 20 == 0)
	{
		SetOutputTexture(velocity);
		//foreach
		for (VelocityPollerList::iterator it = mVelocityPollers.begin(); it != mVelocityPollers.end(); it++)
		{
			const Vector &position = (*it)->GetPosition() * mOptions.SolverResolution / mOptions.Size;
			glReadPixels(position.xi(), position.yi(), 1, 1, GL_RGB, GL_FLOAT, &pixels);
			(*it)->UpdateVelocity(Vector(pixels[0], pixels[1], pixels[2]));
		}
	}*/
}

/** Helpers */
void Fluid::SetOutputTexture(const int textureIndex)
{
	if (mCurrentBoundTexture != textureIndex)
	{
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB,  mTextures[textureIndex], 0 );
		mCurrentBoundTexture = textureIndex;
	}
}

void Fluid::DoCalculationSolver(int &textureIndex)
{
	SetOutputTexture(outputSolver);
	glCallList(mFluidCallListId + SolverCallListOffset);
	std::swap(outputSolver, textureIndex);
}
void Fluid::DoCalculationSolver1D(int &textureIndex)
{
	SetOutputTexture(outputSolver1d);
	glCallList(mFluidCallListId + SolverCallListOffset);
	std::swap(outputSolver1d, textureIndex);
}
void Fluid::DoCalculationData(int &textureIndex)
{
	SetOutputTexture(outputRender);
	glCallList(mFluidCallListId + RenderDataCallListOffset);
	std::swap(outputRender, textureIndex);
}

/** Accessors and Mutators */
Vector Fluid::GetSize()
{
	return mOptions.Size;
}
void Fluid::SetSize(Vector size)
{
	ready = 0;
	mOptions.Size = size;
}

Vector Fluid::GetResolution()
{
	return mOptions.SolverResolution;
}
void Fluid::SetResolution(Vector resolution)
{
	ready = 0;
	mOptions.SolverResolution = resolution;
}

/* FIXME I reckon this can be abstracted
void Fluid::SetColorDensities(float r, float g, float b)
{
	mPerturb->SetParam("densities", r, g, b);
}
/**/

void Fluid::CopyFromCPUtoGPU(GLuint textureTarget, GLuint texId, int x, int y, float *cpuData)
{
	CheckGLError("Before Bind Textures");
	glBindTexture(textureTarget, texId);
	CheckGLError("Bind Textures");

	glTexSubImage2D(textureTarget, 0, 0, 0, x, y, GL_RGBA, GL_FLOAT, cpuData);
	CheckGLError("TexSubImage");
}
