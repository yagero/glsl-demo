#ifndef FBO_H
#define FBO_H

#include <GL/gl.h>
#include <iostream>

// -------------------------------
// Rendu dans une texture
// avec où sans FBO
// -------------------------------
class FrameBufferObject
{
public:
	enum FBO_TYPE { FBO_2D_COLOR, FBO_CUBE_COLOR, FBO_2D_DEPTH };

	FrameBufferObject();
	~FrameBufferObject() {Destroy();}

	bool Create(FBO_TYPE type, GLuint width, GLuint height);
	void Destroy();

	void Begin(GLuint nFace=0) const;	// Commence l'utilisation du rendu dans une texture
	void End(GLuint nFace=0) const;		// Termine l'utilisation du rendu dans une texture

	void Bind(GLint unit=0) const;		// Binde la texture
	void Unbind(GLint unit=0) const;	// Débinde la texture

	inline GLuint getTextureHandle() const	{return m_nTexId;} 
	inline GLuint getWidth() const			{return m_nWidth;}
	inline GLuint getHeight() const			{return m_nHeight;}

private:
	bool CheckStatus();

private:
	bool		m_bUseFBO;
	bool		m_bUseDepthBuffer;
	GLuint		m_nTexId;
	GLuint		m_nWidth, m_nHeight;
	GLuint		m_nFrameBufferHandle;
	GLuint		m_nDepthBufferHandle;
	GLenum		m_eTextureType;
	GLenum		m_eAttachment;
	
};


#endif

