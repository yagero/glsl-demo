#include "Quadtree.h"
#include "QuadtreeNode.h"
#include "../BoundingBox.h"
#include "../Frustum.h"
#include <assert.h>
 
using namespace std;

int Quadtree::DrawGround(bool bReflection) {
	assert(m_pRoot);
	int options = CHUNK_BIT_TESTCHILDREN;
	if(bReflection)
		options |= CHUNK_BIT_WATERREFLECTION;
	return m_pRoot->DrawGround(&Frustum::getInstance(), options);
}

int Quadtree::DrawGrass(bool bReflection)
{
	assert(m_pRoot);
	return m_pRoot->DrawGrass(bReflection);
}

int Quadtree::DrawObjects(bool bReflection)
{
	assert(m_pRoot);
	return m_pRoot->DrawObjects(bReflection);
}


void Quadtree::DrawBBox() {
	assert(m_pRoot);
	m_pRoot->DrawBBox(true);
}


QuadtreeNode* Quadtree::FindLeaf(vec2& pos)
{
	assert(m_pRoot);
	QuadtreeNode* node = m_pRoot;

	while(!node->isALeaf()) {
		int i=0;
		for(i=0; i<4; i++) {
			QuadtreeNode* child = &(node->getChildren()[i]);
			if(child->getBoundingBox().ContainsPoint( vec3(pos.x, child->getBoundingBox().getCenter().y, pos.y) ))
			{
				node = child;
				break;
			}
		}

		if(i>=4) {
			return NULL;
		}
	}

	return node;
}


void Quadtree::Build(BoundingBox* pBBox,		// Bounding box englobant le terrain
					 ivec2 HMsize,				// longueur et largeur de la heightmap
					 unsigned int minHMSize)	// taille minimale des chunks
{
	assert(pBBox);

	// Création du noeud principal
	m_pRoot = new QuadtreeNode();
	m_pRoot->setBoundingBox(*pBBox);

	// fonction récursive de création des noeuds
	m_pRoot->Build(0, ivec2(0,0), HMsize, minHMSize);

	// fonction récursive calcul de bounding box
//	m_pRoot->ComputeBoundingBox(vertices);

//	*pBBox = m_pRoot->getBoundingBox();
}

void Quadtree::ComputeBoundingBox(const vec3* vertices)
{
	assert(m_pRoot);
	assert(vertices);

	// fonction récursive calcul de bounding box
	m_pRoot->ComputeBoundingBox(vertices);

}


void Quadtree::Destroy()
{
	if(m_pRoot) {
		m_pRoot->Destroy();
		delete m_pRoot;
		m_pRoot = NULL;
	}
}


