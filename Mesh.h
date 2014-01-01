#ifndef MESH_H
#define MESH_H


#include <iostream>
#include <GL/glew.h>
#include "Mathlib.h"
#include <vector>
#include "BoundingBox.h"
#include "ResourceBase.h"

class VertexBufferObject;

// -------------------------------
// Objet 3D
// chargement à partir de modèles .obj
// -------------------------------

class Mesh : public ResourceBase
{
public:
	virtual bool Load(const std::string& name);
	virtual void Destroy();

	void Draw();
	void Draw(GLuint group);

	inline GLuint		getGroupCount()		const	{return (GLuint)m_tGroup.size();}
	inline BoundingBox&	getBoundingBox()			{return m_BBox;}

	static void EnableComputeNormals(bool b) {s_bComputeNormals=b;}

	Mesh();
	~Mesh() {Destroy();}
private:
	bool LoadOBJ(const std::string& filename);

	bool ComputeVBO();
	void ComputeNormals();
	void ComputeTangents();
	void ComputeBoundingBox();

private:
	struct sFace {
		GLuint ind[3];
	};

	struct sGroup {
		std::string			strName;
		long				nMaterial;
		std::vector<sFace>	tFace;
	};

private:
	std::vector<sGroup>		m_tGroup;

	BoundingBox				m_BBox;

	// VBO
	VertexBufferObject*		m_pVBO;

	static bool s_bComputeNormals;	// Etat pour le chargement : génération ou non des normales à la main

};

#endif

