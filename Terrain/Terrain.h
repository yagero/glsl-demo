#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <GL/glew.h>
#include "../Mathlib.h"
#include "../BoundingBox.h"
#include "../ImageTools.h"
#include <vector>

class Quadtree;
class VertexBufferObject;
class TerrainObject;

class Terrain
{
public:
	void Load(const std::string& heightmap, const BoundingBox& bbox, GLuint chunkSize=64);
	bool GenerateGrass(const ImageTools::ImageData& map, unsigned int density);
	bool GenerateVegetation(const ImageTools::ImageData& map, unsigned int density);
	bool ComputeBoundingBox();

	void Destroy();
	int  DrawGround(bool bReflection);
	int  DrawGrass(bool bReflection);
	int  DrawObjects(bool bReflection);
	void DrawInfinitePlane(const vec3& eye, float max_distance);

	vec3  getPosition(float x_clampf, float z_clampf) const;
	vec3  getNormal(float x_clampf, float z_clampf) const;
	vec3  getTangent(float x_clampf, float z_clampf) const;

	const BoundingBox&	getBoundingBox()	{return m_BBox;}

	Terrain();
	~Terrain() {Destroy();}

private:

	
private:
	BoundingBox				m_BBox;
	GLuint					m_nHMWidth, m_nHMHeight;

	// Quadtree pour gérer les données
	Quadtree*				m_pQuadtree;

	// VBO
	VertexBufferObject*		m_pGroundVBO;
	VertexBufferObject*		m_pGrassVBO;


};

#endif

