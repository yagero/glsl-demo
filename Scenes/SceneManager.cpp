
#include "SceneManager.h"
#include "SceneBase.h"
#include "SceneTestParallax.h"
#include "SceneTestRefraction.h"
#include "SceneTestFur.h"
#include "SceneTerrain.h"
#include "SceneSimple.h"
#include "SceneTestShadowMapping.h"
#include "SceneToon.h"
#include "../Camera.h"
#include "../VarManager.h"
#include <assert.h>
#include "Script.h"

SceneManager::SceneManager()
{
//	m_SceneDB["test_parallax"] = new SceneTestParallax();
	m_SceneDB["test_refraction"] = new SceneTestRefraction();
	m_SceneDB["terrain"] = new SceneTerrain();
	m_SceneDB["test_fur"] = new SceneTestFur();
//	m_SceneDB["simple"] = new SceneSimple();
	m_SceneDB["test_shadowmapping"] = new SceneTestShadowMapping();
	m_SceneDB["toon"] = new SceneToon();

	setCurrent("terrain");
}

bool SceneManager::setCurrent(const std::string& name)
{
	
	std::map<std::string, SceneBase*>::iterator it = m_SceneDB.find(name);
	if(it == m_SceneDB.end())
		return false;

	if(m_pCurrentScene != it->second)
	{
		m_pCurrentScene = it->second;
		assert(m_pCurrentScene != NULL);
		m_pCurrentScene->Reset();
	}
	return true;
}

SceneBase* SceneManager::getScenePointer(const std::string& name)
{
	std::map<std::string, SceneBase*>::iterator it = m_SceneDB.find(name);
	if(it == m_SceneDB.end())
		return NULL;
	return it->second;
}

void SceneManager::Init()
{
	for(std::map<std::string, SceneBase*>::iterator it = m_SceneDB.begin(); it != m_SceneDB.end(); it++)
		(*it).second->Init();
}

void SceneManager::Destroy()
{
	for(std::map<std::string, SceneBase*>::iterator it = m_SceneDB.begin(); it != m_SceneDB.end(); it++) {
		(*it).second->Destroy();
		delete (*it).second;
		(*it).second = NULL;
	}
}

void SceneManager::Idle(float fElapsedTime)
{
	

	if(Camera::getInstance().getType() == Camera::DRIVEN)
	{
		ScriptIdle(fElapsedTime);
		m_pCurrentScene->InterpolCameraTraj(fElapsedTime);
	}

	m_pCurrentScene->Idle(fElapsedTime);

}

void SceneManager::Keyboard(bool special, unsigned char key)
{
	m_pCurrentScene->Keyboard(special, key);
}

void SceneManager::PreRender()
{
	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
	m_pCurrentScene->PreRender();
	glPopAttrib();
}



void SceneManager::Render()
{
	VarManager& var = VarManager::getInstance();

	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
	m_pCurrentScene->Render();

	if(var.getb("show_camera_splines"))
		m_pCurrentScene->DrawTraj();

	glPopAttrib();
}



