#ifndef VBO_H
#define VBO_H

#include <iostream>
#include <GL/glew.h>
#include "Mathlib.h"
#include <vector>

class VertexBufferObject
{
public:
	bool		Create(GLenum usage);
	void		Destroy();

	// Activation et désactivation du VB
	void		Enable();
	void		Disable();

	// Buffers de données
	inline std::vector<vec3>&	getPosition()	{return m_tDataPosition;}
	inline std::vector<vec3>&	getNormal()		{return m_tDataNormal;}
	inline std::vector<vec2>&	getTexcoord()	{return m_tDataTexcoord;}
	inline std::vector<vec3>&	getTangent()	{return m_tDataTangent;}

	VertexBufferObject();
	~VertexBufferObject() {Destroy();}

private:
	void Enable_VA();	// Activation en Vertex Array
	void Enable_VBO();	// Activation en Vertex Buffer Object
	void Disable_VA();	// Désactivation en Vertex Array
	void Disable_VBO();	// Désactivation en Vertex Buffer Object

private:
	// Identifiants pour le VBO
	GLuint		m_nVBOid;
	GLintptr	m_nVBO_OffsetPosition;
	GLintptr	m_nVBO_OffsetNormal;
	GLintptr	m_nVBO_OffsetTexcoord;
	GLintptr	m_nVBO_OffsetTangent;

	// Données :
	std::vector<vec3>	m_tDataPosition;
	std::vector<vec3>	m_tDataNormal;
	std::vector<vec2>	m_tDataTexcoord;
	std::vector<vec3>	m_tDataTangent;
};

#endif

