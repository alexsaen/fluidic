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