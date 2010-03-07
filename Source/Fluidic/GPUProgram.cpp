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
#include "GPUProgram.h"

using namespace Fluidic;
using namespace std;

GPUProgram::GPUProgram(void)
: mProgram(NULL)
{
}

GPUProgram::~GPUProgram(void)
{
	mParamMap.clear();
	cgDestroyProgram(mProgram);
}

void GPUProgram::SetProgram(CGcontext context, const char *filename, CGprofile profile, char *programName) 
{
	mProgram = cgCreateProgramFromFile(
		context,
		CG_SOURCE,
		filename,
		profile,
		programName,
		0 );

	if (mProgram == 0) throw "Could not load program";
	cgGLLoadProgram(mProgram);
}

CGparameter GPUProgram::AddParam(char *paramName) 
{
	CGparameter param = cgGetNamedParameter(mProgram, paramName);
	if (param == 0) throw "Invalid parameter specified";
	return mParamMap[paramName] = param;
}

void GPUProgram::Bind()
{
	cgGLBindProgram(mProgram);
}

void GPUProgram::SetParam(char *paramName, float a) 
{
	cgSetParameter1f(GetParam(paramName), a);
}

void GPUProgram::SetParam(char *paramName, float a, float b) 
{
	cgSetParameter2f(GetParam(paramName), a, b);
}

void GPUProgram::SetParam(char *paramName, float a, float b, float c)
{

	cgSetParameter3f(GetParam(paramName), a, b, c);
}

void GPUProgram::SetParam(char *paramName, float a, float b, float c, float d)
{

	cgSetParameter4f(GetParam(paramName), a, b, c, d);
}

void GPUProgram::SetParamTex(char *paramName, GLuint texId) 
{
	CGparameter param = GetParam(paramName);
	cgGLSetTextureParameter(param, texId);
	cgGLEnableTextureParameter(param);
}