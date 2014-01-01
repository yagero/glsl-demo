#ifndef SHADER_H
#define SHADER_H


#include <iostream>
#include <GL/glew.h>
#include "Mathlib.h"
#include "ResourceBase.h"

// -------------------------------
// Shader (vertex & pixel)
// -------------------------------

class Shader : public ResourceBase
{
public:
	static bool Init();
	virtual bool Load(const std::string& name);
	virtual void Destroy();

	virtual bool DoesSupportReloading()	const {return true;}

	bool Activate();
	static void Deactivate();

	static Shader* getActivatedShader()	{return s_pCurrent;}
/*
	static bool isExtensionSupported(const std::string& ext);
	static void PrintSupportedExtensions();*/

	// Communication du programme au shader :
	void Uniform(const std::string& ext, GLint value);
	void Uniform(const std::string& ext, GLfloat value);
	void Uniform(const std::string& ext, const vec2& value);
	void Uniform(const std::string& ext, const vec3& value);
	void UniformTexture(const std::string& ext, GLint slot);

//	GLhandleARB	getHandle() {return m_nProgram;}

	Shader() {m_nProgram=0; m_strName="undef";}

private:
	static GLhandleARB loadShader(const std::string& filestr);
	static bool compileShader(GLhandleARB object);
	static GLhandleARB linkShaders(GLhandleARB* object, const unsigned int& nb);
	

private:
	static bool		s_bInitialized;
	static Shader*	s_pCurrent;
	std::string		m_strName;
	GLhandleARB		m_nProgram;

};

#endif

