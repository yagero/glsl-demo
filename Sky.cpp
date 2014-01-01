
#include "Sky.h"
#include "TextureCubemap.h"
#include "ResourceManager.h"
#include "Camera.h"

void Sky::Init()
{
	ResourceManager& res = ResourceManager::getInstance();

	m_pShd = (Shader*)res.LoadResource(ResourceManager::SHADER, "sky");
}

void Sky::DrawSkyAndSun(const vec3& vEyePos, const vec3& vSunVect, const GLuint& cubemap, bool invert) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);


	glPushAttrib(GL_ENABLE_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	m_pShd->Activate();
	{
		m_pShd->UniformTexture("texSky", 0);

		m_pShd->Uniform("enable_sun", true);
		m_pShd->Uniform("sun_vector", vSunVect);
//		m_pShd->Uniform("view_vector", Camera::getInstance().getViewDir());

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glutSolidSphere (1.0, 4, 4);
	}
	m_pShd->Deactivate();

	glPopAttrib();

	glPopMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);
}

void Sky::DrawSky(const vec3& vEyePos, const GLuint& cubemap, bool invert) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);


	glPushAttrib(GL_ENABLE_BIT);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	m_pShd->Activate();
	{
		m_pShd->UniformTexture("texSky", 0);

		m_pShd->Uniform("enable_sun", false);

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glutSolidSphere (1.0, 4, 4);
	}
	m_pShd->Deactivate();
	glDisable(GL_TEXTURE_CUBE_MAP);

	glPopAttrib();

	glPopMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);

/*
	static GLfloat xPlane[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	static GLfloat yPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	static GLfloat zPlane[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT);
	glPushAttrib(GL_POLYGON_BIT);

	glEnable (GL_TEXTURE_GEN_S);
	glEnable (GL_TEXTURE_GEN_T);
	glEnable (GL_TEXTURE_GEN_R);

	glEnable (GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni (GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexGenfv (GL_S, GL_OBJECT_PLANE, xPlane);
	glTexGenfv (GL_T, GL_OBJECT_PLANE, yPlane);
	glTexGenfv (GL_R, GL_OBJECT_PLANE, zPlane);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere (1.0, 4, 4);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDisable (GL_TEXTURE_CUBE_MAP);

	glDisable (GL_TEXTURE_GEN_S);
	glDisable (GL_TEXTURE_GEN_T);
	glDisable (GL_TEXTURE_GEN_R);

	glPopAttrib();
	glPopAttrib();

	glClear(GL_DEPTH_BUFFER_BIT);
	glPopMatrix();*/
}

void Sky::DrawSun(const vec3& vEyePos, const vec3& vSunVect) const
{
	vec4 color;
	glGetLightfv(GL_LIGHT0, GL_DIFFUSE, color);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	glTranslatef(-vSunVect.x, -vSunVect.y, -vSunVect.z);

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(color.r, color.g, color.b);
	glutSolidSphere(0.1, 16, 16);

	glPopAttrib();

	glClear(GL_DEPTH_BUFFER_BIT);
	glPopMatrix();
}


