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