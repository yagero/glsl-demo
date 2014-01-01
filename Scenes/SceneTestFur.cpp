#include "SceneTestFur.h"

#include "../Mathlib.h"
#include "../Camera.h"
#include "../Frustum.h"
#include "../Mesh.h"
#include "../Timer.h"
#include "../Sky.h"

#include "../Shader.h"
#include "../TextureCubemap.h"
#include "../Texture2D.h"
#include "../ImageTools.h"
#include "../FrameBufferObject.h"
#include "../Inputs.h"

#include "../ResourceManager.h"
#include "../VarManager.h"
#include "../Spline3D/SplineGL.h"

Texture2D* SceneTestFur::GenerateFurTexture(unsigned int w, unsigned int h, unsigned int density)
{
	Texture2D* texFur = new Texture2D();
	unsigned int d = 4;
	GLubyte* data = new GLubyte[w*h*d];
	assert(data);
	memset(data, 0, w*h*d);
/*
	for(unsigned int i=0; i<w*h*d; i+=4) {
		data[i+3] = 0;
	}*/

//	unsigned int density = 9000;
	for(unsigned int i=0; i<density; i++) {
		unsigned int x = rand()%w;
		unsigned int y = rand()%h;
		
		unsigned int idx = (y*w+x)*4;
		// RGB value
		data[idx+0] = 255;
		data[idx+1] = 255;
		data[idx+2] = 255;

		// alpha value (poil ou pas poil ?)
		data[idx+3] = 255;
	}

	Texture2D::EnableGenerateMipmaps(false);
	texFur->Load(data, w, h, d);
	Texture2D::EnableGenerateMipmaps(true);

	return texFur;
}

void SceneTestFur::Init()
{
	ResourceManager& res = ResourceManager::getInstance();
	m_vLightPos = vec4(0.0f, 15.0f, 0.0f, 1.0f);

	m_pScene = (Mesh*)res.LoadResource(ResourceManager::MESH, "scene_fur.obj");
	res.LoadResource(ResourceManager::MESH, "bunny_1500_smooth.obj");
	res.LoadResource(ResourceManager::MESH, "cube.obj");
	m_pShaderFur = (Shader*)res.LoadResource(ResourceManager::SHADER, "fur");
	m_pShdLighting = (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "tiger.jpg");

	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "stones_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_diffuse.jpg");

	Texture2D::EnableGenerateMipmaps(false);
	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "stones_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_NM_height.tga");
	Texture2D::EnableGenerateMipmaps(true);


	Texture2D* texFur = GenerateFurTexture(256, 256, 9000);
	res.NewResource(texFur, "texFur");

	LoadCameraTrajFromFile("fur.txt");

	m_pSplineLightPosition = new SplineGL();
	m_pSplineLightPosition->AddPoint( vec3(-5.0,  1.0,  7.0) );
	m_pSplineLightPosition->AddPoint( vec3( 7.0,  1.0,  7.0) );
	m_pSplineLightPosition->AddPoint( vec3( 7.0,  1.0, -7.0) );
	m_pSplineLightPosition->AddPoint( vec3(-7.0,  1.0, -7.0) );
	m_pSplineLightPosition->AddPoint( vec3(-18.5, 3.0, -7.0) );
	m_pSplineLightPosition->AddPoint( vec3(-18.5,13.0,  0.0) );
	m_pSplineLightPosition->AddPoint( vec3(-18.5, 3.0,  7.0) );
	m_pSplineLightPosition->BuildSplines(true);
	m_pSplineLightPosition->setSpeed(0.06f);
}

void SceneTestFur::Destroy()
{
	if(m_pSplineLightPosition)
		delete m_pSplineLightPosition;
	SceneBase::Destroy();
}

void SceneTestFur::Idle(float fElapsedTime)
{
	m_fObjectRotation += 10.0f*fElapsedTime;

	m_pSplineLightPosition->Idle(fElapsedTime);
	m_vLightPos = m_pSplineLightPosition->getPoint();

	float lighttime = m_pSplineLightPosition->getTime() * 4.0f;

	vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
	vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
//	vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
	vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
	vec4 purple(1.0f, 0.0f, 1.0f, 1.0f);

	if(lighttime < 1.0f)		m_vLightColor = m_vLightColor.lerp(yellow, red, lighttime-0.0f);
	else if(lighttime < 2.0f)	m_vLightColor = m_vLightColor.lerp(red, green, lighttime-1.0f);
	else if(lighttime < 3.0f)	m_vLightColor = m_vLightColor.lerp(green, purple, lighttime-2.0f);
	else						m_vLightColor = m_vLightColor.lerp(purple, yellow, lighttime-3.0f);
}

void SceneTestFur::Reset()
{
	VarManager& var = VarManager::getInstance();

	Camera::getInstance().setEye(vec3(0.0f, 0.0f, 0.0f));

	m_fObjectRotation = 0.0f;
	m_pSplineLightPosition->setTime(0.0f);

	SceneBase::Reset();
}


void SceneTestFur::Render()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();


	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);


	vec4 zeros(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);

//	m_pSplineLightPosition->DrawGL();

//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, m_vLightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_vLightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f/512);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f/512);

	glColor3f(m_vLightColor.r, m_vLightColor.g, m_vLightColor.b);
	glPushMatrix();
	glTranslatef(m_vLightPos.x, m_vLightPos.y, m_vLightPos.z);
	glutSolidSphere(0.1, 16, 16);
	glPopMatrix();



	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/7);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);


	res.getTexture2D("rocks_diffuse.jpg")->Bind(0);
	res.getTexture2D("rocks_NM_height.tga")->Bind(1);
	m_pShdLighting->Activate();
	{
		// Sol et plafond
		m_pShdLighting->Uniform("mode", 3);
		m_pShdLighting->Uniform("relief_factor", 0.07f);
		m_pScene->Draw(0);
		m_pScene->Draw(1);

		// Murs
		res.getTexture2D("stones_diffuse.jpg")->Bind(0);
		res.getTexture2D("stones_NM_height.tga")->Bind(1);
		m_pShdLighting->Uniform("relief_factor", 0.05f);

		m_pScene->Draw(3);

		// Cube
		res.getTexture2D("four_diffuse.jpg")->Bind(0);
		res.getTexture2D("four_NM_height.tga")->Bind(1);
		m_pShdLighting->Uniform("relief_factor", 0.1f);

		glPushMatrix();
		{
			glTranslatef(-6.0f, 0.5f, 0.0f);
			glRotatef(-m_fObjectRotation, 0.0f, 1.0f, 0.0f);
			res.getMesh("cube.obj")->Draw();
		}
		glPopMatrix();
	}
	m_pShdLighting->Deactivate();
	res.getTexture2D("rocks_NM_height.tga")->Unbind(1);
	res.getTexture2D("rocks_diffuse.jpg")->Unbind(0);
	
/*
	// DRAW LIGHT SOURCE
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	m_pScene->Draw(2);
	glEnable(GL_TEXTURE_2D);
*/

	
	glPushMatrix();

	glTranslatef(6.0f, 2.5f, 0.0f);
	glRotatef(m_fObjectRotation, 0.0f, 1.0f, 0.0f);
	glScalef(50.f, 50.f, 50.f);

	res.getTexture2D("tiger.jpg")->Bind(0);
	
	m_pShdLighting->Activate();
	{
		m_pShdLighting->Uniform("mode", 0);
		m_pShdLighting->Uniform("tile_factor", 1.0f);
		m_pShdLighting->Uniform("enable_shadow_mapping", 0);
		m_pShdLighting->UniformTexture("texDiffuse", 0);
		res.getMesh("bunny_1500_smooth.obj")->Draw();
	}
	m_pShdLighting->Deactivate();




	// Draw fur 
	glEnable(GL_BLEND);
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf( Frustum::getInstance().getModelviewMatrix() );
	glMatrixMode(GL_MODELVIEW);

	res.getTexture2D("tiger.jpg")->Bind(0);
	res.getTexture2D("texFur")->Bind(1);
	m_pShaderFur->Activate();
	{
		int layer_nb = 25;

		m_pShaderFur->Uniform("time", Timer::getInstance().getCurrentTime());
		m_pShaderFur->Uniform("tile_factor", 1.0f);
		m_pShaderFur->Uniform("fur_length", 0.5f);
		m_pShaderFur->Uniform("fur_weakness", 0.3f);

		m_pShaderFur->UniformTexture("texDiffuse", 0);
		m_pShaderFur->UniformTexture("texFur", 1);

		for(int i=0; i<layer_nb; i++) {
			m_pShaderFur->Uniform("current_layer", ((float)i+1)/layer_nb);
			res.getMesh("bunny_1500_smooth.obj")->Draw();
		}
	}
	m_pShaderFur->Deactivate();

	res.getTexture2D("tiger.jpg")->Unbind(0);
	res.getTexture2D("texFur")->Unbind(1);
	glDisable(GL_BLEND);

	glPopMatrix();


	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}



