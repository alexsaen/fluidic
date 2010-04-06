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

	float timeSinceStart = currTime - startTime;
	if (timeSinceStart > 1000)
	{
		float fps = 1000.f*currFrame / timeSinceStart;
		char title[256];
		sprintf_s(title, 256, "Test Fluid Solver %3.1f FPS @%dsteps", fps, solveCount);  
		glutSetWindowTitle(title);
		startTime = glutGet(GLUT_ELAPSED_TIME);
		currFrame = 0;
		solveCount = 0;
	}

	Interact();
	Update(dt);
	solveCount += fluid->GetSolveCount();
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

		case 'i': // Ink on / off
			options.RenderOptions ^= Fluidic::RR_INK;
			fluid->Init(options, true);
			fluid->SetColorDensities(1, 0, -1);
			break;

		case 'b': // Boundary on / off
			options.RenderOptions ^= Fluidic::RR_BOUNDARIES;
			fluid->Init(options, true);
			fluid->SetColorDensities(1, 0, -1);
			break;

		case 'f': // Flow on / off
			options.RenderOptions ^= Fluidic::RR_FLOW;
			fluid->Init(options, true);
			fluid->SetColorDensities(1, 0, -1);
			break;

		case 'n': // Nullclines on / off
			options.RenderOptions ^= Fluidic::RR_NULLCLINES;
			fluid->Init(options, true);
			fluid->SetColorDensities(1, 0, -1);
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
	previousTime = startTime = glutGet(GLUT_ELAPSED_TIME);
	currFrame = solveCount = 0;
}

void TestScene::Resize(int w, int h)
{
	options.RenderResolution.x = w;
	options.RenderResolution.y = h;

	glViewport(0, 0, w, h);

	InitFluid();
}