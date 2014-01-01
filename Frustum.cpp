#ifdef WIN32
#include <windows.h>
#endif

#include "Frustum.h"

#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <math.h>
using namespace std;


/*
vec3 Frustum::Intersect2D(int id1, int id2) const {
	vec3 p;
//	p.y = (-frustum[id2][3]/frustum[id2][0] + frustum[id1][3]/frustum[id1][0]) / (-frustum[id2][2]/frustum[id2][0] + frustum[id1][2]/frustum[id1][0]);

//	p.x = (-frustum[id1][2] * p.y - frustum[id1][3])/frustum[id1][0];
	return p;
}
*/
bool Frustum::ContainsPoint(const vec3& point) const
{
   for(int p = 0; p < 6; p++)	// pour tous les plans
	   // si le point est en dehors d un des plans
      if(	m_tFrustumPlanes[p][0] * point.x +
			m_tFrustumPlanes[p][1] * point.y +
			m_tFrustumPlanes[p][2] * point.z +
			m_tFrustumPlanes[p][3] <= 0)
         return false;

   return true;
}

int Frustum::ContainsSphere(const vec3& center, float radius) const
{
	// pour tous les plans
	for(int p = 0; p < 6; p++)
	{
		//distance entre le centre du cercle et le plan
		float t =	m_tFrustumPlanes[p][0] * center.x +
					m_tFrustumPlanes[p][1] * center.y +
					m_tFrustumPlanes[p][2] * center.z +
					m_tFrustumPlanes[p][3];

		if( t < -radius)
			return FRUSTUM_OUT;

		if((float)fabs(t) < radius)
			return FRUSTUM_INTERSECT;
	}
	return FRUSTUM_IN;
}


int Frustum::ContainsBoundingBox(const BoundingBox& bbox) const
{
	vec3 tCorners[8] = {	vec3(bbox.min.x, bbox.min.y, bbox.min.z),
							vec3(bbox.max.x, bbox.min.y, bbox.min.z),
							vec3(bbox.min.x, bbox.max.y, bbox.min.z),
							vec3(bbox.min.x, bbox.min.y, bbox.max.z),
							vec3(bbox.max.x, bbox.max.y, bbox.min.z),
							vec3(bbox.min.x, bbox.max.y, bbox.max.z),
							vec3(bbox.max.x, bbox.min.y, bbox.max.z),
							vec3(bbox.max.x, bbox.max.y, bbox.max.z)	};
	int iTotalIn = 0;

	for(int p=0; p<6; p++)
	{
		int iInCount = 8;
		int iPtIn = 1;

		for(int c=0; c<8; c++)
		{
			// teste tout les points
			float side =	m_tFrustumPlanes[p][0] * tCorners[c].x +
							m_tFrustumPlanes[p][1] * tCorners[c].y +
							m_tFrustumPlanes[p][2] * tCorners[c].z +
							m_tFrustumPlanes[p][3];
			if(side < 0) {
				iPtIn = 0;
				iInCount--;
			}
		}

		// Tous les points en dehors :
		if(iInCount == 0)
			return FRUSTUM_OUT;

		iTotalIn += iPtIn;
	}

	// Si tous les points sont dedans :
	if(iTotalIn == 6)
		return FRUSTUM_IN;

	return FRUSTUM_INTERSECT;

}



void Frustum::Extract(const vec3& eye)
{
	m_EyePos = eye;


	glGetFloatv( GL_MODELVIEW_MATRIX, m_mtxMV.mat );	// Récupere la matrice de modelisation
	m_mtxMV.inverse(m_mtxMVinv);						// Inverse de la modelview
	glGetFloatv( GL_PROJECTION_MATRIX, m_mtxProj.mat );	// Récupere la matrice de projection


	float t;

	// On combine les 2, en multipliant la matrice de projection par celle de modelisation
	m_mtxMVProj = m_mtxProj * m_mtxMV;


	/* Extrait le plan de DROITE */
	m_tFrustumPlanes[0][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 0];
	m_tFrustumPlanes[0][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 4];
	m_tFrustumPlanes[0][2] = m_mtxMVProj[11] - m_mtxMVProj[ 8];
	m_tFrustumPlanes[0][3] = m_mtxMVProj[15] - m_mtxMVProj[12];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[0][0] * m_tFrustumPlanes[0][0] + m_tFrustumPlanes[0][1] * m_tFrustumPlanes[0][1] + m_tFrustumPlanes[0][2] * m_tFrustumPlanes[0][2] );
	m_tFrustumPlanes[0][0] /= t;
	m_tFrustumPlanes[0][1] /= t;
	m_tFrustumPlanes[0][2] /= t;
	m_tFrustumPlanes[0][3] /= t;

	/* Extrait le plan de GAUCHE */
	m_tFrustumPlanes[1][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 0];
	m_tFrustumPlanes[1][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 4];
	m_tFrustumPlanes[1][2] = m_mtxMVProj[11] + m_mtxMVProj[ 8];
	m_tFrustumPlanes[1][3] = m_mtxMVProj[15] + m_mtxMVProj[12];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[1][0] * m_tFrustumPlanes[1][0] + m_tFrustumPlanes[1][1] * m_tFrustumPlanes[1][1] + m_tFrustumPlanes[1][2] * m_tFrustumPlanes[1][2] );
	m_tFrustumPlanes[1][0] /= t;
	m_tFrustumPlanes[1][1] /= t;
	m_tFrustumPlanes[1][2] /= t;
	m_tFrustumPlanes[1][3] /= t;

	/* Extrait le plan du BAS */
	m_tFrustumPlanes[2][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 1];
	m_tFrustumPlanes[2][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 5];
	m_tFrustumPlanes[2][2] = m_mtxMVProj[11] + m_mtxMVProj[ 9];
	m_tFrustumPlanes[2][3] = m_mtxMVProj[15] + m_mtxMVProj[13];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[2][0] * m_tFrustumPlanes[2][0] + m_tFrustumPlanes[2][1] * m_tFrustumPlanes[2][1] + m_tFrustumPlanes[2][2] * m_tFrustumPlanes[2][2] );
	m_tFrustumPlanes[2][0] /= t;
	m_tFrustumPlanes[2][1] /= t;
	m_tFrustumPlanes[2][2] /= t;
	m_tFrustumPlanes[2][3] /= t;

	/* Extrait le plan du HAUT */
	m_tFrustumPlanes[3][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 1];
	m_tFrustumPlanes[3][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 5];
	m_tFrustumPlanes[3][2] = m_mtxMVProj[11] - m_mtxMVProj[ 9];
	m_tFrustumPlanes[3][3] = m_mtxMVProj[15] - m_mtxMVProj[13];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[3][0] * m_tFrustumPlanes[3][0] + m_tFrustumPlanes[3][1] * m_tFrustumPlanes[3][1] + m_tFrustumPlanes[3][2] * m_tFrustumPlanes[3][2] );
	m_tFrustumPlanes[3][0] /= t;
	m_tFrustumPlanes[3][1] /= t;
	m_tFrustumPlanes[3][2] /= t;
	m_tFrustumPlanes[3][3] /= t;

	/* Extrait le plan ELOIGNE */
	m_tFrustumPlanes[4][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 2];
	m_tFrustumPlanes[4][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 6];
	m_tFrustumPlanes[4][2] = m_mtxMVProj[11] - m_mtxMVProj[10];
	m_tFrustumPlanes[4][3] = m_mtxMVProj[15] - m_mtxMVProj[14];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[4][0] * m_tFrustumPlanes[4][0] + m_tFrustumPlanes[4][1] * m_tFrustumPlanes[4][1] + m_tFrustumPlanes[4][2] * m_tFrustumPlanes[4][2] );
	m_tFrustumPlanes[4][0] /= t;
	m_tFrustumPlanes[4][1] /= t;
	m_tFrustumPlanes[4][2] /= t;
	m_tFrustumPlanes[4][3] /= t;

	/* Extrait le plan PROCHE */
	m_tFrustumPlanes[5][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 2];
	m_tFrustumPlanes[5][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 6];
	m_tFrustumPlanes[5][2] = m_mtxMVProj[11] + m_mtxMVProj[10];
	m_tFrustumPlanes[5][3] = m_mtxMVProj[15] + m_mtxMVProj[14];

	/* Calcul des normales */
	t = sqrt( m_tFrustumPlanes[5][0] * m_tFrustumPlanes[5][0] + m_tFrustumPlanes[5][1] * m_tFrustumPlanes[5][1] + m_tFrustumPlanes[5][2] * m_tFrustumPlanes[5][2] );
	m_tFrustumPlanes[5][0] /= t;
	m_tFrustumPlanes[5][1] /= t;
	m_tFrustumPlanes[5][2] /= t;
	m_tFrustumPlanes[5][3] /= t;
	
}


