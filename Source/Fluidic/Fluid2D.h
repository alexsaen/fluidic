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

		static FluidOptions DefaultOptions();

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