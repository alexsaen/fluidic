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
    * Neither the name "Fluidic" nor the
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
	class Fluid2D : public Fluid
	{
	public:

		Fluid2D(std::string cgHomeDir);
		~Fluid2D(void);

		void SetColorDensities(float r, float g, float b);

		void GenerateCircularVortex();
		void InjectCheckeredData();
		void Update(float time);
		void Render();

	private:
		// Methods...
		void InitCallLists();
		void InitPrograms(const std::string &cgHomeDir);
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

		inline int Index(int x, int y) {
			return 4 * (x + mOptions.SolverResolution.xi() * y);
		}
		inline int IndexData(int x, int y) {
			return 4 * (x + mOptions.RenderResolution.xi() * y);
		}

	};
}