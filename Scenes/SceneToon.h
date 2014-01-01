#ifndef SCENETOON_H
#define SCENETOON_H

#include "SceneBase.h"

class Shader;
class Texture2D;
class Mesh;


class SceneToon : public SceneBase
{
public:
	// Initialisation des données propres à la scène
	virtual void Init();

	// Destruction des données propres à la scène
	virtual void Destroy();

	// Evolution des données et traitement des commandes synchones
	virtual void Idle(float fElapsedTime);

	// Démarrage de la scène
	virtual void Reset();

	// Affichage de la scène
	virtual void Render();


private:
	// Ici on met les données propres à la scène
	vec4		m_vLightPos;
	Texture2D*	m_pMyTexGround;
	Texture2D*	m_pMyTexWall;
	Texture2D*	m_pMyTexTop;
	float		m_fAngle;
	Mesh*		m_objHomer;
	Mesh*		m_teapot;
	Shader*		m_shadeToon;
	Mesh*		m_pScene;
};

#endif

