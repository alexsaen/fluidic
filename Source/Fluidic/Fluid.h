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

#pragma once

#include <GL/glew.h>
#include <cg/cg.h>
#include <cg/cgGL.h>
#include <list>
#include <string>

#include "FluidOptions.h"
#include "Vector.h"

namespace Fluidic
{
	class GPUProgram;
	class IVelocityPoller;

	/**
	 * \brief Sets up, solves and renders a fluid in real time using the GPU.
	 */
	class Fluid
	{
	public:
		/**
		 * \brief Constructor
		 *
		 * @param context cg context to use for the fluid
		 * @param profile cg profile to use for the fluid solving. Must be PS3 or higher
		 * @param cgHomeDir the directory to load the render programs from (TODO: Move into internal resource file)
		 */
		Fluid(std::string cgHomeDir);
		~Fluid(void);

		/**
		 * \brief Sets up the fluid with the given options. Can be called at any time. Will reset the fluid
		 */
		void Init(const FluidOptions &options);

		/// Returns the size of the fluid
		Vector GetSize();
		/// Sets the size of the fluid
		void SetSize(Vector size);

		/// Returns the solver resolution of the fluid
		Vector GetResolution();
		/// Sets the solver resolution of the fluid
		void SetResolution(Vector resolution);

		/// Sets the densities of each colour in the fluid
		virtual void SetColorDensities(float r, float g, float b)=0;

		/**
		 * \brief Injects a square of ink of the given color/size at the given location
		 *
		 * @param position the position of ink
		 * @param r the red component of color
		 * @param g the green component of color
		 * @param b the blue component of color
		 * @param size the size of the box
		 * @param overwrite true to overwrite value, false to blend
		 */
		void Inject(const Vector &position, float r, float g, float b, float size, bool overwrite);

		/**
		 * \brief Perturbs a fluid at a position/radius
		 *
		 * @param position the position of perturbation
		 * @param velocity the velocity of perturbation
		 * @param size the radius of the circle
		 */		
		void Perturb(const Vector &position, const Vector &velocity, float size);

		/**
		 * \brief Adds a square boundary of size at a location
		 *
		 * @param position the position of boundary
		 * @param size the size of the box
		 */
		void AddArbitraryBoundary(const Vector &position, float size);

		/**
		 * \brief Generates a circular vortex. Warning: This is a slow method - it creates it on the
		 * CPU and copies to the GPU.
		 */
		virtual void GenerateCircularVortex()=0;

		/**
		 * \brief Generates checkered ink data. Warning: This is a slow method - it creates it on the
		 * CPU and copies to the GPU.
		 */
		virtual void InjectCheckeredData()=0;

		/**
		 * \brief Steps the fluid by a time step
		 *
		 * @param time the time in seconds to step the fluid
		 */
		virtual void Update(float time)=0;

		/**
		 * \brief Renders the fluid
		 */
		virtual void Render()=0;

		/**
		 * \brief Attaches a poller to the fluid
		 */
		void AttachPoller(IVelocityPoller *poller);

		/**
		 * \brief Detaches a poller from the fluid
		 */
		void DetachPoller(IVelocityPoller *poller);

	protected:
		static const int SolverCallListOffset = 0;
		static const int RenderDataCallListOffset = 1;
		static const int RenderCallListOffset = 2;

		// Methods
		virtual void InitCallLists() = 0;
		virtual void InitPrograms(const std::string &cgHomeDir) = 0;
		virtual void InitTextures() = 0;
		virtual void InitBuffers() = 0;
		void DestroyBuffers();
		
		void SetupTexture(GLuint texId, GLuint internalFormat, Vector resolution, int components, char *initialData);

		void SetOutputTexture(const int textureIndex);
		void DoCalculationSolver(int &textureIndex);
		void DoCalculationSolver1D(int &textureIndex);
		void DoCalculationData(int &textureIndex);

		static void CopyFromCPUtoGPU(GLuint textureTarget, GLuint texId, int texSizeX, int texSizeY, float *cpuData);

		FluidOptions mOptions;
		int ready;

		struct Injector {
			Vector position;
			Vector color;
			float size;
			bool overwrite;
		};
		typedef std::list<Injector> InjectorList;
		InjectorList mInjectors;
		
		struct Perturber {
			Vector position;
			Vector velocity;
			float size;
		};
		typedef std::list<Perturber> PerturberList;
		PerturberList mPerturbers;

		struct Boundary {
			Vector position;
			float size;
		};
		typedef std::list<Boundary> BoundaryList;
		BoundaryList mBoundaries;

		int mPollFrame;
		typedef std::list<IVelocityPoller*> VelocityPollerList;
		VelocityPollerList mVelocityPollers;

		GLuint mFluidCallListId;

		CGcontext mCgContext;
		CGprofile mCgFragmentProfile;
		std::string mCgHomeDir;

		GLenum mAttachmentPoint;

		GLuint mRenderbufferId;
		GLuint mRenderbufferDataId;
		GLuint mFramebufferId;

		int mCurrentBoundTexture;

		// Simulation Programs
		GPUProgram *mAdvect;
		GPUProgram *mVorticity;
		GPUProgram *mInject;
		GPUProgram *mF1Boundary;
		GPUProgram *mF4Boundary;
		GPUProgram *mF1Jacobi;
		GPUProgram *mF4Jacobi;
		GPUProgram *mDivField;
		GPUProgram *mRender;
		GPUProgram *mSubtractPressureGradient;
		GPUProgram *mOffset;
		GPUProgram *mPerturb;
		GPUProgram *mZCull;

		/// Textures
		GLuint *mTextures;
		int outputSolver;
		int outputSolver1d;
		int outputRender;

		// Solver (3-component)
		int velocity;
		int boundaries;
		int offset;

		// Solver (1-component)
		int pressure;
		int divField;

		// Data (aka Ink/density) Textures
		int data;
	};
}