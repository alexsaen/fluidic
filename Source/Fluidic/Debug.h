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

#include <iostream>
#include <GL/glu.h>
#include "FluidException.h"

namespace Fluidic
{
	/**
	 * Throws an exception if there's an opengl error
	 */
	inline void CheckGLError(const char *label)
	{
#ifdef _DEBUG
		GLenum errCode;
		if ((errCode = glGetError()) != GL_NO_ERROR)
		{
			const GLubyte *errStr;
			errStr = gluErrorString(errCode);
			std::string errString((char*)errStr);
			errString = std::string("OpenGL Error (Label is '") + std::string(label) + "'): " + std::string((char*)errStr);
			throw FluidException(errString);
		}
#endif
	}

	/**
	 * Throws an exception if the framebuffer is in a bad state
	 */
	inline bool CheckFramebufferStatus()
	{
#ifdef _DEBUG
		GLenum status;
		status=(GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status) {
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return true;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			throw FluidException("Framebuffer incomplete,incomplete attachment\n");
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			throw FluidException("Unsupported framebuffer format\n");
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			throw FluidException("Framebuffer incomplete,missing attachment\n");
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			throw FluidException("Framebuffer incomplete,attached images must have same dimensions\n");
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			throw FluidException("Framebuffer incomplete,attached images must have same format\n");
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			throw FluidException("Framebuffer incomplete,missing draw buffer\n");
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			throw FluidException("Framebuffer incomplete,missing read buffer\n");
		}
		throw FluidException("Unknown FrameBuffer status error");
#else
		return true;
#endif
	}

}