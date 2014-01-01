#include "VertexBufferObject.h"


VertexBufferObject::VertexBufferObject()
{
	m_nVBOid = 0;

}

void VertexBufferObject::Destroy()
{
	glDeleteBuffersARB(1, &m_nVBOid);
	m_nVBOid = 0;

	m_tDataPosition.clear();
	m_tDataNormal.clear();
	m_tDataTexcoord.clear();
	m_tDataTangent.clear();
}

/*
- GL_STREAM_DRAW lorsque les informations sur les sommets peuvent être mise à jour entre chaque affichage.
- GL_DYNAMIC_DRAW lorsque les informations sur les sommets peuvent être mise à jour entre chaque frame.
- GL_STATIC_DRAW lorsque les informations sur les sommets ne sont jamais ou presque jamais mise à jour.
*/
bool VertexBufferObject::Create(GLenum usage)
{
	m_nVBOid = 0;

	GLsizeiptr	nSizePosition = 0;
	GLsizeiptr	nSizeNormal = 0;
	GLsizeiptr	nSizeTexcoord = 0;
	GLsizeiptr	nSizeTangent = 0;


	if(!m_tDataPosition.empty()) {
		nSizePosition = m_tDataPosition.size()*sizeof(vec3);
	}
	else {
		std::cerr << "[ERROR] No position data !" << std::endl;
		return false;
	}

	

	if(!m_tDataNormal.empty()) {
		nSizeNormal	= m_tDataNormal.size()*sizeof(vec3);
	}

	if(!m_tDataTexcoord.empty()) {
		nSizeTexcoord = m_tDataTexcoord.size()*sizeof(vec2);
	}

	if(!m_tDataTangent.empty()) {
		nSizeTangent = m_tDataTangent.size()*sizeof(vec3);
	}


	m_nVBO_OffsetPosition	= 0;
	m_nVBO_OffsetNormal		= m_nVBO_OffsetPosition + nSizePosition;
	m_nVBO_OffsetTexcoord	= m_nVBO_OffsetNormal + nSizeNormal;
	m_nVBO_OffsetTangent	= m_nVBO_OffsetTexcoord + nSizeTexcoord;

	glGenBuffersARB(1, &m_nVBOid);
	if(m_nVBOid == 0) {
		std::cerr << "[ERROR] Init VBO failed !" << std::endl;
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBOid);
		glBufferData(GL_ARRAY_BUFFER, nSizePosition+nSizeNormal+nSizeTexcoord+nSizeTangent, 0, usage);

		glBufferSubData(GL_ARRAY_BUFFER_ARB, m_nVBO_OffsetPosition,	nSizePosition,	(const GLvoid*)(&m_tDataPosition[0]));

		if(m_tDataNormal.size())
			glBufferSubData(GL_ARRAY_BUFFER_ARB, m_nVBO_OffsetNormal,	nSizeNormal,	(const GLvoid*)(&m_tDataNormal[0]));

		if(m_tDataTexcoord.size())
			glBufferSubData(GL_ARRAY_BUFFER_ARB, m_nVBO_OffsetTexcoord,	nSizeTexcoord,	(const GLvoid*)(&m_tDataTexcoord[0]));

		if(m_tDataTangent.size())
			glBufferSubData(GL_ARRAY_BUFFER_ARB, m_nVBO_OffsetTangent,	nSizeTangent,	(const GLvoid*)(&m_tDataTangent[0]));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}


void VertexBufferObject::Enable()
{
	if(m_nVBOid)	Enable_VBO();		// avec Vertex Buffer Object
	else			Enable_VA();		// avec Vertex Array
}

void VertexBufferObject::Disable()
{
	if(m_nVBOid)	Disable_VBO();		// avec Vertex Buffer Object
	else			Disable_VA();		// avec Vertex Array
}

void VertexBufferObject::Enable_VA()
{
	unsigned int slot = 0;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &(m_tDataPosition[0].x));

	if(!m_tDataNormal.empty()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &(m_tDataNormal[0].x));
	}

	if(!m_tDataTexcoord.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &(m_tDataTexcoord[0].s));
	}

	if(!m_tDataTangent.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, 0, &(m_tDataTangent[0].x));
	}
}

void VertexBufferObject::Enable_VBO()
{
	unsigned int slot = 0;
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBOid);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (const GLvoid*)m_nVBO_OffsetPosition);

	if(!m_tDataNormal.empty()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, (const GLvoid*)m_nVBO_OffsetNormal);
	}

	if(!m_tDataTexcoord.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, (const GLvoid*)m_nVBO_OffsetTexcoord);
	}

	if(!m_tDataTangent.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, 0, (const GLvoid*)m_nVBO_OffsetTangent);
	}
}

void VertexBufferObject::Disable_VA()
{
	unsigned int slot = 0;

	if(!m_tDataTexcoord.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(!m_tDataTangent.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(!m_tDataNormal.empty()) {
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
}

void VertexBufferObject::Disable_VBO()
{
	unsigned int slot = 0;

	if(!m_tDataTexcoord.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(!m_tDataTangent.empty()) {
		glClientActiveTexture(GL_TEXTURE0 + slot++);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(!m_tDataNormal.empty()) {
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



