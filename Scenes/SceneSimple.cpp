#include "SceneSimple.h"

#include "../Mathlib.h"
#include "../Camera.h"

#include "../Shader.h"
#include "../Texture2D.h"

#include "../ResourceManager.h"
#include "../VarManager.h"
#include "../Spline3D/SplineGL.h"

// Initialisation des données propres à la scène
void SceneSimple::Init()
{
	// On accède au Resource Manager pour charger des ressources :
	ResourceManager& res = ResourceManager::getInstance();

	// Ici on charge une texture, et on récupère un pointeur sur la donnée pour y accéder plus facilement
	m_pMyTex = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");

//	res.LoadResource(ResourceManager::MESH, "simpsons_table.obj");

	m_fAngle = 0.0f;


	m_pCamEyeTraj->AddPoint(vec3(0.0f, 0.0f, 0.0f));
	m_pCamEyeTraj->AddPoint(vec3(5.0f, 5.0f, 0.0f));
	m_pCamEyeTraj->AddPoint(vec3(5.0f, 0.0f, 5.0f));
//	m_pCamEyeTraj->Close();
	m_pCamEyeTraj->BuildSplines(true);

	m_pCamLookAtTraj->AddPoint(vec3(0.0f, 0.0f, 0.0f));
	m_pCamLookAtTraj->AddPoint(vec3(-5.0f, -5.0f, 0.0f));
	m_pCamLookAtTraj->AddPoint(vec3(-5.0f, 0.0f, -5.0f));
//	m_pCamLookAtTraj->Close();
	m_pCamLookAtTraj->BuildSplines(true);
}

// Destruction des données propres à la scène
void SceneSimple::Destroy()
{
	SceneBase::Destroy();
	// Les ressources chargées avec le Resource Manager sont libérées autmatiquement,
	// donc il n'y a rien à faire ici dans ce cas
}

// Evolution des données et traitement des commandes synchones
void SceneSimple::Idle(float fElapsedTime)
{
	// On prend en compte le temps écoulé depuis la frame précédente
	// pour faire évoluer nos données en fonction du temps
	m_fAngle += 10.0f*fElapsedTime;
}

// Démarrage de la scène
void SceneSimple::Reset()
{
	// On peut par exemple remettre la caméra à une position donnée :
	Camera& cam = Camera::getInstance();
	cam.setEye(vec3(10.0f, 0.0f, 10.0f));
	cam.setAngle(RADIANS(225.f), RADIANS(90.f));

}

// Affichage de la scène
void SceneSimple::Render()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();

	DrawAxes();

	glColor3f(1.0f, 0.0f, 0.0f);
//	DrawTraj();

	glPushMatrix();
//	glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);


	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	vec4 pos(0.0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	// Ici on va activer notre texture
	// On pourrait accéder à notre texture via :
	// Texture2D* pTex = res.getTexture2D("rocks_diffuse.jpg");
	// et cela éviterait d'avoir une variable de classe pour accéder à la ressource

	// On bind la texture en slot 0
	m_pMyTex->Bind(0);

	// On dessine un objet
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidTeapot(5.0);
//	res.getMesh("simpsons_table.obj")->Draw();
	// la teapot est dessinée dans le mauvais sens, mais c'est un bug de glut,
	// les autres primitives sont, elles, dessinées dans le bon sens.

	// On la débind en slot 0 aussi
	m_pMyTex->Unbind(0);

	glPopMatrix();
}







