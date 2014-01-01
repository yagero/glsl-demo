#include "TerrainChunk.h"
#include "TerrainObject.h"
#include <assert.h>

#define COORD(x,y)	((y)*(nHMTotalWidth)+(x))

void TerrainChunk::Load(unsigned int depth, ivec2 pos, ivec2 HMsize)
{
	for(unsigned int i=0; i<TERRAIN_CHUNKS_LOD; i++)
		ComputeIndicesArray(i, depth, pos, HMsize);
}


void TerrainChunk::addObject(TerrainObject* obj)
{
	m_tObject.push_back(obj);
}




void TerrainChunk::ComputeIndicesArray(unsigned int lod, unsigned int depth, ivec2 pos, ivec2 HMsize)
{
	assert(lod < TERRAIN_CHUNKS_LOD);

	ivec2 vHeightmapDataPos = pos;
	unsigned int offset = (unsigned int)pow(2.0f, (float)(lod));
	unsigned int div = (unsigned int)pow(2.0f, (float)(depth+lod));
	ivec2 vHeightmapDataSize = HMsize/(div) + ivec2(1,1);

	GLuint nHMWidth = (GLuint)vHeightmapDataSize.x;
	GLuint nHMHeight = (GLuint)vHeightmapDataSize.y;
	GLuint nHMOffsetX = (GLuint)vHeightmapDataPos.x;
	GLuint nHMOffsetY = (GLuint)vHeightmapDataPos.y;

	GLuint nHMTotalWidth = (GLuint)HMsize.x;
	GLuint nHMTotalHeight = (GLuint)HMsize.y;

	m_tIndOffsetW[lod] = nHMWidth*2;
	m_tIndOffsetH[lod] = nHMWidth-1;
	GLuint nIndice = (nHMWidth)*(nHMHeight-1)*2;
	m_tIndice[lod].reserve( nIndice );


	for(GLuint j=0; j<(GLuint)nHMHeight-1; j++)
	{
		for(GLuint i=0; i<(GLuint)nHMWidth; i++)
		{
			GLuint id0 = COORD(i*(offset) + nHMOffsetX+0, j*(offset) + nHMOffsetY+0);
			GLuint id1 = COORD(i*(offset) + nHMOffsetX+0, j*(offset) + nHMOffsetY+(offset));
			m_tIndice[lod].push_back( id0 );
			m_tIndice[lod].push_back( id1 );
		}
	}

	assert(nIndice == m_tIndice[lod].size());
	
}





void TerrainChunk::Destroy()
{
	for(GLuint i=0; i<TERRAIN_CHUNKS_LOD; i++)
		m_tIndice[i].clear();

	for(int i=0; i<(int)m_tObject.size(); i++)
		delete m_tObject[i];
}


int TerrainChunk::DrawObjects(GLuint lod)
{
	assert(lod < TERRAIN_CHUNKS_LOD);

	for(int i=0; i<(int)m_tObject.size(); i++)
		m_tObject[i]->Draw(lod);

	return (int)m_tObject.size();
}


int TerrainChunk::DrawGround(GLuint lod)
{
	assert(lod < TERRAIN_CHUNKS_LOD);
	if(lod>0) lod--;

	for(GLuint j=0; j<(GLuint)m_tIndOffsetH[lod]; j++)
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)m_tIndOffsetW[lod], GL_UNSIGNED_INT, &(m_tIndice[lod][j*m_tIndOffsetW[lod]]) );

	return 1;
}

int  TerrainChunk::DrawGrass(GLuint lod, float d)
{
	assert(lod < TERRAIN_CHUNKS_LOD);
	if(lod != 0) return 0;
	if(d > TERRAIN_GRASS_MAX_DISTANCE) return 0;

	if(m_tGrassIndice.size() > 0)
	{
		float ratio = 1.0f - (d / TERRAIN_GRASS_MAX_DISTANCE);
		ratio *= 2.0f;
		if(ratio > 1.0f) ratio = 1.0f;

		GLsizei indices_count = (GLsizei)( ratio * m_tGrassIndice.size() );
		indices_count -= indices_count%4; // c'est des quads

		if(indices_count > 0)
			glDrawElements(GL_QUADS, indices_count, GL_UNSIGNED_INT, &(m_tGrassIndice[0]) );
	}

	return 1;
}





