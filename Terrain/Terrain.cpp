#include "Terrain.h"
#include "../ImageTools.h"
#include "../VarManager.h"
#include "../BoundingBox.h"
#include <assert.h>
#include "Quadtree.h"
#include "QuadtreeNode.h"
#include "TerrainChunk.h"
#include "TerrainObject.h"
#include "../VertexBufferObject.h"

#define COORD(x,y,w)	((y)*(w)+(x))

Terrain::Terrain()
{
	m_nHMWidth = 0;
	m_nHMHeight = 0;
	m_pQuadtree = NULL;
	m_pGroundVBO = NULL;
	m_pGrassVBO = NULL;
}

void Terrain::Destroy()
{
	m_nHMWidth = 0;
	m_nHMHeight = 0;
//	m_tIndice.clear();

	if(m_pQuadtree) {
		m_pQuadtree->Destroy();
		delete m_pQuadtree;
		m_pQuadtree = NULL;
	}
	if(m_pGroundVBO) {
		m_pGroundVBO->Destroy();
		delete m_pGroundVBO;
		m_pGroundVBO = NULL;		
	}


}





void Terrain::Load(const std::string& heightmap, const BoundingBox& bbox, GLuint chunkSize)
{
	std::cout << "Loading Terrain..." << std::endl;

	m_pGroundVBO = new VertexBufferObject();
	std::vector<vec3>&		tPosition	= m_pGroundVBO->getPosition();
	std::vector<vec3>&		tNormal		= m_pGroundVBO->getNormal();
	std::vector<vec3>&		tTangent	= m_pGroundVBO->getTangent();

	// Chargement de la heightmap :
	unsigned int d;
	GLubyte* data = ImageTools::OpenImage(heightmap, m_nHMWidth, m_nHMHeight, d);
	assert(data!=NULL);


	// Taille du terrain à créer
	m_BBox = bbox;

	// Taille de l'image
	GLuint nIMGWidth = m_nHMWidth;
	GLuint nIMGHeight = m_nHMHeight;

	// La heightmap doit être de taille impaire
	// afin de pouvoir la découper dans le quadtree
	if(m_nHMWidth%2==0)		m_nHMWidth++;
	if(m_nHMHeight%2==0)	m_nHMHeight++;


	// Remplissage du tableau de vertices
	tPosition.resize(m_nHMWidth*m_nHMHeight);
	for(GLuint j=0; j<m_nHMHeight; j++) {
		for(GLuint i=0; i<m_nHMWidth; i++) {
			// Coordonnées de longueur et largeur
			GLuint idxHM = COORD(i,j,m_nHMWidth);
			tPosition[idxHM].x = m_BBox.min.x + ((float)i) * (m_BBox.max.x - m_BBox.min.x)/(m_nHMWidth-1);
			tPosition[idxHM].z = m_BBox.min.z + ((float)j) * (m_BBox.max.z - m_BBox.min.z)/(m_nHMHeight-1);

			// Coordonnées de hauteur
			// Comme on a peut etre agrandie la heightmap pour la rendre impaire,
			// on va peut être aller d'un cran trop loin dans l'image
			GLuint idxIMG = COORD(	i<nIMGWidth? i : i-1,
									j<nIMGHeight? j : j-1,
									nIMGWidth);
			float h = (float)(data[idxIMG*d + 0] + data[idxIMG*d + 1] + data[idxIMG*d + 2])/3;
			tPosition[idxHM].y = m_BBox.min.y + ((float)h) * (m_BBox.max.y - m_BBox.min.y)/(255);

		}
	}

	delete[] data;
	



	GLuint offset = 2;

	// Remplissage du tableau de normales et de tangentes
	tNormal.resize(m_nHMWidth*m_nHMHeight);
	tTangent.resize(m_nHMWidth*m_nHMHeight);
	for(GLuint j=offset; j<m_nHMHeight-offset; j++) {
		for(GLuint i=offset; i<m_nHMWidth-offset; i++) {
			GLuint idx = COORD(i,j,m_nHMWidth);

			vec3 vU = tPosition[COORD(i+offset, j+0, m_nHMWidth)] - tPosition[COORD(i-offset, j+0, m_nHMWidth)];
			vec3 vV = tPosition[COORD(i+0, j+offset, m_nHMWidth)] - tPosition[COORD(i+0, j-offset, m_nHMWidth)];

			tNormal[idx].cross(vV, vU);
			tNormal[idx].normalize();
			tTangent[idx] = -vU;
			tTangent[idx].normalize();
		}
	}

	// Copie des normales et tangentes sur les bords selon la largeur
	for(GLuint j=0; j<offset; j++) {
		for(GLuint i=0; i<m_nHMWidth; i++) {
			GLuint idx0 = COORD(i,	j,		m_nHMWidth);
			GLuint idx1 = COORD(i,	offset,	m_nHMWidth);

			tNormal[idx0] = tNormal[idx1];
			tTangent[idx0] = tTangent[idx1];

			idx0 = COORD(i,	m_nHMHeight-1-j,		m_nHMWidth);
			idx1 = COORD(i,	m_nHMHeight-1-offset,	m_nHMWidth);

			tNormal[idx0] = tNormal[idx1];
			tTangent[idx0] = tTangent[idx1];
		}
	}

	// Copie des normales et tangentes sur les bords selon la longueur
	for(GLuint i=0; i<offset; i++) {
		for(GLuint j=0; j<m_nHMHeight; j++) {
			GLuint idx0 = COORD(i,		j,		m_nHMWidth);
			GLuint idx1 = COORD(offset,	j,	m_nHMWidth);

			tNormal[idx0] = tNormal[idx1];
			tTangent[idx0] = tTangent[idx1];

			idx0 = COORD(m_nHMWidth-1-i,		j,	m_nHMWidth);
			idx1 = COORD(m_nHMWidth-1-offset,	j,	m_nHMWidth);

			tNormal[idx0] = tNormal[idx1];
			tTangent[idx0] = tTangent[idx1];
		}
	}

	// Génération du VBO
	m_pGroundVBO->Create(GL_STATIC_DRAW);
	std::cout << "Loading Terrain OK" << std::endl;


	// Génération du Quadtree
	std::cout << "Loading Quadtree..." << std::endl;
	m_pQuadtree = new Quadtree();
	m_pQuadtree->Build(&m_BBox, ivec2(m_nHMWidth, m_nHMHeight), chunkSize);
	std::cout << "Loading Quadtree OK" << std::endl;
}


bool Terrain::ComputeBoundingBox()
{
	m_pQuadtree->ComputeBoundingBox( &(m_pGroundVBO->getPosition()[0]) );
	return true;
}




bool Terrain::GenerateVegetation(const ImageTools::ImageData& map, unsigned int density)
{
	std::cout << "Generating Vegetation..." << std::endl;
	VarManager& var = VarManager::getInstance();

	for(int k=0; k<(int)density; k++) {
		float x = random(1.0f);
		float y = random(1.0f);
		int map_x = (int)(x * map.w);
		int map_y = (int)(y * map.h);
		ivec3 map_color = map.getColor(map_x, map_y);

		if(map_color.blue < 25) {
			k--;
			continue;
		}

		vec3 P = getPosition(x, y);

		if(P.y < var.getf("water_height")) {
			k--;
			continue;
		}

		QuadtreeNode* node = m_pQuadtree->FindLeaf(vec2(P.x, P.z));
		assert(node);
		TerrainChunk* chunk = node->getChunk();
		assert(chunk);

		chunk->addObject( new TerrainObject(	TerrainObject::PALM,
												vec4(P.x, P.y-1.0f, P.z, random(360.0f))
											) );
	}

	std::cout << "Generating Vegetation OK" << std::endl;
	return true;
}

bool Terrain::GenerateGrass(const ImageTools::ImageData& map, unsigned int density)
{
	std::cout << "Generating Grass..." << std::endl;
	assert(map.data);

	vec2 pos0(cosf(RADIANS(0.0f)), sinf(RADIANS(0.0f)));
	vec2 pos120(cosf(RADIANS(120.0f)), sinf(RADIANS(120.0f)));
	vec2 pos240(cosf(RADIANS(240.0f)), sinf(RADIANS(240.0f)));

	vec3 tVertices[] = {
		vec3(-pos0.x, -pos0.y, 0.0f),		vec3(-pos0.x, -pos0.y, 1.0f),		vec3(pos0.x, pos0.y, 1.0f),		vec3(pos0.x, pos0.y, 0.0f),
		vec3(-pos120.x, -pos120.y, 0.0f),	vec3(-pos120.x, -pos120.y, 1.0f),	vec3(pos120.x, pos120.y, 1.0f),	vec3(pos120.x, pos120.y, 0.0f),
		vec3(-pos240.x, -pos240.y, 0.0f),	vec3(-pos240.x, -pos240.y, 1.0f),	vec3(pos240.x, pos240.y, 1.0f),	vec3(pos240.x, pos240.y, 0.0f)
	};

	vec2 tTexcoords[] = {
		vec2(0.0f, 0.49f), vec2(0.0f, 0.01f), vec2(1.0f, 0.01f), vec2(1.0f, 0.49f),
		vec2(0.0f, 0.49f), vec2(0.0f, 0.01f), vec2(1.0f, 0.01f), vec2(1.0f, 0.49f),
		vec2(0.0f, 0.49f), vec2(0.0f, 0.01f), vec2(1.0f, 0.01f), vec2(1.0f, 0.49f)
	};

	m_pGrassVBO = new VertexBufferObject();


	m_pGrassVBO->getPosition().reserve( density * 3*4 );
	m_pGrassVBO->getNormal().reserve( density * 3*4 );
	m_pGrassVBO->getTexcoord().reserve( density * 3*4 );

	for(int k=0; k<(int)density; k++) {
		float x = random(1.0f);
		float y = random(1.0f);
		int map_x = (int)(x * map.w);
		int map_y = (int)(y * map.h);
		vec2 uv_offset = vec2(0.0f, random(3)==0 ? 0.0f : 0.5f);
		float size = random(0.5f) + 0.5f;

		ivec3 map_color = map.getColor(map_x, map_y);
		if(map_color.green < 150) {
			k--;
			continue;
		}

		size = (float)(map_color.green+1) / 256;

		vec3 P = getPosition(x, y);
	//	if(P.y >= minH && P.y <= maxH)
		{
			vec3 N = getNormal(x, y);
			vec3 T = getTangent(x, y);
			vec3 B = Cross(N, T);

			if(N.y < 0.8f) {
				k--;
				continue;
			}
			else {
				mat3 matRot;
				matRot.rotate_z(random(360.0f));

				GLuint idx = (GLuint)m_pGrassVBO->getPosition().size();

				QuadtreeNode* node = m_pQuadtree->FindLeaf(vec2(P.x, P.z));
				assert(node);
				TerrainChunk* chunk = node->getChunk();
				assert(chunk);

				for(int i=0; i<3*4; i++)
				{
					vec3 data = matRot*(tVertices[i]*size);
					vec3 vertex = P;
					vertex.x += Dot(data, T);
					vertex.y += Dot(data, B);
					vertex.z += Dot(data, N);


					m_pGrassVBO->getPosition().push_back( vertex );
					m_pGrassVBO->getNormal().push_back( tTexcoords[i].t < 0.2f ? -N : N );
					m_pGrassVBO->getTexcoord().push_back( uv_offset + tTexcoords[i] );

					chunk->getGrassIndiceArray().push_back(idx+i);
				}



			}
		}
	}

	bool ret = m_pGrassVBO->Create(GL_STATIC_DRAW);

	std::cout << "Generating Grass OK" << std::endl;
	return ret;
}




vec3 Terrain::getPosition(float x_clampf, float z_clampf) const
{
	if(x_clampf<.0f || z_clampf<.0f || x_clampf>1.0f || z_clampf>1.0f) return vec3(0.0f, 0.0f, 0.0f);

	vec2  posF(	x_clampf * m_nHMWidth, z_clampf * m_nHMHeight );
	ivec2 posI(	(int)(posF.x), (int)(posF.y) );
	vec2  posD(	posF.x - posI.x, posF.y - posI.y );

	if(posI.x >= (int)m_nHMWidth-1)		posI.x = m_nHMWidth-2;
	if(posI.y >= (int)m_nHMHeight-1)	posI.y = m_nHMHeight-2;
	assert(posI.x>=0 && posI.x<(int)m_nHMWidth-1 && posI.y>=0 && posI.y<(int)m_nHMHeight-1);

	vec3 pos(m_BBox.min.x + x_clampf * (m_BBox.max.x - m_BBox.min.x), 0.0f, m_BBox.min.z + z_clampf * (m_BBox.max.z - m_BBox.min.z));
	pos.y =   (m_pGroundVBO->getPosition()[ COORD(posI.x,  posI.y,  m_nHMWidth) ].y)  * (1.0f-posD.x) * (1.0f-posD.y)
			+ (m_pGroundVBO->getPosition()[ COORD(posI.x+1,posI.y,  m_nHMWidth) ].y)  *       posD.x  * (1.0f-posD.y)
			+ (m_pGroundVBO->getPosition()[ COORD(posI.x,  posI.y+1,m_nHMWidth) ].y)  * (1.0f-posD.x) *       posD.y
			+ (m_pGroundVBO->getPosition()[ COORD(posI.x+1,posI.y+1,m_nHMWidth) ].y)  *       posD.x  *       posD.y;
	return pos;
}

vec3 Terrain::getNormal(float x_clampf, float z_clampf) const
{
	if(x_clampf<.0f || z_clampf<.0f || x_clampf>1.0f || z_clampf>1.0f) return vec3(0.0f, 1.0f, 0.0f);

	vec2  posF(	x_clampf * m_nHMWidth, z_clampf * m_nHMHeight );
	ivec2 posI(	(int)(x_clampf * m_nHMWidth), (int)(z_clampf * m_nHMHeight) );
	vec2  posD(	posF.x - posI.x, posF.y - posI.y );

	if(posI.x >= (int)m_nHMWidth-1)		posI.x = m_nHMWidth-2;
	if(posI.y >= (int)m_nHMHeight-1)	posI.y = m_nHMHeight-2;
	assert(posI.x>=0 && posI.x<(int)m_nHMWidth-1 && posI.y>=0 && posI.y<(int)m_nHMHeight-1);

	return    (m_pGroundVBO->getNormal()[ COORD(posI.x,  posI.y,  m_nHMWidth) ])  * (1.0f-posD.x) * (1.0f-posD.y)
			+ (m_pGroundVBO->getNormal()[ COORD(posI.x+1,posI.y,  m_nHMWidth) ])  *       posD.x  * (1.0f-posD.y)
			+ (m_pGroundVBO->getNormal()[ COORD(posI.x,  posI.y+1,m_nHMWidth) ])  * (1.0f-posD.x) *       posD.y
			+ (m_pGroundVBO->getNormal()[ COORD(posI.x+1,posI.y+1,m_nHMWidth) ])  *       posD.x  *       posD.y;
}

vec3 Terrain::getTangent(float x_clampf, float z_clampf) const
{
	if(x_clampf<.0f || z_clampf<.0f || x_clampf>1.0f || z_clampf>1.0f) return vec3(1.0f, 0.0f, 0.0f);

	vec2  posF(	x_clampf * m_nHMWidth, z_clampf * m_nHMHeight );
	ivec2 posI(	(int)(x_clampf * m_nHMWidth), (int)(z_clampf * m_nHMHeight) );
	vec2  posD(	posF.x - posI.x, posF.y - posI.y );

	if(posI.x >= (int)m_nHMWidth-1)		posI.x = m_nHMWidth-2;
	if(posI.y >= (int)m_nHMHeight-1)	posI.y = m_nHMHeight-2;
	assert(posI.x>=0 && posI.x<(int)m_nHMWidth-1 && posI.y>=0 && posI.y<(int)m_nHMHeight-1);

	return    (m_pGroundVBO->getTangent()[ COORD(posI.x,  posI.y,  m_nHMWidth) ])  * (1.0f-posD.x) * (1.0f-posD.y)
			+ (m_pGroundVBO->getTangent()[ COORD(posI.x+1,posI.y,  m_nHMWidth) ])  *       posD.x  * (1.0f-posD.y)
			+ (m_pGroundVBO->getTangent()[ COORD(posI.x,  posI.y+1,m_nHMWidth) ])  * (1.0f-posD.x) *       posD.y
			+ (m_pGroundVBO->getTangent()[ COORD(posI.x+1,posI.y+1,m_nHMWidth) ])  *       posD.x  *       posD.y;
}







void Terrain::DrawInfinitePlane(const vec3& eye, float max_distance)
{
	float h = getBoundingBox().min.y;

	glActiveTexture(GL_TEXTURE0);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord3f(1.0f, 0.0f, 0.0f);
		glVertex3f(eye.x - max_distance, h, eye.z - max_distance);
		glVertex3f(eye.x - max_distance, h, eye.z + max_distance);
		glVertex3f(eye.x + max_distance, h, eye.z + max_distance);
		glVertex3f(eye.x + max_distance, h, eye.z - max_distance);
	glEnd();

}


int Terrain::DrawObjects(bool bReflection)
{
	glPushAttrib(GL_POLYGON_BIT);
	glDisable(GL_CULL_FACE);
		int ret = m_pQuadtree->DrawObjects(bReflection);
	glPopAttrib();

	return ret;
}

int Terrain::DrawGrass(bool bReflection)
{
//	assert(m_pGrassVBO);
	if(m_pGrassVBO)
	{
		glPushAttrib(GL_POLYGON_BIT);
		glDisable(GL_CULL_FACE);
		m_pGrassVBO->Enable();
			int ret = m_pQuadtree->DrawGrass(bReflection);
		m_pGrassVBO->Disable();
		glPopAttrib();

		return ret;
	}

	return 0;
}


int Terrain::DrawGround(bool bReflection)
{
	assert(m_pGroundVBO);

	m_pGroundVBO->Enable();
		int ret = m_pQuadtree->DrawGround(bReflection);
	m_pGroundVBO->Disable();

	return ret;
}

//////////////////////////////////////////////////////////////
// TESTS avec une heightmap de 1024x1024 affichage brut :
// GL_TRIANGLES avec VA : 10 FPS
// GL_TRIANGLES avec VBO : 28 FPS
// GL_TRIANGLE_STRIP avec VA : 23 FPS
// GL_TRIANGLE_STRIP avec VBO : 60 FPS
//////////////////////////////////////////////////////////////



