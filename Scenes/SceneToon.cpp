#include "SceneToon.h"

#include "../Mathlib.h"
#include "../Camera.h"
#include "../Mesh.h"
#include "../Inputs.h"
#include "../Shader.h"
#include "../Texture2D.h"

#include "../ResourceManager.h"
#include "../VarManager.h"
#include "../Spline3D/SplineGL.h"

// Initialisation des données propres à la scène
void SceneToon::Init()
{
	// On accède au Resource Manager pour charger des ressources :
	ResourceManager& res = ResourceManager::getInstance();

	// Ici on charge une texture, et on récupère un pointeur sur la donnée pour y accéder plus facilement
	m_pMyTexGround = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "dirtground008.jpg");
	m_pMyTexWall = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "brickwall004.jpg");
	m_pMyTexTop = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "floor_tile_006a_reflect.jpg");
	m_pScene = (Mesh*)res.LoadResource(ResourceManager::MESH, "scene_fur.obj");
	m_objHomer = (Mesh*)res.LoadResource(ResourceManager::MESH, "bunny_1500_smooth.obj");
	m_teapot = (Mesh*)res.LoadResource(ResourceManager::MESH, "teapot.obj");
	m_shadeToon = (Shader*)res.LoadResource(ResourceManager::SHADER, "toon");
	m_fAngle = 0.0f;

	m_vLightPos = vec4(0.0f, 5.0f, 0.0f, 1.0f);

	LoadCameraTrajFromFile("toon.txt");
}

// Destruction des données propres à la scène
void SceneToon::Destroy()
{
	SceneBase::Destroy();
	// Les ressources chargées avec le Resource Manager sont libérées autmatiquement,
	// donc il n'y a rien à faire ici dans ce cas
}

// Evolution des données et traitement des commandes synchones
void SceneToon::Idle(float fElapsedTime)
{
	Inputs& inputs = Inputs::getInstance();

	if( inputs.Key('z') )			m_vLightPos.z += 0.1f;
	if( inputs.Key('s') )			m_vLightPos.z -= 0.1f;
	if( inputs.Key('q') )			m_vLightPos.x += 0.1f;
	if( inputs.Key('d') )			m_vLightPos.x -= 0.1f;
	if( inputs.Key('r') )			m_vLightPos.y += 0.1f;
	if( inputs.Key('f') )			m_vLightPos.y -= 0.1f;

	// On prend en compte le temps écoulé depuis la frame précédente
	// pour faire évoluer nos données en fonction du temps
	m_fAngle += 10.0f*fElapsedTime;
}

// Démarrage de la scène
void SceneToon::Reset()
{
	VarManager& var = VarManager::getInstance();

	// On peut par exemple remettre la caméra à une position donnée :
	Camera::getInstance().setEye(vec3(0.0f, 0.0f, 0.0f));


	m_fAngle = 0.0f;

	SceneBase::Reset();
}

// Affichage de la scène
void SceneToon::Render()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();

	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);

		vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
		vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
		vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
		vec4 zeros(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 marron(0.95, 0.6, 0.25,1.0);
		vec4 jaune(1.0, 0.8, 0.0,1.0);

		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
			glDisable(GL_LIGHTING);
			glTranslatef(m_vLightPos.x, m_vLightPos.y, m_vLightPos.z);
			glutSolidSphere(0.1, 16, 16);
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, m_vLightPos);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
		glLightfv(GL_LIGHT0, GL_AMBIENT, &white.r);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f/512);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f/512);


	//	glDisable(GL_CULL_FACE);
		
	

		m_shadeToon->Activate();
		{
			m_shadeToon->UniformTexture("myText", 0);
			m_shadeToon->Uniform("enable_texture", true);


			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/4);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white/2);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zeros);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
			
			// Le sol
			m_pMyTexGround->Bind(0);
			m_pScene->Draw(0);
			m_pMyTexGround->Unbind(0);	

			//Les murs
			m_pMyTexWall->Bind(0);
			m_pScene->Draw(3);
			m_pMyTexWall->Unbind(0);	

			// Le plafond
			m_pMyTexTop->Bind(0);
			m_pScene->Draw(1);
			m_pMyTexTop->Unbind(0);

			m_shadeToon->Uniform("enable_texture", false);
			
			glPushMatrix();
			{
				glTranslatef(0.0,4.0,0.0);
				// TORE
				glPushMatrix();
				{
					glRotatef(m_fAngle, 1.0f, 1.0f, 0.0f);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red/4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red/2);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
					glutSolidTorus(1.0,3.0,100,100);
				}
				glPopMatrix();

				// TEAPOT
				glPushMatrix();
				{
					glTranslatef(0.0,0.0,-12.0);
					glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, marron/4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, marron/2);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
					m_teapot->Draw();
				}
				glPopMatrix();

				// LAPIN
				glPushMatrix();
				{
					glTranslatef(0.0,2.0,9.0);
					glScalef(25.0,25.0,25.0);
					glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, jaune/4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, jaune/2);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);				
					m_objHomer->Draw();
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		m_shadeToon->Deactivate();


	// Affichage des contours

	glDisable(GL_LIGHTING);
	glLineWidth(5.0);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	glColor3f(0.0,0.0,0.0);


			glPushMatrix();
			{
				glTranslatef(0.0,4.0,0.0);
				// TORE
				glPushMatrix();
				{
					glRotatef(m_fAngle, 1.0f, 1.0f, 0.0f);
					glutSolidTorus(1.0,3.0,100,100);
				}
				glPopMatrix();

				// TEAPOT
				glPushMatrix();
				{
					glTranslatef(0.0,0.0,-12.0);
					glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);
					m_teapot->Draw();
				}
				glPopMatrix();

				// LAPIN
				glPushMatrix();
				{
					glTranslatef(0.0,2.0,9.0);
					glScalef(25.0,25.0,25.0);
					glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);			
					m_objHomer->Draw();
				}
				glPopMatrix();
			}
			glPopMatrix();


	glPopAttrib();

}







