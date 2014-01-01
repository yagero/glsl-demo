#include "SplineGL.h"
#include "Interpol3D.h"
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>


SplineGL::SplineGL() {
	m_pSpline = new Interpol3D();
	m_t = 0.0f;
	m_fSpeed = 0.01f;
	m_bFinished = false;
}

bool SplineGL::isValid() const
{
	return m_pSpline->isValid();
}

void SplineGL::BuildSplines(bool closed, float speed)
{
	if(closed)
		m_pSpline->Close();
	m_pSpline->BuildSplines();
	m_t = 0.0f;
	m_bFinished = false;
	m_fSpeed = speed;
}


vec3 SplineGL::getPoint()
{
	return m_pSpline->getPoint(m_t);
}

const std::vector<vec3>&	SplineGL::getControlPoints()	const
{
	return m_pSpline->getControlPoints();
}

void SplineGL::Destroy()
{
	if(m_pSpline)
		delete m_pSpline;
	m_pSpline = NULL;
}

bool SplineGL::AddPoint(const vec3& pt)
{
	return m_pSpline->AddPoint(pt);
}

bool SplineGL::DeleteLastPoint()
{
	return m_pSpline->DeleteLastPoint();
}

bool SplineGL::Clear()
{
	return m_pSpline->Clear();
}


void SplineGL::Idle(float fElapsedTime)
{
	assert(m_pSpline);

	m_bFinished = false;
	m_t += m_fSpeed*fElapsedTime;

	if(m_t>1.0f) {
		m_t = 0.0f;
		m_bFinished = true;
	}
}

void SplineGL::DrawGL()
{
	assert(m_pSpline);
	float step = 0.01f;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	if(m_pSpline->isValid())
	{
		glBegin(GL_LINE_STRIP);
		for(float t=0.0f; t<1.0f; t+=step) {
			glVertex3fv( &(m_pSpline->getPoint(t).x) );
		}
		glEnd();

		const std::vector<vec3>& tPoints = m_pSpline->getControlPoints();
		for(unsigned int i=0; i<tPoints.size(); i++) {
			glPushMatrix();
			glTranslatef(tPoints[i].x, tPoints[i].y, tPoints[i].z);
			glutSolidSphere(0.2, 8, 8);
			glPopMatrix();
		}
	}

	glPopAttrib();
}


