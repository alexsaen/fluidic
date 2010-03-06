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