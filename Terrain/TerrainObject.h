#ifndef TERRAINOBJECT_H
#define TERRAINOBJECT_H

#include <iostream>
#include <GL/glew.h>
#include "../Mathlib.h"
#include <vector>

class Mesh;

class TerrainObject
{
public:
	enum TYPE {PALM};

	void Draw(unsigned int id = 0);
	void Destroy();

	inline Mesh*		getMesh(unsigned int id = 0)	const {return m_tMesh[id];}
	inline const vec3&	getPosition()					const {return m_vPosition;}

	TerrainObject(TYPE mesh, vec4 tr);
	~TerrainObject() {Destroy();}
private:
	vec3				m_vPosition;
	float				m_fAngle;
	std::vector<Mesh*>	m_tMesh;

};

#endif

