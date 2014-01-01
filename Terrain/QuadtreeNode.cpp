#include "QuadtreeNode.h"
#include "../Frustum.h"
#include "../Mesh.h"
#include "TerrainChunk.h"
#include "TerrainObject.h"
#include <math.h>
#include <iostream>
#include <assert.h>

using namespace std;

void QuadtreeNode::Build(unsigned int depth,		// profondeur de la récursion
						 ivec2 pos,					// position du chunk dans la heightmap
						 ivec2 HMsize,				// longueur et largeur de la heightmap
						 unsigned int minHMSize)	// taille minimale des chunks
{
	m_nLOD = 0;

	unsigned int div = (unsigned int)pow(2.0f, (float)depth);
	ivec2 nodesize = HMsize/(div);
	if(nodesize.x%2==0) nodesize.x++;
	if(nodesize.y%2==0) nodesize.y++;
	ivec2 newsize = nodesize/2;


	// Condition d'arrêt
	if((unsigned int)max(newsize.x, newsize.y) < minHMSize)
	{
		// Création d'un nouveau chunk
		m_pTerrainChunk = new TerrainChunk();
		m_pTerrainChunk->Load(depth, pos, HMsize);

		// Pas de fils
		m_pChildren = NULL;
		return;
	}



	//On decoupe => creation des 4 fils
	m_pChildren = new QuadtreeNode[4];

	// Calcul des bounding box des fils
	vec3 center = m_BBox.getCenter();
	m_pChildren[CHILD_NW].setBoundingBox( BoundingBox(m_BBox.min, center) );
	m_pChildren[CHILD_NE].setBoundingBox( BoundingBox(vec3(center.x, 0.0f, m_BBox.min.z), vec3(m_BBox.max.x, 0.0f, center.z)) );
	m_pChildren[CHILD_SW].setBoundingBox( BoundingBox(vec3(m_BBox.min.x, 0.0f, center.z), vec3(center.x, 0.0f, m_BBox.max.z)) );
	m_pChildren[CHILD_SE].setBoundingBox( BoundingBox(center, m_BBox.max) );

	// Calcul des positions des noeuds fils
	ivec2 tNewHMpos[4];
	tNewHMpos[CHILD_NW] = pos + ivec2(0, 0);
	tNewHMpos[CHILD_NE] = pos + ivec2(newsize.x, 0);
	tNewHMpos[CHILD_SW] = pos + ivec2(0, newsize.y);
	tNewHMpos[CHILD_SE] = pos + ivec2(newsize.x, newsize.y);


	// De la même facon, on traite les fils
	for(int i=0; i<4; i++)
		m_pChildren[i].Build(depth+1, tNewHMpos[i], HMsize, minHMSize);
}

void QuadtreeNode::ComputeBoundingBox(const vec3* vertices)
{
	assert(vertices);

	m_BBox.min.y = 100000.0f;
	m_BBox.max.y = -100000.0f;

	if(m_pTerrainChunk) {
		std::vector<GLuint>& tIndices = m_pTerrainChunk->getIndiceArray(0);

		for(GLuint i=0; i<tIndices.size(); i++) {
			vec3 vertex = vertices[ tIndices[i] ];

			if(vertex.y > m_BBox.max.y)	m_BBox.max.y = vertex.y;
			if(vertex.y < m_BBox.min.y)	m_BBox.min.y = vertex.y;
		}

		for(GLuint i=0; i<m_pTerrainChunk->getObjectsArray().size(); i++) {
			TerrainObject* obj = m_pTerrainChunk->getObjectsArray()[ i ];
			Mesh* mesh = obj->getMesh(0);
			BoundingBox bbox = mesh->getBoundingBox();
			bbox.Translate( obj->getPosition() );

			m_BBox.Add( bbox );
		}
	}

	// De la même facon, on traite les fils
	if(m_pChildren) {
		for(int i=0; i<4; i++) {
			m_pChildren[i].ComputeBoundingBox(vertices);

			if(m_BBox.min.y > m_pChildren[i].m_BBox.min.y)	m_BBox.min.y = m_pChildren[i].m_BBox.min.y;
			if(m_BBox.max.y < m_pChildren[i].m_BBox.max.y)	m_BBox.max.y = m_pChildren[i].m_BBox.max.y;
		}
	}
}


void QuadtreeNode::Destroy()
{
	if(m_pChildren) {
		for(int i=0; i<4; i++)
			m_pChildren[i].Destroy();
		delete [] m_pChildren;
		m_pChildren = NULL;
	}
}

int QuadtreeNode::DrawObjects(bool bReflection)
{
	if(!m_pChildren) {
		assert(m_pTerrainChunk);
		if( m_nLOD>=0 )
			return m_pTerrainChunk->DrawObjects( bReflection? TERRAIN_CHUNKS_LOD-1 : (GLuint)m_nLOD );
		else
			return 0;
	}
	else {
		int ret = 0;
		if( m_nLOD>=0 )
			for(int i=0; i<4; i++)
				ret += m_pChildren[i].DrawObjects(bReflection);
		return ret;		
	}
}


int QuadtreeNode::DrawGrass(bool bReflection)
{
	if(!m_pChildren) {
		assert(m_pTerrainChunk);
		if( m_nLOD>=0 )
			return m_pTerrainChunk->DrawGrass( (GLuint)m_nLOD, m_fDistance );
		else
			return 0;
	}
	else {
		int ret = 0;
		if( m_nLOD>=0 )
			for(int i=0; i<4; i++)
				ret += m_pChildren[i].DrawGrass(bReflection);
		return ret;		
	}
}


void QuadtreeNode::DrawBBox(bool bTest)
{
	if(!m_pChildren) {
		assert(m_pTerrainChunk);

		glBegin(GL_LINE_LOOP);
		glVertex3f( m_BBox.min.x, m_BBox.min.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.min.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.min.y, m_BBox.max.z );
		glVertex3f( m_BBox.min.x, m_BBox.min.y, m_BBox.max.z );
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f( m_BBox.min.x, m_BBox.max.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.max.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.max.y, m_BBox.max.z );
		glVertex3f( m_BBox.min.x, m_BBox.max.y, m_BBox.max.z );
		glEnd();

		glBegin(GL_LINES);
		glVertex3f( m_BBox.min.x, m_BBox.min.y, m_BBox.min.z );
		glVertex3f( m_BBox.min.x, m_BBox.max.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.min.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.max.y, m_BBox.min.z );
		glVertex3f( m_BBox.max.x, m_BBox.min.y, m_BBox.max.z );
		glVertex3f( m_BBox.max.x, m_BBox.max.y, m_BBox.max.z );
		glVertex3f( m_BBox.min.x, m_BBox.min.y, m_BBox.max.z );
		glVertex3f( m_BBox.min.x, m_BBox.max.y, m_BBox.max.z );
		glEnd();
		
	}
	else {
		if( m_nLOD>=0 )
			for(int i=0; i<4; i++)
				m_pChildren[i].DrawBBox(bTest);
	}
}

// Affichage du quadtree récursivement
int QuadtreeNode::DrawGround(Frustum* pFrust, int options)
{
	assert(pFrust);

	m_nLOD = -1;

	vec3 center = m_BBox.getCenter();				// centre de la Bounding Sphere
	float radius = (m_BBox.max-center).length();	// rayon de la Bounding Sphere

	if(options & CHUNK_BIT_TESTCHILDREN) {
		// Si on n'est pas dans le noeud :
		if(!m_BBox.ContainsPoint(pFrust->getEyePos()))
		{
			int resSphereInFrustum = pFrust->ContainsSphere(center, radius);
			switch(resSphereInFrustum) {
				case FRUSTUM_OUT: return 0;		//si la "sphere" n'est pas dans le champ de vision
				case FRUSTUM_IN:
					options &= ~CHUNK_BIT_TESTCHILDREN;				//si la "sphere" est completement dans le champ de vision
					break;		
				case FRUSTUM_INTERSECT:								//si la "sphere" est partiellement dans le champ de vision
				{		
					int resBoxInFrustum = pFrust->ContainsBoundingBox(m_BBox);
					switch(resBoxInFrustum) {
						case FRUSTUM_IN: options &= ~CHUNK_BIT_TESTCHILDREN; break;
						case FRUSTUM_OUT: return 0;
					}
				}
			}
		}
	}

	m_nLOD = 0; 

	if(!m_pChildren) {
		assert(m_pTerrainChunk);

		if(options & CHUNK_BIT_WATERREFLECTION) {
			// Rendu du terrain réflechi avec la plus basse définition
			return m_pTerrainChunk->DrawGround(TERRAIN_CHUNKS_LOD-1);
		}
		else {
			// Calcul du niveau de LOD en fonction de la distance entre l'oeil et le chunk
			vec3 vEyeToChunk = this->getBoundingBox().getCenter() - pFrust->getEyePos();
			m_fDistance = vEyeToChunk.length();
			GLuint lod = 0;
			if(m_fDistance > TERRAIN_CHUNK_LOD1)		lod = 2;
			else if(m_fDistance > TERRAIN_CHUNK_LOD0)	lod = 1;
			m_nLOD = lod;

			return m_pTerrainChunk->DrawGround(lod);
		}
	}
	else {
		int ret = 0;
		for(int i=0; i<4; i++)
			ret += m_pChildren[i].DrawGround(pFrust, options);
		return ret;
	}

}


