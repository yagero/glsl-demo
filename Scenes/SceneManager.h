#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "../Singleton.h"
#include <iostream>
#include <map>
#include <hash_map>
#include <string>

class SceneBase;

SINGLETON_BEGIN( SceneManager )
public:
	SceneManager();

	void Init();
	void Destroy();
	void Idle(float fElapsedTime);
	void Keyboard(bool special, unsigned char key);
	void PreRender();
	void Render();

	bool setCurrent(const std::string& name);
	SceneBase* getScenePointer(const std::string& name);
	inline SceneBase* getCurrentScenePointer()	const {return m_pCurrentScene;}
	
private:
	std::map<std::string, SceneBase*>	m_SceneDB;			// Toutes les scènes
	SceneBase*							m_pCurrentScene;	// Pointeur sur la scène courante

SINGLETON_END()

#endif

