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

#include "Vector.h"

namespace Fluidic
{
	enum SolverOptionsFlags {
		RS_NONE = 0,
		RS_ADVECT_VELOCITY = 1,
		RS_ADVECT_DATA = 2,
		RS_DIFFUSE_VELOCITY = 4,
		RS_DIFFUSE_DATA = 8,
		RS_VORTICITY_CONFINEMENT = 16,
		RS_ZCULL = 32,
		RS_DOUBLE_PRECISION = 64,

		RS_PERFECT = RS_ADVECT_VELOCITY | RS_ADVECT_DATA | RS_DIFFUSE_VELOCITY,
		RS_ACCURATE = RS_ADVECT_VELOCITY | RS_ADVECT_DATA | RS_DIFFUSE_VELOCITY | RS_ZCULL,
		RS_NICE = RS_ADVECT_VELOCITY | RS_ADVECT_DATA | RS_VORTICITY_CONFINEMENT,
		RS_FAST = RS_ADVECT_VELOCITY | RS_ADVECT_DATA | RS_VORTICITY_CONFINEMENT | RS_ZCULL,
		RS_UGLY = RS_ADVECT_VELOCITY | RS_ADVECT_DATA | RS_ZCULL,
	};

	const float ViscosityAir = 0.0000178f;
	const float ViscosityWater = 0.0009f;
	const float ViscosityOliveOil = 0.081f;
	const float ViscosityPitch = 23000000.f;

	/**
	 * Options for the fluid
	 */
	struct FluidOptions
	{
		/// The rate at which the fluid diffuses.
		float Viscosity;

		/// Resolution the fluid will render at
		Vector RenderResolution;

		/// Resolution the fluid will be solved at
		Vector SolverResolution;

		/// The width and height of the fluid
		Vector Size;

		/// The steps to use in solving the fluid (uses SolverOptionsFlags)
		int SolverOptions;

		/**
		 * Returns true if the given option is set
		 *
		 * @param option the solver option to check
		 * @return true if option is set
		 */
		bool GetOption(SolverOptionsFlags option);

		// Note: The following cannot be relied on outside of the Fluid class
		Vector RenderDelta;
		Vector SolverDelta;
		Vector RenderDeltaInv; //1/RenderDelta, for optimisation purposes
		Vector SolverDeltaInv; //1/SolverDelta, for optimisation purposes
		Vector SolverToRenderScale; //2 means solver res is double render res, for example

	};

	inline bool FluidOptions::GetOption(SolverOptionsFlags option) 
	{ 
		return ((SolverOptions & option) == option); 
	}

}