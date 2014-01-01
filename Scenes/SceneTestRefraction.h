#ifndef SCENETESTREFRACTION_H
#define SCENETESTREFRACTION_H

#include "SceneBase.h"
#include "../FrameBufferObject.h"

class Shader;
class TextureCubemap;
class Mesh;

class SceneTestRefraction : public SceneBase
{
public:
	virtual void Init();
	virtual void PreRender();
	virtual void Render();
	virtual void Idle(float fElapsedTime);
	virtual void Reset();
	
private:
	void RenderScene(int iCubeMap);

private:
	TextureCubemap*		m_pSkybox;
	Shader*				m_pShdRefraction;
	Shader*				m_pShdLighting;
	Mesh*				m_pScene;
	FrameBufferObject	m_fboCubemap[3];

	float				m_fObjectRotation;
};

#endif

