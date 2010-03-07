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
#include "TestScene2D.h"

#include <GL/glut.h>
using namespace Fluidic;
using namespace TestFluidic;

TestScene2D::TestScene2D()
{
	fluid = new Fluid2D(CG_PROGRAM_DIR);
	options = Fluid2D::DefaultOptions();
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