#ifndef SCENETESTPARALLAX_H
#define SCENETESTPARALLAX_H

#include "SceneBase.h"

class TextureCubemap;
class Shader;

class SceneTestParallax : public SceneBase
{
public:
	virtual void Init();
	virtual void Render();
	virtual void Idle(float fElapsedTime);
	virtual void Reset();
	
protected:

private:
	vec4				m_vLightPos;
	TextureCubemap*		m_pSkybox;
	Shader*				m_pShader;
};

#endif

