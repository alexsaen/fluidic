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
	//program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "SimpleAdvect2D");
	program->SetProgram(mCgContext, GetPathTo("Advect"), mCgFragmentProfile, "Advect2D");
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