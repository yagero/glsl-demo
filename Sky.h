#ifndef SKY_H
#define SKY_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glut.h>

#include "Singleton.h"
#include "Mathlib.h"
#include <iostream>

class Shader;

SINGLETON_BEGIN( Sky )
public:
	void Init();
	void DrawSky(const vec3& vEyePos, const GLuint& cubemap, bool invert=false) const;
	void DrawSun(const vec3& vEyePos, const vec3& vSunVect) const;

	void DrawSkyAndSun(const vec3& vEyePos, const vec3& vSunVect, const GLuint& cubemap, bool invert) const;

private:
	Shader*	m_pShd;
SINGLETON_END()

#endif


