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
	//program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "SimpleAdvect3D");
	program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "Advect3D");
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