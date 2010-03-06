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
#include "TestScene2D.h"
#include "TestScene3D.h"

#include <GL/glew.h>
#include <GL/glut.h>

int width = 400, height = 400;

using namespace std;
using namespace Fluidic;
using namespace TestFluidic;

int main(int argc, char **argv);
void Display(void);
void Reshape(int, int);
void Timer(int);
void Idle();

void Mouse(int, int, int, int);
void Motion(int, int);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void Special(int key, int x, int y);

inline int TimeSinceLastReset();
inline void ResetTime();

void InitGLUT(int argc, char **argv);
void InitGLEW(void);

TestScene *scene;

int main(int argc, char **argv)
{
	//perform initialisation
	InitGLUT(argc, argv);
	InitGLEW();

	scene = new TestScene2D();

	glutMainLoop();
}

void InitGLUT(int argc, char **argv)
{
	glutInit( &argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(200,200);

	glutCreateWindow("Test Fluid Solver");

	glClearColor(0.0,0.0,0.0,0.0);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(Special);

}

void InitGLEW()
{
	int err = glewInit();
	if (GLEW_OK != err)
	{
		exit(err);
	}
}

void Reshape(GLsizei w, GLsizei h)
{
	scene->Resize(width=w, height=h);
}

void Idle()
{
	glutPostRedisplay();
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->Tick();
	glutSwapBuffers();
}

void Mouse(int button,int state,int x,int y) 
{
	scene->HandleMouseButton(button, state, x, y);
}


void Motion (int x, int y) 
{
	scene->HandleMouseMove(x, y);
}

void Keyboard(unsigned char key, int x, int y) 
{
	x=y;  //shush compiler warnings

	if (key == '2') 
	{
		delete scene;
		scene = new TestScene2D();
		scene->Resize(width, height);
	}
	else if (key == '3') 
	{
		delete scene;
		scene = new TestScene3D();
		scene->Resize(width, height);
	}
	else
	{
		scene->HandleKeyboard(key, true);
	}
}
void KeyboardUp(unsigned char key, int x, int y) 
{
	x=y;  //shush compiler warnings
	scene->HandleKeyboard(key, false);
}

void Special(int key, int x, int y) {
	x=y; //shush the compiler warnings
	scene->HandleKeyboardSpecial(key);
}