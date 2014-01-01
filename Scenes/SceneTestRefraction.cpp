#include "SceneTestRefraction.h"

#include "../Mathlib.h"
#include "../Camera.h"
#include "../Frustum.h"
#include "../Mesh.h"
#include "../Timer.h"
#include "../Sky.h"


#include "../Shader.h"
#include "../TextureCubemap.h"
#include "../Texture2D.h"
#include "../FrameBufferObject.h"
#include "../Inputs.h"

#include "../ResourceManager.h"
#include "../VarManager.h"


void SceneTestRefraction::Init()
{
	ResourceManager& res = ResourceManager::getInstance();

//	m_pSkybox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "posx.jpg negx.jpg posy.jpg negy.jpg posz.jpg negz.jpg");
	m_pSkybox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "sand_up.jpg sand_dn.jpg sand_tp.jpg sand_bm.jpg sand_lt.jpg sand_rt.jpg");

	m_pScene = (Mesh*)res.LoadResource(ResourceManager::MESH, "scene_refraction.obj");
	res.LoadResource(ResourceManager::MESH, "bunny_1500_smooth.obj");
	res.LoadResource(ResourceManager::MESH, "geosphere.obj");
	res.LoadResource(ResourceManager::MESH, "plane_front_and_back.obj");

	m_pShdRefraction = (Shader*)res.LoadResource(ResourceManager::SHADER, "reflection_refraction_perpixel3");
	m_pShdLighting = (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");
	res.LoadResource(ResourceManager::TEXTURE2D, "gordon.jpg");
//	res.LoadResource(ResourceManager::TEXTURE2D, "glossmap.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "gordon_normal.jpg");

	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "stones_diffuse.jpg");

	Texture2D::EnableGenerateMipmaps(false);
	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "stones_NM_height.tga");
	Texture2D::EnableGenerateMipmaps(true);

	m_fboCubemap[0].Create(FrameBufferObject::FBO_CUBE_COLOR, 512, 512);
	m_fboCubemap[1].Create(FrameBufferObject::FBO_CUBE_COLOR, 512, 512);
	m_fboCubemap[2].Create(FrameBufferObject::FBO_CUBE_COLOR, 512, 512);

	m_fObjectRotation = 0.0;

	LoadCameraTrajFromFile("refraction.txt");
}

void SceneTestRefraction::Idle(float fElapsedTime)
{
	m_fObjectRotation += 5.0f*fElapsedTime;
}

void SceneTestRefraction::Reset()
{
	VarManager& var = VarManager::getInstance();

	Camera::getInstance().setEye(vec3(0.0f, 0.0f, 0.0f));

	m_fObjectRotation = 0.0;

	SceneBase::Reset();
}


void SceneTestRefraction::PreRender()
{
	VarManager& var = VarManager::getInstance();

	Camera::getInstance().SaveCamera();

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity ();
	gluPerspective(90.0, 1.0, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));

	glMatrixMode (GL_MODELVIEW);

	vec3 vCubemapPos[3] = {	vec3(0.0f, 2.0f, 0.0f),
							vec3(6.0f*cosf(RADIANS(m_fObjectRotation)), 4.0f, -6.0f*sinf(RADIANS(m_fObjectRotation))),
							vec3(0.0, 4.0f, 8.0f) };

	static int idCubemap = 0;

	{
		// Cubemap de l'objet "idCubemap"
		for(GLuint i=0; i<6; i++)
		{
			// Position de la caméra :
			Camera::getInstance().RenderLookAtToCubeMap( vCubemapPos[ idCubemap ], i );

			m_fboCubemap[ idCubemap ].Begin(i);
				RenderScene( idCubemap+1 );
			m_fboCubemap[ idCubemap ].End(i);
		}
	}


	idCubemap++;
	if(idCubemap>2) idCubemap = 0;

	glMatrixMode (GL_PROJECTION);
	glPopMatrix();
	glMatrixMode (GL_MODELVIEW);

	Camera::getInstance().RestoreCamera();

}


void SceneTestRefraction::Render()
{
	RenderScene(0);
}



void SceneTestRefraction::RenderScene(int iCubeMap)
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();


	Sky::getInstance().DrawSky( Camera::getInstance().getEye(), m_pSkybox->getHandle() );

//	DrawAxes();



	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 zeros(0.0f, 0.0f, 0.0f);
	vec4 position(0.0f, 15.0f, 0.0f, 1.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f/512);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f/512);

	
//	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/10);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
//	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);





	m_pShdLighting->Activate();
	{
		m_pShdLighting->UniformTexture("texDiffuse", 0);
		m_pShdLighting->UniformTexture("texNormalHeightMap", 1);

		m_pShdLighting->Uniform("mode", iCubeMap? 2 : 3);
		m_pShdLighting->Uniform("tile_factor", 1.0f);
//		m_pShdLighting->Uniform("parallax_factor", 0.05f);
		m_pShdLighting->Uniform("relief_factor", 0.07f);
		m_pShdLighting->Uniform("enable_shadow_mapping", 0);

		res.getTexture2D("rocks_diffuse.jpg")->Bind(0);
		res.getTexture2D("rocks_NM_height.tga")->Bind(1);

			m_pScene->Draw(0);
			m_pScene->Draw(2);

		res.getTexture2D("stones_diffuse.jpg")->Bind(0);
		res.getTexture2D("stones_NM_height.tga")->Bind(1);
		m_pShdLighting->Uniform("relief_factor", 0.05f);

			m_pScene->Draw(1);

		res.getTexture2D("rocks_NM_height.tga")->Unbind(1);
		res.getTexture2D("rocks_diffuse.jpg")->Unbind(0);
	}
	m_pShdLighting->Deactivate();

	// DRAW LIGHT SOURCE
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	m_pScene->Draw(3);
	glEnable(GL_TEXTURE_2D);


	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf( Frustum::getInstance().getModelviewInvMatrix() );
	glMatrixMode(GL_MODELVIEW);


	m_pShdRefraction->Activate();

	m_pShdRefraction->Uniform("enable_chromatic_aberration", true);
	m_pShdRefraction->Uniform("enable_bumpmapping", false);
	m_pShdRefraction->Uniform("enable_diffuse", false);
	m_pShdRefraction->Uniform("enable_glossmap", false);
	m_pShdRefraction->Uniform("enable_refraction", true);
	m_pShdRefraction->Uniform("enable_reflection", true);
	m_pShdRefraction->Uniform("refraction_eta", 1.05f);
	m_pShdRefraction->Uniform("fresnel_bias", 0.2f);

	m_pShdRefraction->UniformTexture("texCubeMap", 0);

	if(iCubeMap!=1)
	{
		m_fboCubemap[0].Bind(0);
		{
			glPushMatrix();
				glTranslatef(0.0f, 2.0f, 0.0f);
				glScalef(40.f, 40.f, 40.f);
				res.getMesh("bunny_1500_smooth.obj")->Draw();
			glPopMatrix();
		}
		m_fboCubemap[0].Unbind(0);
	}


	if(iCubeMap!=2)
	{
		m_fboCubemap[1].Bind(0);
		{
			m_pShdRefraction->Uniform("fresnel_bias", 0.7f);

			glPushMatrix();
				glRotatef(m_fObjectRotation, 0.0f, 1.0f, 0.0f);
				glTranslatef(6.0f, 4.0f, 0.0f);
				glScalef(0.4f, 0.4f, 0.4f);
				res.getMesh("geosphere.obj")->Draw();
			glPopMatrix();
		}
		m_fboCubemap[1].Unbind(0);
	}


	if(iCubeMap!=3)
	{
		m_fboCubemap[2].Bind(0);
		res.getTexture2D("gordon.jpg")->Bind(1);
		res.getTexture2D("gordon_normal.jpg")->Bind(2);
		{
			m_pShdRefraction->UniformTexture("texDiffuse", 1);
			m_pShdRefraction->UniformTexture("texNormalMap", 2);
			m_pShdRefraction->Uniform("fresnel_bias", 0.2f);
			m_pShdRefraction->Uniform("enable_diffuse", true);
			m_pShdRefraction->Uniform("enable_bumpmapping", true);
			m_pShdRefraction->Uniform("bumpmapping_factor", 3.0f);
			m_pShdRefraction->Uniform("refraction_eta", 0.95f);
			m_pShdRefraction->Uniform("diffuse_intensity", 0.2f);

			glPushMatrix();
				glTranslatef(0.0f, 4.0f, 8.0f);
				glRotatef(-2.0f*m_fObjectRotation + 90.0f, 0.0f, 1.0f, 0.0f);
				glScalef(0.6f, 0.6f, 0.6f);
				res.getMesh("plane_front_and_back.obj")->Draw();
			glPopMatrix();
		}
		res.getTexture2D("gordon_normal.jpg")->Unbind(2);
		res.getTexture2D("gordon.jpg")->Unbind(1);
		m_fboCubemap[2].Unbind(0);
	}

	m_pShdRefraction->Deactivate();

	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}



