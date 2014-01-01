#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include "FrameBufferObject.h"
#include "glInfo.h"
#include <assert.h>

FrameBufferObject::FrameBufferObject()
{
	m_nFrameBufferHandle=0;
	m_nDepthBufferHandle=0;
	m_nTexId = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bUseFBO = true;
}

void FrameBufferObject::Destroy()
{
	glDeleteTextures(1, &m_nTexId);
	glDeleteFramebuffersEXT(1, &m_nFrameBufferHandle);
	if(m_bUseDepthBuffer)
		glDeleteRenderbuffersEXT(1, &m_nDepthBufferHandle);

	m_nTexId = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bUseFBO = true;
}


void FrameBufferObject::Begin(GLuint nFace) const
{
	assert(nFace<6);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_nWidth, m_nHeight);

	if(m_bUseFBO) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_nFrameBufferHandle);

		if(m_eTextureType == GL_TEXTURE_CUBE_MAP) {
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, m_eAttachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X+nFace, m_nTexId, 0);
		}
	}
	else {
        glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT); // for GL_DRAW_BUFFER and GL_READ_BUFFER
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
	}
}

void FrameBufferObject::End(GLuint nFace) const
{
	assert(nFace<6);
	if(m_bUseFBO) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	else {
        // copy the framebuffer pixels to a texture
        glBindTexture(m_eTextureType, m_nTexId);
        glCopyTexSubImage2D(m_eTextureType, 0, 0, 0, 0, 0, m_nWidth, m_nHeight);
        glBindTexture(m_eTextureType, 0);

        glPopAttrib();	// GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
	}
	glPopAttrib();		// GL_VIEWPORT_BIT
}

void FrameBufferObject::Bind(GLint unit) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB + unit);
	glEnable(m_eTextureType);
	glBindTexture(m_eTextureType, m_nTexId);
}

void FrameBufferObject::Unbind(GLint unit) const
{
	glActiveTextureARB(GL_TEXTURE0_ARB + unit);
	glBindTexture( m_eTextureType, 0 );
	glDisable(m_eTextureType);
}


bool FrameBufferObject::Create(FBO_TYPE type, GLuint width, GLuint height)
{
	Destroy();


	m_nWidth = width;
	m_nHeight = height;
	m_bUseFBO = glInfo::getInstance().isExtensionSupported("GL_EXT_framebuffer_object");
	m_bUseDepthBuffer = true;
	m_eTextureType = type==FBO_CUBE_COLOR? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;


	// Texture
	glGenTextures( 1, &m_nTexId );
	glBindTexture(m_eTextureType, m_nTexId);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	if(type==FBO_2D_DEPTH)
//		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	GLenum eTarget;
	GLuint nFrames;
	if(type!=FBO_CUBE_COLOR) {
		eTarget = GL_TEXTURE_2D;
		nFrames = 1;
	}
	else {
		eTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		nFrames = 6;
	}

	for(GLuint i=0; i<nFrames; i++)
	{
		if(type==FBO_2D_DEPTH)
			glTexImage2D(eTarget+i, 0, GL_DEPTH_COMPONENT24, m_nWidth, m_nHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		else
			glTexImage2D(eTarget+i, 0, GL_RGB, m_nWidth, m_nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}



	if(m_bUseFBO)
	{
		// Frame buffer
		glGenFramebuffersEXT(1, &m_nFrameBufferHandle);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_nFrameBufferHandle);

		if(m_bUseDepthBuffer /*&& format!=GL_DEPTH_COMPONENT*/)
		{
			// Depth buffer
			glGenRenderbuffersEXT(1, &m_nDepthBufferHandle);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_nDepthBufferHandle);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_nWidth, m_nHeight);

			// attache le frame buffer avec le depth buffer
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_nDepthBufferHandle);
		}


		//  attach the framebuffer to our texture, which may be a depth texture
		if(type==FBO_2D_DEPTH) {
			m_eAttachment = GL_DEPTH_ATTACHMENT_EXT;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, eTarget, m_nTexId, 0);
			//  disable drawing to any buffers, we only want the depth
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		} else {
			m_eAttachment = GL_COLOR_ATTACHMENT0_EXT;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, eTarget, m_nTexId, 0);
		}

		m_bUseFBO = CheckStatus();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	return true;
}





bool FrameBufferObject::CheckStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
//        std::cerr << "Framebuffer complete." << std::endl;
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        std::cerr << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        std::cerr << "[ERROR] Unsupported by FBO implementation." << std::endl;
        return false;

    default:
        std::cerr << "[ERROR] Unknow error." << std::endl;
        return false;
    }
}




