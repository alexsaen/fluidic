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
#include "GPUProgramLoader2D.h"

using namespace std;
using namespace Fluidic;

GPUProgramLoader2D::GPUProgramLoader2D(const string &cgHomeDir, CGcontext context, CGprofile profile)
: mCgHomeDir(cgHomeDir), mCgContext(context), mCgFragmentProfile(profile)
{
}

const char *GPUProgramLoader2D::GetPathTo(char *program)
{
	char *path = new char[255];
	sprintf_s(path, 255, "%s%s.cg", mCgHomeDir.c_str(), program);
	return path;
}

GPUProgram *GPUProgramLoader2D::Advect() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "SimpleAdvect2D");
	program->AddParam("velocity");
	program->AddParam("data");
	program->AddParam("d");
	program->AddParam("boundary");
	program->AddParam("scale");
	return program;
}
GPUProgram *GPUProgramLoader2D::Vorticity() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Vorticity"), mCgFragmentProfile, "Vorticity2D");
	program->AddParam("velocity");
	program->AddParam("d");
	return program;
}
GPUProgram *GPUProgramLoader2D::Inject() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Interact"), mCgFragmentProfile, "Inject2D");
	program->AddParam("data");
	program->AddParam("color");
	program->AddParam("scale");
	program->AddParam("p");
	return program;
}
GPUProgram *GPUProgramLoader2D::Perturb() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Interact"), mCgFragmentProfile, "Perturb2D");
	program->AddParam("data");
	program->AddParam("velocity");
	program->AddParam("d");
	program->AddParam("densities");
	return program;
}
GPUProgram *GPUProgramLoader2D::F1Boundary() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "F1Boundary2D");
	program->AddParam("data");
	program->AddParam("offset");
	program->AddParam("scale");
	return program;
}	
GPUProgram *GPUProgramLoader2D::F4Boundary() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "F4Boundary2D");
	program->AddParam("data");
	program->AddParam("offset");
	program->AddParam("scale");
	program->AddParam("boundaries");
	return program;
}
GPUProgram *GPUProgramLoader2D::Offset() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "CalculateOffsets2D");
	program->AddParam("boundaries");
	program->AddParam("res");
	return program;
}	
GPUProgram *GPUProgramLoader2D::F1Jacobi() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Jacobi"), mCgFragmentProfile, "F1Jacobi2D");
	program->AddParam("x");
	program->AddParam("b");
	program->AddParam("alpha");
	program->AddParam("beta");
	return program;
}
GPUProgram *GPUProgramLoader2D::F4Jacobi() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Jacobi"), mCgFragmentProfile, "F4Jacobi2D");
	program->AddParam("x");
	program->AddParam("b");
	program->AddParam("alpha");
	program->AddParam("beta");
	return program;
}
GPUProgram *GPUProgramLoader2D::DivField() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Pressure"), mCgFragmentProfile, "DivField2D");
	program->AddParam("velocity");
	program->AddParam("d");
	return program;
}	
GPUProgram *GPUProgramLoader2D::SubtractPressureGradient() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Pressure"), mCgFragmentProfile, "SubtractPressureGradient2D");
	program->AddParam("pressure");
	program->AddParam("velocity");
	program->AddParam("d");
	return program;
}
GPUProgram *GPUProgramLoader2D::ZCull() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Optimise"), mCgFragmentProfile, "ZCull");
	program->AddParam("data");
	program->AddParam("velocity");
	program->AddParam("boundaries");
	return program;
}

GPUProgram *GPUProgramLoader2D::Render() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Render"), mCgFragmentProfile, "Render2D");
	program->AddParam("data");
	program->AddParam("velocity");
	program->AddParam("pressure");
	program->AddParam("boundaries");
	program->AddParam("scale");
	return program;
}