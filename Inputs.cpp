#include "Inputs.h"
#include <GL/glut.h>

Inputs::Inputs()
{
	memset(tMouse, 0, 3*sizeof(GLboolean));
	memset(tBoard, 0, 256*sizeof(GLboolean));
	memset(tSpecial, 0, 256*sizeof(GLboolean));
}


void Inputs::MouseFunc(int button, int state, int x, int y) {
	vMouse.x = x; vMouse.y = y;
	tMouse[button] = !state;
}


void Inputs::MotionFunc(int x, int y) {
	vMouse.x = x; vMouse.y = y;
}

void Inputs::PassiveMotionFunc(int x, int y) {
	vMouse.x = x; vMouse.y = y;
}


void Inputs::KeyboardFunc(unsigned char key)	{	tBoard[key]   = GL_TRUE;	}
void Inputs::KeyboardUpFunc(unsigned char key)	{	tBoard[key]   = GL_FALSE;	}
void Inputs::SpecialFunc(int key)				{	tSpecial[key] = GL_TRUE;	}
void Inputs::SpecialUpFunc(int key)				{	tSpecial[key] = GL_FALSE;	}


