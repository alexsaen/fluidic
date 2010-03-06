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
#include "GPUProgramLoader3D.h"

using namespace std;
using namespace Fluidic;

GPUProgramLoader3D::GPUProgramLoader3D(const string &cgHomeDir, CGcontext context, CGprofile vertexProfile, CGprofile fragmentProfile)
: mCgHomeDir(cgHomeDir), mCgContext(context), mCgVertexProfile(vertexProfile), mCgFragmentProfile(fragmentProfile)
{
}

const char *GPUProgramLoader3D::GetPathTo(char *program)
{
	char *path = new char[255];
	sprintf_s(path, 255, "%s%s.cg", mCgHomeDir.c_str(), program);
	return path;
}

GPUProgram *GPUProgramLoader3D::Advect() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "SimpleAdvect3D");
	program->AddParam("velocity");
	program->AddParam("data");
	program->AddParam("d");
	program->AddParam("boundary");
	//program->AddParam("scale");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::Vorticity() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Vorticity"), mCgFragmentProfile, "Vorticity3D");
	program->AddParam("velocity");
	program->AddParam("d");
	program->AddParam("res");
	program->AddParam("slabs");
	//program->AddParam("boundary");
	//program->AddParam("scale");
	return program;
}
GPUProgram *GPUProgramLoader3D::Inject() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Interact"), mCgFragmentProfile, "Inject3D");
	program->AddParam("data");
	program->AddParam("color");
	//program->AddParam("scale");
	//program->AddParam("p");
	return program;
}
GPUProgram *GPUProgramLoader3D::Perturb() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Interact"), mCgFragmentProfile, "Perturb3D");
	program->AddParam("data");
	program->AddParam("velocity");
	program->AddParam("d");
	program->AddParam("densities");
	//program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::F1Boundary() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "F1Boundary3D");
	program->AddParam("data");
	program->AddParam("offset");
	program->AddParam("scale");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}	
GPUProgram *GPUProgramLoader3D::F4Boundary() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "F4Boundary3D");
	program->AddParam("data");
	program->AddParam("offset");
	program->AddParam("scale");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::Offset() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Boundary"), mCgFragmentProfile, "CalculateOffsets3D");
	program->AddParam("boundaries");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}	
GPUProgram *GPUProgramLoader3D::F1Jacobi() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Jacobi"), mCgFragmentProfile, "F1Jacobi3D");
	program->AddParam("x");
	program->AddParam("b");
	program->AddParam("alpha");
	program->AddParam("beta");
	//program->AddParam("boundary");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::F4Jacobi() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Jacobi"), mCgFragmentProfile, "F4Jacobi3D");
	program->AddParam("x");
	program->AddParam("b");
	program->AddParam("alpha");
	program->AddParam("beta");
	//program->AddParam("boundary");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::DivField() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Pressure"), mCgFragmentProfile, "DivField3D");
	program->AddParam("velocity");
	program->AddParam("d");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}	
GPUProgram *GPUProgramLoader3D::SubtractPressureGradient() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Pressure"), mCgFragmentProfile, "SubtractPressureGradient3D");
	program->AddParam("pressure");
	program->AddParam("velocity");
	program->AddParam("d");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}
GPUProgram *GPUProgramLoader3D::ZCull() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Optimise"), mCgFragmentProfile, "ZCull");
	program->AddParam("data");
	program->AddParam("velocity");
	program->AddParam("boundaries");
	return program;
}

GPUProgram *GPUProgramLoader3D::RayCastVertex() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Render"), mCgVertexProfile, "RaycastVProgram3D");
	return program;
}

GPUProgram *GPUProgramLoader3D::RayCastFragment() 
{
	GPUProgram *program = new GPUProgram();
	program->SetProgram(mCgContext, GetPathTo("Render"), mCgFragmentProfile, "RaycastFProgram3D");
	program->AddParam("backface_tex");
	program->AddParam("volume_tex");
	program->AddParam("stepsize");
	program->AddParam("screenRes");
	program->AddParam("res");
	program->AddParam("slabs");
	return program;
}

GPUProgram *GPUProgramLoader3D::Render() 
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