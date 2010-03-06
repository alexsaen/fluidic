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
#include "TestScene.h"
#include <GL/glut.h>
#include <iostream>

using namespace std;
using namespace TestFluidic;

TestScene::TestScene()
: fluidTextR(0), fluidTextG(0.5), fluidTextB(1), previousTime(0), currFrame(0)
{
	BuildFluidTextCallList();
}

TestScene::~TestScene()
{
}

void TestScene::BuildFluidTextCallList()
{

	/// Generate a list for the word "FLUID" - to test blending
	fluidTextCallList = glGenLists(1);

	glNewList(fluidTextCallList, GL_COMPILE);
	//F
	glBegin(GL_QUADS);
	glVertex2f(0, 1);
	glVertex2f(0, 0);
	glVertex2f(0.05, 0);
	glVertex2f(0.05, 1);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(0.05, 1);
	glVertex2f(0.05, 0.8);
	glVertex2f(0.15, 0.8);
	glVertex2f(0.15, 1);
	glEnd();	
	glBegin(GL_QUADS);
	glVertex2f(0.05, 0.6);
	glVertex2f(0.05, 0.4);
	glVertex2f(0.10, 0.4);
	glVertex2f(0.10, 0.6);
	glEnd();

	//L
	glBegin(GL_POLYGON);
	glVertex2f(0.25, 0.2);
	glVertex2f(0.25, 1);
	glVertex2f(0.20, 1);
	glVertex2f(0.20, 0);
	glVertex2f(0.35, 0);
	glVertex2f(0.35, 0.2);
	glEnd();

	//U
	glBegin(GL_POLYGON);
	glVertex2f(0.45, 0.2);
	glVertex2f(0.45, 1);
	glVertex2f(0.4, 1);
	glVertex2f(0.4, 0);
	glVertex2f(0.55, 0);
	glVertex2f(0.55, 0.2);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(0.55, 0);
	glVertex2f(0.55, 1);
	glVertex2f(0.60, 1);
	glVertex2f(0.60, 0);
	glEnd();

	//I
	glBegin(GL_QUADS);
	glVertex2f(0.65, 0);
	glVertex2f(0.65, 1);
	glVertex2f(0.70, 1);
	glVertex2f(0.70, 0);
	glEnd();

	//D
	glBegin(GL_QUADS);
	glVertex2f(0.75, 0);
	glVertex2f(0.75, 1);
	glVertex2f(0.80, 1);
	glVertex2f(0.80, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.8, 0.2);
	glVertex2f(0.8, 0);
	glVertex2f(0.85, 0);
	glVertex2f(0.95, 0.5);
	glVertex2f(0.875, 0.5); //inner point
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.875, 0.5); //inner point
	glVertex2f(0.95, 0.5);
	glVertex2f(0.85, 1);
	glVertex2f(0.8, 1);
	glVertex2f(0.8, 0.8);
	glEnd();
	
	//I
	glBegin(GL_QUADS);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(1.05, 1);
	glVertex2f(1.05, 0);
	glEnd();
		
	//C
	glBegin(GL_QUADS);
	glVertex2f(1.1, 0);
	glVertex2f(1.1, 1);
	glVertex2f(1.15, 1);
	glVertex2f(1.15, 0);
	glEnd();
	
	glBegin(GL_QUADS);
	glVertex2f(1.15, 0);
	glVertex2f(1.15, 0.2);
	glVertex2f(1.25, 0.2);
	glVertex2f(1.25, 0);
	glEnd();
	
	glBegin(GL_QUADS);
	glVertex2f(1.15, 0.8);
	glVertex2f(1.15, 1);
	glVertex2f(1.25, 1);
	glVertex2f(1.25, 0.8);
	glEnd();

	glEndList();
}

void TestScene::DrawFluidText() 
{
	// Since it was tacked on later, it no longer fits in (1,1), so scale it down
	glScalef(0.8, 1, 1);
	glColor4f(
		fluidTextR < 1 ? fluidTextR : 2-fluidTextR,
		fluidTextG < 1 ? fluidTextG : 2-fluidTextG,
		fluidTextB < 1 ? fluidTextB : 2-fluidTextB,
		1);
	glCallList( fluidTextCallList );
}


void TestScene::Tick()
{
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int elapsedTime = currTime - previousTime;
	previousTime = currTime;

	float dt = 0.001f * elapsedTime;
	currFrame++;

	if (currFrame > 0 && currFrame % 100 == 0) 
	{
		float fps = 1000.f*currFrame / (currTime - startTime);
		char title[256];
		sprintf_s(title, 256, "Test Fluid Solver %3.1f FPS (%.4fs)", fps, dt);  
		glutSetWindowTitle(title);
		startTime = glutGet(GLUT_ELAPSED_TIME);
		currFrame = 0;
	}

	Interact();
	Update(dt);
	Display();
}

void TestScene::Update(float time)
{
	mouseState.dx = mouseState.dy = 0;
	
	fluidTextR += 0.05*time; if (fluidTextR > 2) fluidTextR -= 2;
	fluidTextG += 0.05*time; if (fluidTextG > 2) fluidTextG -= 2;
	fluidTextB += 0.05*time; if (fluidTextB > 2) fluidTextB -= 2;
}

void TestScene::HandleMouseMove(int x, int y)
{
	mouseState.dx = x - mouseState.x;
	mouseState.dy = y - mouseState.y;
	mouseState.x = x;
	mouseState.y = y;
}
void TestScene::HandleMouseButton(int button, int state, int x, int y)
{
	MouseButton mb = 
		button == 0 ? MB_LEFT : 
		button == 1 ? MB_RIGHT : 
		button == 2 ? MB_MIDDLE :
		MB_NONE;

	if (state == GLUT_DOWN && mb != MB_NONE) {
		mouseState.buttons |= mb;
		mouseState.x = x;
		mouseState.y = y;
	} else if (state == GLUT_UP && mb != MB_NONE) {
		mouseState.buttons &= ~mb;
	}

	mouseState.modifiers = glutGetModifiers();
}
void TestScene::HandleKeyboard(unsigned char key, bool down)
{
	if (!down) return;
	switch(key) {
		case 'c': //circular vortex
			fluid->GenerateCircularVortex();
			break;

		case 'g': //checkered (grid) data
			fluid->InjectCheckeredData();
			break;

		case 'q': //quit
			exit(0);
			break;
		
		case '[': //decrease resolution
			options.SolverResolution /= 2;
			cout << "Solver Resolution now: " << options.SolverResolution << endl;
			InitFluid();
			break;

		case ']': //increase resolution
			options.SolverResolution *= 2;
			cout << "Solver Resolution now: " << options.SolverResolution << endl;
			InitFluid();
			break;

		case '=':
		case '+':
			options.RenderResolution /= 2;
			fluid->Init(options);
			break;

		case '-':
			options.RenderResolution *= 2;
			fluid->Init(options);
			break;

		default:
			//empty
			break;
	}
}
void TestScene::HandleKeyboardSpecial(int key)
{
	switch (key) 
	{
		case GLUT_KEY_F1: //help
			cout << endl << endl;
			cout << "=============================================" << endl;
			cout << "Mouse Commands: " << endl;
			cout << "      Click: Ink (red|green|blue)" << endl;
			cout << " Ctrl+Click: Perturb (quick|-|slow)" << endl;
			cout << "Shift+Click: Walls (small|medium|large)" << endl;
			cout << endl;
			cout << "Key Commands: " << endl;
			cout << "[: Decrease Solver Resolution" << endl;
			cout << "]: Increase Solver Resolution" << endl;
			cout << "q: quit" << endl;
			cout << endl;
			cout << "c: Circular Vortex" << endl;
			cout << "g: Inject gridded data" << endl;

			break;
	}
}

void TestScene::InitFluid() 
{
	fluid->Init(options);
	
	startTime = glutGet(GLUT_ELAPSED_TIME);
	currFrame = 0;
}

void TestScene::Resize(int w, int h)
{
	options.RenderResolution.x = w;
	options.RenderResolution.y = h;

	glViewport(0, 0, w, h);

	InitFluid();
}