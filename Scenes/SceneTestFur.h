#ifndef SCENETESTFUR_H
#define SCENETESTFUR_H

#include "SceneBase.h"

class Texture2D;
class Shader;
class Mesh;
class SplineGL;

class SceneTestFur : public SceneBase
{
public:
	virtual void Init();
	virtual void Destroy();
	virtual void Render();
	virtual void Idle(float fElapsedTime);
	virtual void Reset();
	
private:
	Texture2D* GenerateFurTexture(unsigned int w, unsigned int h, unsigned int density);

private:
	vec4				m_vLightPos;
	Shader*				m_pShaderFur;
	Shader*				m_pShdLighting;

	Mesh*				m_pScene;

	SplineGL*			m_pSplineLightPosition;

	float				m_fObjectRotation;

	vec4				m_vLightColor;
};

#endif

