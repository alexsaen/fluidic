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