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
#include "TestScene3D.h"

#include <iostream>
#include <GL/glut.h>

using namespace std;
using namespace Fluidic;
using namespace TestFluidic;

TestScene3D::TestScene3D()
: moving(false), injectZPos(0.5f)
{
	fluid = new Fluid3D(CG_PROGRAM_DIR);
	options.RenderResolution = Vector(400, 400);
	options.SolverResolution = Vector(64, 64, 64);
	options.Size = Vector(1, 1, 1);
	options.SolverOptions = RS_NICE | RS_DOUBLE_PRECISION;
	options.Viscosity = 0;//ViscosityAir;
	fluid->Init(options);
	fluid->SetColorDensities(1, 0, -1);
	
	obj = new TestObject(options.Size/2, Vector::Zero(), 1);
	fluid->AttachPoller(obj);

	//initialise the camera
	camera.x = 0;
	camera.y = 0.865;
	camera.z = 2;
	camera.yaw = 180;
	camera.pitch = -30;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, options.RenderResolution.x/options.RenderResolution.y, 0.01, 400.0);
	glMatrixMode (GL_MODELVIEW);
	

	//set up all the depth test and etc.
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	//light information
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glDepthMask(GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 0.0) ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
}

TestScene3D::~TestScene3D()
{
	cgDestroyContext(cgContext);
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
}

void TestScene3D::Interact()
{
	TestScene::Interact();

	if (moving)
	{
		//update yaw depending on how much the mouse has moved
		//in the 'x' direction.  keep it between 0 and 360 degrees.
		camera.yaw -= mouseState.dx/10;
		if (camera.yaw >= 360) camera.yaw -= 360;
		if (camera.yaw < 0)camera.yaw += 360;
		mouseState.dx=0;

		//update pitch depending on how much the mouse has moved
		//in the 'x' direction.  keep it between 0 and 360 degrees.
		camera.pitch -= mouseState.dy/10;
		if (camera.pitch > 60) camera.pitch=60;
		if (camera.pitch < -60) camera.pitch = -60;
		mouseState.dy=0;
		
		//move mouse back to center of screen, and reset theMouse variable to suit
		mouseState.x = options.RenderResolution.xi()/2;
		mouseState.y = options.RenderResolution.yi()/2;
		mouseState.dx = 0;
		mouseState.dy = 0;

		glutWarpPointer(mouseState.x,mouseState.y);
	}
	else
	{
		float xPos = (float)mouseState.x / options.RenderResolution.x;
		float yPos = (float)mouseState.y / options.RenderResolution.y;
		Vector position(xPos, yPos, injectZPos);
		position = position * options.Size;

		float dx = options.Size.x * (float)mouseState.dx / options.RenderResolution.x;
		float dy = -options.Size.y * (float)mouseState.dy / options.RenderResolution.y;
		Vector d(dx, dy);

		bool lmb = ((mouseState.buttons & MB_LEFT) == MB_LEFT);
		bool mmb = ((mouseState.buttons & MB_MIDDLE) == MB_MIDDLE);
		bool rmb = ((mouseState.buttons & MB_RIGHT) == MB_RIGHT);

		bool ctrl = (mouseState.modifiers & GLUT_ACTIVE_CTRL) == GLUT_ACTIVE_CTRL;
		bool shift = (mouseState.modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT;
		bool noMods = !ctrl && !shift;

		float size = options.Size.Length();

		if (lmb || rmb || mmb)
		{
			//dye injection
			if (lmb && noMods) fluid->Inject(position, 1, 0, 0, 0.05f * size, false); //red
			if (rmb && noMods) fluid->Inject(position, 0, 1, 0, 0.05f * size, false); //green
			if (mmb && noMods) fluid->Inject(position, 0, 0, 1, 0.05f * size, false); //blue

			//fluid perturbation
			if (lmb && ctrl) fluid->Perturb(position, d*5, 0.025f*size); //normal
			if (rmb && ctrl) fluid->Perturb(position, d, 0.025*size); //slow
			if (mmb && ctrl) fluid->Perturb(position, d*20, 0.025*size); //fast

			// arbitrary boundaries
			if (lmb && shift) fluid->AddArbitraryBoundary(position, 0.05*size);
			if (mmb && shift) fluid->AddArbitraryBoundary(position, 0.1*size);
			if (rmb && shift) fluid->AddArbitraryBoundary(position, 0.2*size);
		}
	}


}

void TestScene3D::HandleKeyboard(unsigned char key, bool down)
{
	TestScene::HandleKeyboard(key, down);

	switch(key) {
		case ' ': // toggle mode
			if (down) 
			{
				moving = !moving;
				if (moving) glutSetCursor(GLUT_CURSOR_NONE);
				else glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
			}
			break;
		case 'a': //strafe left
			camera.vx = down ? -3 : 0;
			break;
			
		case 'd': //strafe right
			camera.vx = down ? 3 : 0;
			break;
			
		case 'w': //forward
			camera.vz = down ? 3 : 0;
			break;
			
		case 's': //back
			camera.vz = down ? -3 : 0;
			break;
	}
}
void TestScene3D::HandleKeyboardSpecial(int key)
{
	TestScene::HandleKeyboardSpecial(key);
	if (key == GLUT_KEY_UP)
	{
		injectZPos += 0.1;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		injectZPos -= 0.1;
	} 
	else if (key == GLUT_KEY_F1)
	{
		cout << endl;
		cout << "Space: Toggle moving/injecting" << endl;
		cout << "    w: Move Forward" << endl;
		cout << "    a: Move Back" << endl;
		cout << "    s: Strafe Left" << endl;
		cout << "    d: Strafe Right" << endl;
		cout << endl;
		cout << "   Up: Move injection point up" << endl;
		cout << " Down: Move injection point down" << endl;
	}
}
void TestScene3D::Update(float time)
{
	TestScene::Update(time);
	fluid->Update(time);
	obj->Update(time);

	if (moving) 
	{
		//move the camera forward in the directino we're moving (if we are moving)
		camera.z += camera.vz * cos(degToRad(camera.yaw))*time;
		camera.x += camera.vz * sin(degToRad(camera.yaw))*time;
		camera.y += camera.vz * sin(degToRad(camera.pitch))*time;

		//move the camera left or right (if we're moving)
		camera.z += camera.vx * sin(degToRad(camera.yaw))*time;
		camera.x -= camera.vx * cos(degToRad(camera.yaw))*time;
	}
}

void TestScene3D::Display()
{
	glPushMatrix();
	glTranslatef(options.Size.x*0.1, options.Size.y*0.4, 0);
	glScalef(options.Size.x*0.8, options.Size.y*0.2, 0);
	DrawFluidText();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	//set up the camera
	gluLookAt(camera.x, camera.y, camera.z, //position
		camera.x + sin(degToRad(camera.yaw)), camera.y + sin(degToRad(camera.pitch)),camera.z + cos(degToRad(camera.pitch))*cos(degToRad(camera.yaw)), //direction
		0,cos(degToRad(camera.pitch)),0); //roll

	//draw a sphere
	glColor3f(0.3, 0.0, 0.3);
	glTranslatef(0, 0, -3);
	glutSolidSphere(1, 16, 16);
	glTranslatef(0, 0, 3);


	//draw wire cube around fluid
	glDisable(GL_BLEND);
	glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 0.0);
		glTranslatef(-options.Size.x/2, -options.Size.y/2, -options.Size.z/2);
		glScalef(options.Size.x, options.Size.y, options.Size.z);
		glTranslatef(0.5,0.5,0.5);
		glutWireCube(1);
	glPopMatrix();
	glEnable(GL_BLEND);

		
	// draw the ball
	glColor3f(1, 1, 1);
	glPushMatrix();
		glTranslatef(-0.5, -0.5, -0.5);
		const Vector &objPosition = obj->GetPosition();
		glTranslatef(objPosition.x, objPosition.y, objPosition.z);
		glutSolidSphere(options.Size.Length()/80, 16, 16);
	glPopMatrix();

	
	//draw the fluid, blending it in with current scene
	fluid->Render();
	
	glPopMatrix();
}


void TestScene3D::Resize(int w, int h)
{	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, options.RenderResolution.x/options.RenderResolution.y, 0.01, 400.0);
	glMatrixMode (GL_MODELVIEW);

	TestScene::Resize(w, h);
}