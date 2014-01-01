#ifndef SCENEBASE_H
#define SCENEBASE_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

#include "../Mathlib.h"

class SplineGL;

// Classe de base abstraite des scènes
class SceneBase
{
public:
	// Initialisation des données propres à la scène
	virtual void Init() {}

	// Rechargement des shaders
	virtual void Reload() {}

	// Destruction des données propres à la scène
	virtual void Destroy();

	// Evolution des données et traitement des commandes synchones
	virtual void Idle(float fElapsedTime) {}

	// Traitement des commandes asynchones
	virtual void Keyboard(bool special, unsigned char key);

	// Démarrage de la scène
	virtual void Reset();

	// Pré-affichage de la scène (peut utiliser des FBO)
	virtual void PreRender() {}

	// Affichage de la scène (ne peut pas utiliser des FBO)
	virtual void Render() = 0;

	// On met à jour la position dans les splines de caméra
	void InterpolCameraTraj(float fElapsedTime);

	// Affiche la trajectoire de la caméra
	void DrawTraj();	

	SceneBase();
	virtual ~SceneBase()  {Destroy();}

private:
	SceneBase(const SceneBase& sc) {}
	SceneBase &operator=(const SceneBase& sc) {return (*this);}

public:
	// Affiche un répère
	static void DrawAxes();

	// Affiche une chaine de caractère à l'écran
	static void DrawString(vec2 pos, void* font, const std::string& str);

	// Charge la trajectoire de la caméra depuis un fichier
	bool LoadCameraTrajFromFile(const std::string& name);

	// Sauve la trajectoire de la caméra dans un fichier
	bool SaveCameraTrajInFile(const std::string& name);

	inline SplineGL*	getCamEyeSpline()	const	{return m_pCamEyeTraj;}

protected:
	SplineGL*		m_pCamEyeTraj;
	SplineGL*		m_pCamLookAtTraj;
};

#endif


