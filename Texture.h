#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <iostream>
#include "ResourceBase.h"

// -------------------------------
// Abstract Texture
// -------------------------------

class Texture : public ResourceBase
{
public:
	virtual GLenum getTextureType() const = 0;

	void Gen();
	virtual bool Load(const std::string& name);
	virtual void Destroy();

	void Bind(GLuint slot) const;
	void Unbind(GLuint slot) const;

	GLuint getHandle() const {return m_nHandle;} 

	static void EnableGenerateMipmaps(bool b) {s_bGenerateMipmaps=b;}

	Texture() {m_nHandle=0;}
	~Texture() {Destroy();}

protected:
	void Bind() const;
	void Unbind() const;
	bool LoadFile(GLenum target, const std::string& name);
	void LoadData(GLenum target, GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d);
	
protected:
	GLuint	m_nHandle;				// texture opengl id
	static bool s_bGenerateMipmaps;	// generate mipmap or not
};

#endif

