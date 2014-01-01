#ifndef CAMERA_H
#define CAMERA_H


#include "Mathlib.h"
#include "Singleton.h"

// -------------------------------
// Gestion de la vue
// -------------------------------

SINGLETON_BEGIN( Camera )
public:	enum TYPE {FREE, DRIVEN};
private:
	vec3	vEye;		// Position de la caméra
	vec3	vCenter;	// Position du centre de visée
	vec3	vViewDir;	// Vecteur Devant
	vec3	vLeftDir;	// Vecteur Gauche
	vec3	vUp;		// Vecteur Up

	float	fAngleX;			// Angle de vue Teta
	float	fAngleY;			// Angle de vue Phi

	TYPE	eType;				// type de la camera


	vec3	tSaveVectors[5];
	float	tSaveFloats[2];
	bool	bSaved;

public:
	inline TYPE  getType()				const	{return eType;}
	inline const vec3& getEye()			const	{return vEye;}
	inline const vec3& getCenter()		const	{return vCenter;}
	inline const vec3& getViewDir()		const	{return vViewDir;}
	inline float getAngleX()			const	{return fAngleX;}
	inline float getAngleY()			const	{return fAngleY;}


protected:
	Camera();

public:
	void SaveCamera();
	void RestoreCamera();

	void Refresh();

	// Fixer un paramètre :
	void setType(TYPE t)				{ eType = t;		Refresh(); }
	void setEye(vec3 vPos)				{ vEye = vPos;		Refresh(); }
	void setCenter(vec3 vPos)			{ vCenter = vPos;	Refresh(); }
	void setAngleX(float Angle)			{ fAngleX = Angle;	Refresh(); }
	void setAngleY(float Angle)			{ fAngleY = Angle;	Refresh(); }
	void setAngle(float AngleX, float AngleY)	{
		fAngleX = AngleX;
		fAngleY = AngleY;
		Refresh();
	}

	// Modifier un paramètre :
	void Translate(vec3 vector)	{ vEye += vector;	Refresh(); }
	void RotateX(float AngleX)		{
		fAngleX += AngleX;
				if(fAngleX<0.0f)	fAngleX += M_PI*2;
		else	if(fAngleX>M_PI*2)	fAngleX -= M_PI*2;
		Refresh();
	}
	void RotateY(float AngleY)		{
		fAngleY += AngleY;
		if(fAngleY<0 || fAngleY>M_PI)	fAngleY -= AngleY;
		Refresh();
	}
	void Rotate(float AngleX, float AngleY)		{
		fAngleX += AngleX;
		fAngleY += AngleY;
				if(fAngleX<0.0f)		fAngleX += M_PI*2;
		else	if(fAngleX>M_PI*2)		fAngleX -= M_PI*2;
		if(fAngleY<0 || fAngleY>M_PI)	fAngleY -= AngleY;
		Refresh();
	}

	// Autres Fonctions :
	void PlayerMoveForward(float factor);	// Avancer/Reculer
	void PlayerMoveStrafe(float factor);	// Strafe Gauche/Droit
	void TranslateForward(float factor);	// Avancer/Reculer
	void TranslateStrafe(float factor);		// Strafe Gauche/Droit
	void MoveAnaglyph(float factor);

	void RenderLookAt(bool inverty=false, float planey=0.0f);

	void RenderLookAtToCubeMap(const vec3& eye, unsigned int nFace);
SINGLETON_END()

#endif