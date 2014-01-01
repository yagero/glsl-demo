#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

#include <iostream>
#include <GL/glew.h>
#include "../Mathlib.h"
#include <vector>

#define TERRAIN_CHUNKS_LOD 3

#define TERRAIN_CHUNK_LOD0	100.0f
#define TERRAIN_CHUNK_LOD1	180.0f

#define TERRAIN_GRASS_MAX_DISTANCE	80.0f

class TerrainObject;

class TerrainChunk
{
public:
	void Destroy();
	int  DrawGround(GLuint lod);
	int  DrawGrass(GLuint lod, float d);
	int  DrawObjects(GLuint lod);

	void Load(unsigned int depth, ivec2 pos, ivec2 HMsize);

	inline std::vector<GLuint>&				getIndiceArray(GLuint lod)	{return m_tIndice[lod];}
	inline std::vector<GLuint>&				getGrassIndiceArray()		{return m_tGrassIndice;}
	inline std::vector<TerrainObject*>&		getObjectsArray()			{return m_tObject;}
	void									addObject(TerrainObject* obj);

	TerrainChunk() {}
	~TerrainChunk() {Destroy();}

private:
	void ComputeIndicesArray(unsigned int lod, unsigned int depth, ivec2 pos, ivec2 HMsize);
	
private:
	// Indices pour le terrain
	std::vector<GLuint>		m_tIndice[TERRAIN_CHUNKS_LOD];
	GLuint					m_tIndOffsetW[TERRAIN_CHUNKS_LOD];
	GLuint					m_tIndOffsetH[TERRAIN_CHUNKS_LOD];

	// Indices pour l'herbe
	std::vector<GLuint>		m_tGrassIndice;

	// Objets:
	std::vector<TerrainObject*>	m_tObject;

};

#endif

