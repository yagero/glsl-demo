#include "SceneManager.h"
#include "SceneBase.h"
#include "SceneTestParallax.h"
#include "SceneTestRefraction.h"
#include "SceneTestFur.h"
#include "SceneTerrain.h"
#include "SceneSimple.h"
#include "SceneTestShadowMapping.h"
#include "SceneToon.h"
#include "../VarManager.h"
#include <assert.h>
#include "../Spline3D/SplineGL.h"

static int current = -1;

void ScriptIdle(float fElapsedTime)
{
	SceneManager& manag = SceneManager::getInstance();
	SceneBase* pScene = manag.getCurrentScenePointer();
	assert(pScene);

	static std::string tSceneName[8] = {"terrain", "test_shadowmapping", "terrain", "test_refraction", "terrain", "toon", "terrain", "test_fur"};

	switch(current) {
	case -1: {
		manag.setCurrent(tSceneName[0]);
		SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
		pTerrain->LoadCameraTrajFromFile("terrain_1.txt");
		pTerrain->setSunAngle(vec2(0.0f, RADIANS(45.0f)));
		current = 0;
		break;}
	case 0: {
		manag.setCurrent(tSceneName[0]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[1]);
			current = 1;
		}
		break;}
	case 1: {
		manag.setCurrent(tSceneName[1]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[2]);
			SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
			pTerrain->LoadCameraTrajFromFile("terrain_2.txt");
			pTerrain->setSunAngle(vec2(0.0f, RADIANS(-30.0f)));
			current = 2;
		}
		break;}

	case 2: {
		manag.setCurrent(tSceneName[2]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[3]);
			current = 3;
		}
		break;}

	case 3: {
		manag.setCurrent(tSceneName[3]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[4]);
			SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
			pTerrain->LoadCameraTrajFromFile("terrain_3.txt");
			pTerrain->setSunAngle(vec2(RADIANS(-5.0f), RADIANS(71.0f)));
			current = 4;
		}
		break;}

	case 4: {
		manag.setCurrent(tSceneName[4]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[5]);
			current = 5;
		}
		break;}

	case 5: {
		manag.setCurrent(tSceneName[5]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[6]);
			SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
			pTerrain->LoadCameraTrajFromFile("terrain_4.txt");
			pTerrain->setSunAngle(vec2(RADIANS(50.0f), RADIANS(-65.0f)));
			current = 6;
		}
		break;}

	case 6: {
		manag.setCurrent(tSceneName[6]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[7]);
			current = 7;
		}
		break;}

	case 7: {
		manag.setCurrent(tSceneName[7]);
		if(pScene->getCamEyeSpline()->isFinished()) {
			manag.setCurrent(tSceneName[0]);
			SceneTerrain* pTerrain = (SceneTerrain*)manag.getCurrentScenePointer();
			pTerrain->LoadCameraTrajFromFile("terrain_1.txt");
			pTerrain->setSunAngle(vec2(0.0f, RADIANS(45.0f)));
			current = 0;
		}
		break;}

	default:
		assert(0);
	}
}


