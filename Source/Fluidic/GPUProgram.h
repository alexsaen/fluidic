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

#include <GL/glew.h>
#include <cg/cg.h>
#include <cg/cgGL.h>

#include <string>
#include <map>

namespace Fluidic
{
	/**
	 * Abstraction for GPU Programs. Loads program from a file, and sets params.
	 * This is more for convenience than speed - it's actually somewhat slower in the param lookups.
	 */
	class GPUProgram
	{
	public:
		GPUProgram();
		~GPUProgram();

		/**
		 * Loads the given program from a file into the context.
		 *
		 * @param context context to load the program into
		 * @param filename the filename to load
		 * @param profile the cg profile to use
		 * @param programName the name of the program
		 */
		void SetProgram(CGcontext context, const char *filename, CGprofile profile, char *programName);

		/**
		 * Adds a parameter definition to the given program
		 * 
		 * @param paramName name of the parameter
		 * @return CGparameter the handler to the parameter
		 */
		CGparameter AddParam(char *paramName);

		/**
		 * Sets a 1 component parameter value
		 *
		 * @param paramName name of the parameter
		 * @param a the value of the 1st component to set
		 */
		void SetParam(char *paramName, float a);

		/**
		 * Sets a 2 component parameter value
		 *
		 * @param paramName name of the parameter
		 * @param a the value of the 1st component to set
		 * @param b the value of the 2nd component to set
		 */
		void SetParam(char *paramName, float a, float b);

		/**
		 * Sets a 3 component parameter value
		 *
		 * @param paramName name of the parameter
		 * @param a the value of the 1st component to set
		 * @param b the value of the 2nd component to set
		 * @param c the value of the 3rd component to set
		 */
		void SetParam(char *paramName, float a, float b, float c);

		/**
		 * Sets a 4 component parameter value
		 *
		 * @param paramName name of the parameter
		 * @param a the value of the 1st component to set
		 * @param b the value of the 2nd component to set
		 * @param c the value of the 3rd component to set
		 * @param d the value of the 4th component to set
		 */
		void SetParam(char *paramName, float a, float b, float c, float d);


		/**
		 * Sets a 1 texture parameter value
		 *
		 * @param paramName name of the parameter
		 * @param texId the texture id for this parameter
		 */
		void SetParamTex(char *paramName, GLuint texId);

		/**
		 * Binds this program
		 */
		void Bind();
	private:

		typedef std::map<std::string, CGparameter> ParamMap;
		ParamMap mParamMap;
		CGprogram mProgram;
		
		inline CGparameter GetParam(char *paramName)
		{
			ParamMap::iterator it = mParamMap.find(paramName);
			CGparameter param;
			if (it == mParamMap.end()) {
				//param = AddParam(paramName);
				throw "Param not found";
			} else {
				param = it->second;
			}
			
			return param;
		}
	};
}