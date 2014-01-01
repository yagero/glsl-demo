#ifndef SCENESIMPLE_H
#define SCENESIMPLE_H

#include "SceneBase.h"

class Texture2D;

class SceneSimple : public SceneBase
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
	Texture2D*	m_pMyTex;
	float		m_fAngle;
};

#endif

