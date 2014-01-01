#ifndef TEXTURE2D_H
#define TEXTURE2D_H


#include "Texture.h"

class FrameBufferObject;

// -------------------------------
// Textures 2D
// -------------------------------

class Texture2D : public Texture
{
public:
	virtual GLenum getTextureType() const {return GL_TEXTURE_2D;}
	bool Load(const std::string& name);
	bool Load(GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d);

	Texture2D() : Texture() {}

protected:
	

protected:


};

#endif

