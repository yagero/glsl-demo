#ifndef QUADTREE
#define QUADTREE

#include <iostream>
#include <string>
#include "../Mathlib.h"

class QuadtreeNode;
class BoundingBox;

class Quadtree {
public:
	void Build(BoundingBox* pBBox, ivec2 HMSize, unsigned int minHMSize);
	void ComputeBoundingBox(const vec3* vertices);
	void Destroy();

	int  DrawGround(bool bReflection);
	int  DrawGrass(bool bReflection);
	int  DrawObjects(bool bReflection);
	void DrawBBox();

	QuadtreeNode*	FindLeaf(vec2& pos);

	Quadtree()	{m_pRoot = NULL;}
	~Quadtree()	{Destroy();}

private:
	QuadtreeNode*	m_pRoot;	// Noeud principal du Quadtree
};

#endif

