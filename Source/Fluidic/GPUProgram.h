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