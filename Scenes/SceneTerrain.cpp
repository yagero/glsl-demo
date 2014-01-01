#include <sstream>

#include "SceneTerrain.h"

#include "../Mathlib.h"
#include "../Camera.h"
#include "../Mesh.h"
#include "../Timer.h"
#include "../Sky.h"
#include "../Frustum.h"

#include "../Shader.h"
#include "../TextureCubemap.h"
#include "../Texture2D.h"
#include "../FrameBufferObject.h"
#include "../Inputs.h"

#include "../ResourceManager.h"
#include "../VarManager.h"
#include "../ImageTools.h"
#include "../Terrain/Terrain.h"
#include "../Terrain/TerrainChunk.h"

void SceneTerrain::Init()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();

	m_vSunAngle = vec2(0.0f, RADIANS(45.0f));

	m_pSkybox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "sand_up.jpg sand_dn.jpg sand_tp.jpg sand_bm.jpg sand_lt.jpg sand_rt.jpg");

	m_pShaderLighting	= (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");
	m_pShaderTerrain	= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_ground");
	m_pShaderWater		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_water");
	m_pShaderGrass		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_grass");
	m_pShaderTree		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_tree");

	res.LoadResource(ResourceManager::MESH, "terrain_house.obj");

	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");

	m_tTextures.push_back( (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_detail_NM.tga") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "sandfloor009a.jpg") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_rocky_map_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_grass_map_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_water_caustics.jpg") );
	
	
	res.LoadResource(ResourceManager::TEXTURE2D, "grass_billboards.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "palm_texture.tga");


	m_pTexWaterNoiseNM = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_water_NM.jpg");
	m_pTerrainDiffuseMap = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "test_heightmap512_2_diffusemap.jpg");

	var.set("water_height", 4.2f);

	m_pTerrain = new Terrain();
	assert(m_pTerrain != NULL);
	//BoundingBox bbox( vec3(-300.0f, 0.0f, -300.0f), vec3(300.0f, 50.0f, 300.0f) );
	BoundingBox bbox( vec3(-300.0f, 0.0f, -300.0f), vec3(300.0f, 40.0f, 300.0f) );
//	m_pTerrain->Load("test_heightmap1024_2.jpg", bbox, 32);
	m_pTerrain->Load("test_heightmap512_2.jpg", bbox, 16);

	ImageTools::ImageData img;
	ImageTools::OpenImage("test_heightmap512_2_diffusemap.jpg", img);
	m_pTerrain->GenerateGrass(img, 200000);
	m_pTerrain->GenerateVegetation(img, 100);
	m_pTerrain->ComputeBoundingBox();
	img.Destroy();

	Reload(); // set shader constants

	m_fboWaterReflection.Create(FrameBufferObject::FBO_2D_COLOR, 512, 512);
	m_fboDepthMapFromLight[0].Create(FrameBufferObject::FBO_2D_DEPTH, 2048, 2048);
	m_fboDepthMapFromLight[1].Create(FrameBufferObject::FBO_2D_DEPTH, 2048, 2048);

	LoadCameraTrajFromFile("terrain.txt");
}

void SceneTerrain::Reload()
{
	vec3 fogColor(0.7f, 0.7f, 0.9f);
	m_pShaderTerrain->Activate();
	m_pShaderTerrain->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
	m_pShaderTerrain->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
	m_pShaderTerrain->Uniform("fog_color", fogColor);
	m_pShaderTerrain->Deactivate();

	m_pShaderWater->Activate();
	m_pShaderWater->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
	m_pShaderWater->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
	m_pShaderWater->Uniform("fog_color", fogColor);
	m_pShaderWater->Deactivate();
}

void SceneTerrain::Destroy()
{
	m_pTerrain->Destroy();
	delete m_pTerrain;
	m_tTextures.clear();

	SceneBase::Destroy();
}

void SceneTerrain::Idle(float fElapsedTime)
{
	SceneBase::Idle(fElapsedTime);
	Inputs& inputs = Inputs::getInstance();

	if( inputs.Key('z') )			m_vSunAngle.y += 0.01f;
	if( inputs.Key('s') )			m_vSunAngle.y -= 0.01f;
	if( inputs.Key('q') )			m_vSunAngle.x += 0.01f;
	if( inputs.Key('d') )			m_vSunAngle.x -= 0.01f;

	m_vSunVector = vec4(	-cosf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							-cosf(m_vSunAngle.y),
							-sinf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							0.0f );
}

void SceneTerrain::Keyboard(bool special, unsigned char key)
{
	SceneBase::Keyboard(special, key);
}


void SceneTerrain::Reset()
{
	VarManager& var = VarManager::getInstance();

	Camera::getInstance().setEye(vec3(0.0f, 0.0f, 0.0f));

	SceneBase::Reset();
}



void SceneTerrain::SetLightCameraMatrices()
{
	VarManager& var = VarManager::getInstance();
	vec3 eye = Camera::getInstance().getEye();
//	eye.y = m_pTerrain->getPosition(eye.x, eye.z).y;


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(	eye.x - 500*m_vSunVector.x,		eye.y - 500*m_vSunVector.y,		eye.z - 500*m_vSunVector.z,
				eye.x,							eye.y,							eye.z,
				0.0,							1.0,							0.0	);
//	Frustum::getInstance().Extract(vec3(-m_vSunVector));
}


void SceneTerrain::RestoreLightCameraMatrices()
{
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void SceneTerrain::PreRender()
{
	Frustum& frust = Frustum::getInstance();
	VarManager& var = VarManager::getInstance();

	vec3 eye_pos = Camera::getInstance().getEye();
	vec3 sun_pos = eye_pos - vec3(m_vSunVector);
	GLdouble tabOrtho[TERRAIN_SHADOWMAPS_COUNT] = {20.0, 100.0};
//	float sizey = fabs(sinf(m_vSunAngle.y)) + 1.0f;
	float sizey = 1.0f;

	// On place la caméra à la lumière et on récupère les matrices
	SetLightCameraMatrices();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();


	// Affichage de la Depth map du point de vue du soleil dans un FBO
	for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-tabOrtho[i], tabOrtho[i], -tabOrtho[i]*sizey, tabOrtho[i]*sizey, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));
		glMatrixMode(GL_MODELVIEW);
		Frustum::getInstance().Extract(sun_pos);
		
		m_fboDepthMapFromLight[i].Begin();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderEnvironment(false, true);
		m_fboDepthMapFromLight[i].End();
	}


	// Récupération des matrices MV et proj de la lumière
//	Frustum::getInstance().Extract(sun_pos);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));
	Frustum::getInstance().Extract(sun_pos);
	mat4& matLightMV = frust.getModelviewMatrix();
	mat4& matLightProj = frust.getProjectionMatrix();
	m_matSunModelviewProj = matLightProj * matLightMV;


	// On remet les matrices comme avant
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	RestoreLightCameraMatrices();
	Frustum::getInstance().Extract(eye_pos);


	// Affichage de la scène réfléchie dans un FBO
	m_fboWaterReflection.Begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderEnvironment(true, false);
	m_fboWaterReflection.End();
}

void SceneTerrain::Render()
{
	VarManager& var = VarManager::getInstance();


	// Affichage de la scène normale
	RenderEnvironment(false, false);

	// Affichage de la surface de l'eau
	RenderWaterSurface();


/*
	// Affichage du texte
	glColor3f(1.0f, 1.0f, 1.0f);
	std::stringstream strStream;
	strStream << var.geti("terrain_chunks_drawn") << " chunks";
	DrawString(vec2(1.0f, 20.0f), GLUT_BITMAP_HELVETICA_18, strStream.str());
	strStream.str("");
	strStream << var.geti("terrain_chunks_reflected_drawn") << " reflected chunks";
	DrawString(vec2(1.0f, 1.0f), GLUT_BITMAP_HELVETICA_18, strStream.str());
*/
}


// Rendu de la surface de l'eau
void SceneTerrain::RenderWaterSurface()
{
	VarManager& var = VarManager::getInstance();
	float h = var.getf("water_height");


	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadMatrixf( m_matSunModelviewProj );
	glMatrixMode(GL_MODELVIEW);



	m_pShaderWater->Activate();
	{
		m_fboWaterReflection.Bind(0);
		m_pShaderWater->UniformTexture("texWaterReflection", 0);

		m_pTexWaterNoiseNM->Bind(1);
		m_pShaderWater->UniformTexture("texWaterNoiseNM", 1);

		m_pShaderWater->Uniform("win_width", var.geti("win_width"));
		m_pShaderWater->Uniform("win_height", var.geti("win_height"));
		m_pShaderWater->Uniform("noise_tile", 10.0f);
		m_pShaderWater->Uniform("noise_factor", 0.1f);
		m_pShaderWater->Uniform("time", Timer::getInstance().getCurrentTime());
		m_pShaderWater->Uniform("water_shininess", 50.0f);

		for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
			m_fboDepthMapFromLight[i].Bind(2 + i);

		m_pShaderWater->UniformTexture("texDepthMapFromLight0", 2);
		m_pShaderWater->UniformTexture("texDepthMapFromLight1", 3);
		m_pShaderWater->Uniform("depth_map_size", 512);


		vec3 e = Camera::getInstance().getEye();
		float d = 2.0f * var.getf("cam_zfar");

		glBegin(GL_QUADS);
			glVertex3f(e.x - d, h, e.z - d);
			glVertex3f(e.x - d, h, e.z + d);
			glVertex3f(e.x + d, h, e.z + d);
			glVertex3f(e.x + d, h, e.z - d);
		glEnd();

		for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
			m_fboDepthMapFromLight[i].Unbind(2 + i);

		m_pTexWaterNoiseNM->Unbind(1);
		m_fboWaterReflection.Unbind(0);
	}
	m_pShaderWater->Deactivate();

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();


}


// Rendu de l'environnement (pour la réflexion de l'eau ou non)
void SceneTerrain::RenderEnvironment(bool bWaterReflection, bool bDepthMap)
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();
	Camera& cam = Camera::getInstance();

	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 orange(1.0f, 0.5f, 0.0f, 1.0f);
	vec4 yellow(1.0f, 1.0f, 0.8f, 1.0f);
	float sun_cosy = cosf(m_vSunAngle.y);

	glPushAttrib(GL_ENABLE_BIT);


	if(!bDepthMap)
	{


		
		vec4 vSunColor = white.lerp(orange, yellow, 0.25f + sun_cosy * 0.75f);


		if(cam.getEye().y < var.getf("water_height"))
		{
			bWaterReflection = false;
			var.set("enable_underwater", true);
		}
		else
		{
			var.set("enable_underwater", false);
		}


		// Affichage du ciel et du soleil
		Sky::getInstance().DrawSkyAndSun( Camera::getInstance().getEye(), vec3(m_vSunVector), m_pSkybox->getHandle(), bWaterReflection );



		// Si c'est pour la réflexion, on dessine la scène à l'envers
		if(bWaterReflection) {
			glDisable(GL_CULL_FACE);
			glLoadIdentity();
			
			glScalef(-1.0f, 1.0f, 1.0f);
			cam.RenderLookAt(true, var.getf("water_height"));
		}

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		vec3 zeros(0.0f, 0.0f, 0.0f);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
		glLightfv(GL_LIGHT0, GL_POSITION, m_vSunVector);
		glLightfv(GL_LIGHT0, GL_AMBIENT, white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, vSunColor);
		glLightfv(GL_LIGHT0, GL_SPECULAR, vSunColor);

	}

	// Affichage des maisons à l'arrache
	m_pShaderLighting->Activate();
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/6);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);

		res.getTexture2D("wall_diffuse.jpg")->Bind(0);
		res.getTexture2D("wall_NM_height.tga")->Bind(1);

		m_pShaderLighting->UniformTexture("texDiffuse", 0);
		m_pShaderLighting->UniformTexture("texNormalHeightMap", 1);
		m_pShaderLighting->Uniform("mode", 1);
		m_pShaderLighting->Uniform("enable_shadow_mapping", 0);
		m_pShaderLighting->Uniform("tile_factor", 2.0f);

		glPushMatrix();
		glTranslatef(172.0f, 5.18f, 175.2f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(19.0f, 35.8f, -123.1f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.338f, 18.02f, 259.0f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(178.6f, 32.42f, 26.31f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		res.getTexture2D("wall_NM_height.tga")->Unbind(1);
		res.getTexture2D("wall_diffuse.jpg")->Unbind(0);
	}
	m_pShaderLighting->Deactivate();


	if(!bDepthMap)
	{
		vec4 vGroundAmbient = white.lerp(white*0.2f, black, sun_cosy);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vGroundAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);



		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadMatrixf( m_matSunModelviewProj );
		glMatrixMode(GL_MODELVIEW);

		// Activation des textures
		GLuint idx=0;
		for(GLuint i=0; i<m_tTextures.size(); i++)
			m_tTextures[i]->Bind(idx++);
		m_pTerrainDiffuseMap->Bind(idx++);
		if(!bDepthMap) {
			for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
				m_fboDepthMapFromLight[i].Bind(idx++);
		}

		// Affichage du terrain
		m_pShaderTerrain->Activate();
		{
			m_pShaderTerrain->Uniform("detail_scale", 120.0f);
			m_pShaderTerrain->Uniform("diffuse_scale", 70.0f);

			m_pShaderTerrain->Uniform("water_height", var.getf("water_height"));
			m_pShaderTerrain->Uniform("water_reflection_rendering", bWaterReflection);

			m_pShaderTerrain->Uniform("time", Timer::getInstance().getCurrentTime());

			m_pShaderTerrain->UniformTexture("texNormalHeightMap", 0);
			m_pShaderTerrain->UniformTexture("texDiffuse0", 1);
			m_pShaderTerrain->UniformTexture("texDiffuse1", 2);
			m_pShaderTerrain->UniformTexture("texDiffuse2", 3);
			m_pShaderTerrain->UniformTexture("texWaterCaustics", 4);
			m_pShaderTerrain->UniformTexture("texDiffuseMap", 5);
			m_pShaderTerrain->UniformTexture("texDepthMapFromLight0", 6);
			m_pShaderTerrain->UniformTexture("texDepthMapFromLight1", 7);

			m_pShaderTerrain->Uniform("depth_map_size", 512);

			int ret = m_pTerrain->DrawGround(bWaterReflection);
			var.set(bWaterReflection? "terrain_chunks_reflected_drawn" : "terrain_chunks_drawn", ret);

			m_pTerrain->DrawInfinitePlane(Camera::getInstance().getEye(), 2.0f*var.getf("cam_zfar"));
		}
		m_pShaderTerrain->Deactivate();

		
		// Désactivation des textures
		if(!bDepthMap) {
			for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
				m_fboDepthMapFromLight[i].Unbind(--idx);
		}
		m_pTerrainDiffuseMap->Unbind(--idx);
		for(GLint i=(GLint)m_tTextures.size()-1; i>=0; i--)
			m_tTextures[i]->Unbind(--idx);
		

		// Affichage de l'herbe
		if(!bWaterReflection)
		{
			glEnable(GL_BLEND);
			m_pShaderGrass->Activate();
			{
				res.getTexture2D("grass_billboards.tga")->Bind(0);
				for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
					m_fboDepthMapFromLight[i].Bind(1+i);

				m_pShaderGrass->UniformTexture("texDiffuse", 0);
				m_pShaderGrass->UniformTexture("texDepthMapFromLight0", 1);
				m_pShaderGrass->UniformTexture("texDepthMapFromLight1", 2);
				m_pShaderGrass->Uniform("depth_map_size", 512);

				m_pShaderGrass->Uniform("time", Timer::getInstance().getCurrentTime());
				m_pShaderGrass->Uniform("lod_metric", TERRAIN_GRASS_MAX_DISTANCE);

					m_pTerrain->DrawGrass(bWaterReflection);

				for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
					m_fboDepthMapFromLight[i].Unbind(1+i);
				res.getTexture2D("grass_billboards.tga")->Unbind(0);
			}
			m_pShaderGrass->Deactivate();
			glDisable(GL_BLEND);
		}

		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}


	m_pShaderTree->Activate();
	{
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadMatrixf( Frustum::getInstance().getModelviewInvMatrix() );
		glMatrixMode(GL_MODELVIEW);

		GLuint idx=0;
		res.getTexture2D("palm_texture.tga")->Bind(idx++);
		if(!bDepthMap) {
			for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
				m_fboDepthMapFromLight[i].Bind(idx++);
		}


		m_pShaderTree->UniformTexture("texDiffuse", 0);
		m_pShaderTree->UniformTexture("texDepthMapFromLight0", 1);
		m_pShaderTree->UniformTexture("texDepthMapFromLight1", 2);

		m_pShaderTree->Uniform("time", Timer::getInstance().getCurrentTime());

			m_pTerrain->DrawObjects(bWaterReflection);


		res.getTexture2D("palm_texture.tga")->Unbind(--idx);
		if(!bDepthMap) {
			for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
				m_fboDepthMapFromLight[i].Unbind(--idx);
		}

		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}
	m_pShaderTree->Deactivate();



	glPopAttrib();

}



