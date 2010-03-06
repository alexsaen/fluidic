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

		int startTime;
		int previousTime;
		int currFrame;

	};
}