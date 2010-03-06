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
#include "TestScene2D.h"

#include <GL/glut.h>
using namespace Fluidic;
using namespace TestFluidic;

TestScene2D::TestScene2D()
{
	fluid = new Fluid2D(CG_PROGRAM_DIR);

	options.Size = Vector(1, 1);
	options.RenderResolution = Vector(400, 400);
	options.SolverResolution = Vector(200, 200);
	options.SolverOptions = RS_NICE;
	options.Viscosity = ViscosityAir;
	fluid->Init(options);
	fluid->SetColorDensities(1, 0, -1);
	
	obj = new TestObject(options.Size/2, Vector::Zero2D(), 1);
	fluid->AttachPoller(obj);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, options.Size.x, 0, options.Size.y, 0, 4);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

TestScene2D::~TestScene2D()
{
	glDisable(GL_BLEND);
	cgDestroyContext(cgContext);
}

void TestScene2D::Interact()
{
	TestScene::Interact();
}

void TestScene2D::Update(float time)
{

	float xPos = options.Size.x * (float)mouseState.x / options.RenderResolution.x;
	float yPos = options.Size.y * (1 - (float)mouseState.y/options.RenderResolution.y);
	Vector position(xPos, yPos);

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
		if (lmb && noMods) fluid->Inject(position, 40*time, 0, 0, 0.05f * size, false); //red
		if (rmb && noMods) fluid->Inject(position, 0, 40*time, 0, 0.05f * size, false); //green
		if (mmb && noMods) fluid->Inject(position, 0, 0, 40*time, 0.05f * size, false); //blue

		//fluid perturbation
		if (lmb && ctrl) fluid->Perturb(position, d*5, 0.025f*size); //normal
		if (rmb && ctrl) fluid->Perturb(position, d, 0.025*size); //slow
		if (mmb && ctrl) fluid->Perturb(position, d*20, 0.025*size); //fast

		// arbitrary boundaries
		if (lmb && shift) fluid->AddArbitraryBoundary(position, 0.05*size);
		if (mmb && shift) fluid->AddArbitraryBoundary(position, 0.1*size);
		if (rmb && shift) fluid->AddArbitraryBoundary(position, 0.2*size);
	}

	TestScene::Update(time);
	fluid->Update(time);
	obj->Update(time);
}

void TestScene2D::Display()
{
	glPushMatrix();
	glTranslatef(options.Size.x*0.1, options.Size.y*0.4, 0);
	glScalef(options.Size.x*0.8, options.Size.y*0.2, 0);
	DrawFluidText();
	glPopMatrix();

	fluid->Render();
	
	glColor3f(1, 1, 1);
	glPushMatrix();
	const Vector &objPosition = obj->GetPosition();
	glTranslatef(objPosition.x, objPosition.y, 0);
	glutSolidSphere(options.Size.Length()/80, 16, 16);
	glPopMatrix();
}


void TestScene2D::Resize(int w, int h)
{
	options.Size = options.Size * Vector(w, h) / options.RenderResolution;
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, options.Size.x, 0, options.Size.y, 0, 4);
	glMatrixMode (GL_MODELVIEW);

	TestScene::Resize(w, h);
}