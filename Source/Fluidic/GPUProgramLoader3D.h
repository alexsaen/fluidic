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

#include "GPUProgram.h"
#include <string>

namespace Fluidic
{
	/**
	 * Abstraction for GPU Programs. Loads program from a file, and sets params.
	 * This is more for convenience than speed - it's actually somewhat slower in the param lookups.
	 */
	class GPUProgramLoader3D
	{
	public:
		GPUProgramLoader3D(const std::string &cgHomeDir, CGcontext context, CGprofile vertexProfile, CGprofile fragmentProfile);
		GPUProgram *Advect();
		GPUProgram *Vorticity();
		GPUProgram *Inject();
		GPUProgram *Perturb();
		GPUProgram *F1Boundary();
		GPUProgram *F4Boundary();
		GPUProgram *Offset();
		GPUProgram *F1Jacobi();
		GPUProgram *F4Jacobi();
		GPUProgram *DivField();
		GPUProgram *SubtractPressureGradient();
		GPUProgram *ZCull();
		GPUProgram *Render();

		GPUProgram *RayCastVertex();
		GPUProgram *RayCastFragment();

	protected:
		const char *GetPathTo(char *program);
	private:
		std::string mCgHomeDir;
		CGcontext mCgContext;
		CGprofile mCgVertexProfile;
		CGprofile mCgFragmentProfile;
	};

}