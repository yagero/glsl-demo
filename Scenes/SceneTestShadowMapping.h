#ifndef SCENETESTSHADOWMAPPING_H
#define SCENETESTSHADOWMAPPING_H

#include "SceneBase.h"
#include "../FrameBufferObject.h"

class TextureCubemap;
class Shader;
class SplineGL;

class SceneTestShadowMapping : public SceneBase
{
public:
	virtual void Init();
	virtual void Destroy();
	virtual void PreRender();
	virtual void Render();
	virtual void Idle(float fElapsedTime);
	virtual void Reset();
	
private:
	// Rendu de la scène (pour la depth map ou non)
	void RenderScene(bool bDepthMap);

	// On place les matrices et la caméra au point de vue de la lumière pour le pré-rendu
	void SetLightCameraMatrices();
	void RestoreLightCameraMatrices();

private:
	vec4				m_vLightPos;
	vec3				m_vLightLookAt;
	mat4				m_matLightModelviewProj;
	mat4				m_matLightModelviewInv;
	TextureCubemap*		m_pSkybox;
	Shader*				m_pShader;
	FrameBufferObject	m_fboDepthMapFromLight;
	FrameBufferObject	m_fboTextureProjected;

	SceneBase*			m_pSubScene;

	SplineGL*			m_pSplineLightLookAt;
	SplineGL*			m_pSplineLightPosition;

	float				m_fObjectRotation;
};

#endif

