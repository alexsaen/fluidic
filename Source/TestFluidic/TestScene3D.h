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
#include <iostream>
#include <fstream>

#include "TestScene.h"
#include "TestObject.h"

namespace TestFluidic
{
	
	struct Camera {
		float x, y, z;
		float vx, vy, vz;
		float pitch, yaw, roll;

		Camera() 
		{
			vx = vy = vz = 0;
		}
	};

	class TestScene3D : public TestScene
	{
	public:
		TestScene3D();
		~TestScene3D();

		void Interact();
		void Update(float time);
		void Display();
		
		void HandleKeyboard(unsigned char key, bool down);
		void HandleKeyboardSpecial(int key);

		void Resize(int w, int h);

	protected:
		bool moving;
		CGcontext cgContext;
		CGprofile cgFragmentProfile;
		CGprofile cgVertexProfile;
		
		Camera camera;
		float injectZPos;
		
		TestObject *obj;

		float degToRad(float degrees)
		{
			return (float)(degrees * 3.141592654 / 180);
		}

		float radToDeg(float radians)
		{
			return (float)(radians * 180 / 3.141592645);
		}
	};
}