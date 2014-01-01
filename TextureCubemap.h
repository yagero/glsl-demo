#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include <GL/glew.h>
#include "Texture.h"

// -------------------------------
// Textures Cubemap (6 images 2D)
// -------------------------------

class TextureCubemap : public Texture
{
public:
	virtual GLenum getTextureType() const {return GL_TEXTURE_CUBE_MAP;}
	bool Load(const std::string& name);

	TextureCubemap() : Texture() {}

protected:
	

protected:


};

#endif

