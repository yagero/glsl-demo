

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include "Camera.h"
#include "Frustum.h"
#include <assert.h>

void Camera::SaveCamera()
{
	tSaveVectors[0] = vEye;
	tSaveVectors[1] = vCenter;
	tSaveVectors[2] = vViewDir;
	tSaveVectors[3] = vLeftDir;
	tSaveVectors[4] = vUp;

	tSaveFloats[0] = fAngleX;
	tSaveFloats[1] = fAngleY;

	bSaved = true;
}

void Camera::RestoreCamera()
{
	if(bSaved) {
		vEye = tSaveVectors[0];
		vCenter = tSaveVectors[1];
		vViewDir = tSaveVectors[2];
		vLeftDir = tSaveVectors[3];
		vUp = tSaveVectors[4];

		fAngleX = tSaveFloats[0];
		fAngleY = tSaveFloats[1];
	}

	bSaved = false;
}


Camera::Camera() {
	// Paramètres de la caméra par défault :
	fAngleX	=	3.0f;
	fAngleY	=	M_PI/2;

	vUp			= vec3(0.0f, 1.0f, 0.0f);
	vEye		= vec3(0.0f, 0.0f, 0.0f);

	eType = FREE;
	bSaved = false;

	Refresh();
}


void Camera::Refresh()
{
	switch(eType) {
	case FREE:
		vViewDir.x = cosf(fAngleX) * sinf(fAngleY);
		vViewDir.y = cosf(fAngleY);
		vViewDir.z = sinf(fAngleX) * sinf(fAngleY);
		vCenter = vEye + vViewDir;
		vLeftDir.cross(vUp, vViewDir);
		vLeftDir.normalize();
		break;

	case DRIVEN:
		vViewDir = vCenter - vEye;
		vViewDir.normalize();
		vLeftDir.cross(vUp, vViewDir);
		vLeftDir.normalize();
		break;
	}

//	vLeftDir.cross(vUp, vViewDir);
}



void Camera::RenderLookAt(bool inverty, float planey) {

	if(inverty)
		gluLookAt(	vEye.x,		2.0f*planey-vEye.y,		vEye.z,
					vCenter.x,	2.0f*planey-vCenter.y,	vCenter.z,
					-vUp.x,		-vUp.y,					-vUp.z	);
	else
		gluLookAt(	vEye.x,		vEye.y,		vEye.z,
					vCenter.x,	vCenter.y,	vCenter.z,
					vUp.x,		vUp.y,		vUp.z	);
	
	Frustum::getInstance().Extract(vEye);
}




// Autres Fonctions :
void Camera::PlayerMoveForward(float factor)	{	// Avancer/Reculer
	vec3 vPrevEye = vEye;	// on sauvegarde la position précédente

	// on fait varier la position de la caméra :
	vEye += vViewDir * factor;

	Refresh();
}

void Camera::TranslateForward(float factor)	{	// Avancer/Reculer
	// vec3 vPrevEye = vEye;	// on sauvegarde la position précédente

	// on fait varier la position de la caméra :
	vEye += vViewDir * factor;

	Refresh();
}

void Camera::PlayerMoveStrafe(float factor)	{	// Strafe Gauche/Droit
	// vec3 vPrevEye = vEye;	// on sauvegarde la position précédente

	// on fait varier la position de la caméra :
	vEye += vLeftDir * factor;

	Refresh();
}

void Camera::TranslateStrafe(float factor)	{	// Strafe Gauche/Droit
	// vec3 vPrevEye = vEye;	// on sauvegarde la position précédente

	// on fait varier la position de la caméra :
	vEye += vLeftDir * factor;

	Refresh();
}


void Camera::MoveAnaglyph(float factor)
{
	vEye += vLeftDir * factor;
	vCenter += vLeftDir * factor;
}

void Camera::RenderLookAtToCubeMap(const vec3& eye, unsigned int nFace)
{
	assert(nFace < 6);

	// Tableau de vecteurs "Center" pour la caméra :
	vec3 TabCenter[6] = {	vec3(eye.x+1.0f,	eye.y,		eye.z),
							vec3(eye.x-1.0f,	eye.y,		eye.z),

							vec3(eye.x,			eye.y+1.0f,	eye.z),
							vec3(eye.x,			eye.y-1.0f,	eye.z),

							vec3(eye.x,			eye.y,		eye.z+1.0f),
							vec3(eye.x,			eye.y,		eye.z-1.0f) };


	// Tableau de vecteurs "Up" pour la caméra :
	static vec3 TabUp[6] = {	vec3(0.0f,	-1.0f,	0.0f),
								vec3(0.0f,	-1.0f,	0.0f),

								vec3(0.0f,	0.0f,	1.0f),
								vec3(0.0f,	0.0f,	-1.0f),

								vec3(0.0f,	-1.0f,	0.0f),
								vec3(0.0f,	-1.0f,	0.0f) };

	setEye( eye );

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt(	eye.x,						eye.y,						eye.z,
				TabCenter[nFace].x,	TabCenter[nFace].y,	TabCenter[nFace].z,
				TabUp[nFace].x,				TabUp[nFace].y,				TabUp[nFace].z		);

	Frustum::getInstance().Extract(eye);
}



