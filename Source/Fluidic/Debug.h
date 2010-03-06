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