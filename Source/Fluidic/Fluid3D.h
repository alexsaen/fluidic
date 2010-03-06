/*
Copyright (c) 2010, Steven Leigh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Steven Leigh BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "Fluid.h"

namespace Fluidic
{
	/**
	 * \brief Sets up, solves and renders a 2d fluid in real time using the GPU.
	 */
	class Fluid3D : public Fluid
	{
	public:

		Fluid3D(std::string cgHomeDir);
		~Fluid3D(void);

		void SetColorDensities(float r, float g, float b);

		void GenerateCircularVortex();
		void InjectCheckeredData();
		void Update(float time);
		void Render();

	private:
		inline int Index(int x, int y, int z) {
			//transform 3d to 1d index (commenting - easier to read than above #define)
			return 4 * (
				x + // x coord INSIDE slab
				8 * mOptions.SolverResolution.xi() * y + // y coord INSIDE slab
				(z % 8) * mOptions.SolverResolution.xi() + // moves slab to appropriate x-slab offset
				(z / 8) * 8 * mOptions.SolverResolution.xi() * mOptions.SolverResolution.yi()// moves slab to appropriate y-slab offset
			); 
		}
		
		inline Vector Coords2D(Vector pos) {
			//transform 3d coords to 2d coords (commenting - easier to read than above #define)
			return Vector(
				pos.x + (pos.zi() % 8)* mOptions.SolverResolution.xi(),
				pos.y + (pos.zi() / 8)* mOptions.SolverResolution.yi()
			); 
		}


		// Methods...
		void InitCallLists();
		void InitPrograms(const std::string &cgHomeDir);
		void SetGlobalProgramParams();
		void InitTextures();
		void InitBuffers();

		void Poll(float time);

		void ZCullStep(bool clearFirst);

		void AdvectDataStep(float time);
		void AdvectVelocityStep(float time);

		void VorticityConfinementStep(float time);

		void DiffuseDataStep(float time);
		void DiffuseVelocityStep(float time);

		void UpdatePressureStep(float time);
		void SubtractPressureGradientStep(float time);

		void PerturbDensityStep(float time);

		void UpdateArbitraryBoundaryStep();
		void UpdateOffsetStep();
		void BoundaryVelocityStep();
		void BoundaryPressureStep();

		void InjectInkStep();
		void PerturbFluidStep();

		void PrePostUpdate(bool pre);

		inline void GLCubeVertex(float x, float y, float z)
		{
			float x1 = x > 0.f ? 1.f : 0.f;
			float y1 = y > 0.f ? 1.f : 0.f;
			float z1 = z > 0.f ? 1.f : 0.f;
			glColor3f(x1, y1, z1);
			glTexCoord3f(x1, y1, z1);
			glVertex3f(x, y, z);
		}

		Vector mSlabs; // 2d, number of slabs for texture atlas
		CGprofile mCgVertexProfile; ///< vertex profile
		
		// Additional programs for rendering
		GPUProgram *mRaycastVProgram;
		GPUProgram *mRaycastFProgram;
		GPUProgram *mIlluminate;

		// Additional texture for rendering
		int backface;
	};
}