#include "SceneBase.h"
#include <assert.h>
#include "../Spline3D/SplineGL.h"
#include "../Camera.h"
#include "../VarManager.h"
#include "../ResourceManager.h"
#include <fstream>
#include <sstream>

SceneBase::SceneBase() {
	m_pCamEyeTraj = new SplineGL();
	m_pCamLookAtTraj = new SplineGL();
}

void SceneBase::Reset()
{
	m_pCamEyeTraj->setTime(0.0f);
	m_pCamLookAtTraj->setTime(0.0f);
	VarManager::getInstance().set("enable_underwater", false);
}

void SceneBase::DrawAxes()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	glPopAttrib();
}

void SceneBase::DrawTraj()
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	glColor3f(1.0f, 0.0f, 0.0f);
	m_pCamEyeTraj->DrawGL();

	glColor3f(0.0f, 1.0f, 0.0f);
	m_pCamLookAtTraj->DrawGL();
}


void SceneBase::InterpolCameraTraj(float fElapsedTime)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	if(m_pCamEyeTraj->isValid() && m_pCamLookAtTraj->isValid()) {
		Camera& cam = Camera::getInstance();
		m_pCamEyeTraj->Idle(fElapsedTime);
		m_pCamLookAtTraj->Idle(fElapsedTime);
		cam.setEye(m_pCamEyeTraj->getPoint());
		cam.setCenter(m_pCamLookAtTraj->getPoint());
	}
}


void SceneBase::DrawString(vec2 pos, void* font, const std::string& str)
{
	assert(font);
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
//	glRasterPos2f(pos.x, pos.y);
	glWindowPos2f(pos.x, pos.y);

	for(GLuint i=0; i<str.size(); i++)
		glutBitmapCharacter(font, str[i]);

	glPopAttrib();
}


void SceneBase::Destroy()
{
	if(m_pCamEyeTraj) delete m_pCamEyeTraj;
	m_pCamEyeTraj = NULL;
	if(m_pCamLookAtTraj) delete m_pCamLookAtTraj;
	m_pCamLookAtTraj = NULL;
}


bool SceneBase::LoadCameraTrajFromFile(const std::string& name)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	m_pCamEyeTraj->Clear();
	m_pCamLookAtTraj->Clear();

	std::string filename = "splines/" + name;
	std::ifstream fp(filename.c_str());
	if(!fp) return false;

	float speed = 0.01f;
	std::string str;
	std::string strBuffer;
	std::getline(fp, strBuffer);
	if(strBuffer.size() > 3) {
		std::stringstream strStream(strBuffer);
		float f;
		strStream >> str >> f;
		if(str == "SPEED")
			speed = f;
	}

	while(std::getline(fp, strBuffer)) {
		if(strBuffer.size() > 3) {
			std::stringstream strStream(strBuffer);
			
			vec3 point;
			strStream >> str >> point.x >> point.y >> point.z;

			if(str == "EYE")			m_pCamEyeTraj->AddPoint(point);
			else if(str == "LOOKAT")	m_pCamLookAtTraj->AddPoint(point);
		}
	}

	m_pCamEyeTraj->BuildSplines(false, speed);
	m_pCamLookAtTraj->BuildSplines(false, speed);

	fp.close();

	return true;
}


bool SceneBase::SaveCameraTrajInFile(const std::string& name)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	std::string filename = "splines/" + name;
	std::ofstream fp(filename.c_str());
	if(!fp) return false;

	fp << "SPEED " << m_pCamEyeTraj->getSpeed() << std::endl;

	for(int i=0; i<(int)m_pCamEyeTraj->getControlPoints().size(); i++) {
		fp << "EYE"	<< " " << m_pCamEyeTraj->getControlPoints()[i].x
					<< " " << m_pCamEyeTraj->getControlPoints()[i].y
					<< " " << m_pCamEyeTraj->getControlPoints()[i].z << std::endl;

		if(i<(int)m_pCamLookAtTraj->getControlPoints().size()) {
			fp << "LOOKAT"	<< " " << m_pCamLookAtTraj->getControlPoints()[i].x
							<< " " << m_pCamLookAtTraj->getControlPoints()[i].y
							<< " " << m_pCamLookAtTraj->getControlPoints()[i].z << std::endl;
		}

		fp << std::endl;
	}

	fp.close();

	return true;
}


void SceneBase::Keyboard(bool special, unsigned char key)
{
	
	if(special)
	{
		if(key == GLUT_KEY_F2)
		{
			ResourceManager::getInstance().Reload();
			Reload();
		}
	}
	/*
	if(!special) {
		switch(key) {
		case 'e':
			m_pCamEyeTraj->AddPoint( Camera::getInstance().getEye() );
			m_pCamEyeTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case 'E':
			m_pCamEyeTraj->DeleteLastPoint();
			m_pCamEyeTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case 'l':
			m_pCamLookAtTraj->AddPoint( Camera::getInstance().getEye() );
			m_pCamLookAtTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case 'L':
			m_pCamLookAtTraj->DeleteLastPoint();
			m_pCamLookAtTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		}
	}*/
}



