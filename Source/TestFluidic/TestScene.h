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
#include <fstream>

#include "Fluidic.h"
#include "TestObject.h"

namespace TestFluidic
{
#ifdef _DEBUG
#define CG_PROGRAM_DIR "../../../../Resources/"
#else
// FIXME: Either parametize this, or set up a deployment area so the dir structure isn't crazy
#define CG_PROGRAM_DIR "../Resources/"
#endif

		//Mouse Button state
		enum MouseButton 
		{
			MB_NONE = 0,
			MB_LEFT = 1,
			MB_RIGHT = 2,
			MB_MIDDLE = 4
		};

		struct InputState 
		{
			int buttons;
			int dx; //change in x
			int dy; //change in y
			int x;
			int y;
			int modifiers;

			InputState() 
			{
				buttons = MB_NONE;
				modifiers = 0;
				dx = dy = x = y = 0;
			}
		};

	class TestScene
	{
	public:
		TestScene();
		virtual ~TestScene();

		void Tick();

		virtual void Interact(){}
		virtual void Update(float time);
		virtual void Display(){};

		virtual void HandleMouseMove(int x, int y);
		virtual void HandleMouseButton(int button,int state,int x,int y);
		virtual void HandleKeyboard(unsigned char key, bool down);
		virtual void HandleKeyboardSpecial(int key);
		
		virtual void Resize(int width, int height);

	protected:
		InputState mouseState;

		Fluidic::Fluid *fluid;
		Fluidic::FluidOptions options;

		void InitFluid();

		int fluidTextCallList;
		float fluidTextR, fluidTextG, fluidTextB;
		void DrawFluidText();
		void BuildFluidTextCallList();

		int solveCount;
		int startTime;
		int previousTime;
		int currFrame;

	};
}