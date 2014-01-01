#include "SceneTestShadowMapping.h"

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
#include "../Spline3D/SplineGL.h"
#include "SceneManager.h"

void SceneTestShadowMapping::Init()
{
	ResourceManager& res = ResourceManager::getInstance();
	m_vLightPos = vec4(0.0f, 4.0f, 1.0f, 1.0f);
	m_vLightLookAt = vec3(0.0f, 0.0f, 0.0f);

	m_pSkybox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "arch_posx.jpg arch_negx.jpg arch_posy.jpg arch_negy.jpg arch_posz.jpg arch_negz.jpg");
	res.LoadResource(ResourceManager::MESH, "scene_shadow.obj");
	res.LoadResource(ResourceManager::MESH, "bunny_1500_smooth.obj");
	res.LoadResource(ResourceManager::MESH, "teapot.obj");
	m_pShader = (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");

	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "brick_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "sand_up.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "woodfloor005a_diffuse.jpg");

	Texture2D::EnableGenerateMipmaps(false);
	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "brick_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "grate01.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "grate02.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "grate03.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "woodfloor005a_NM.jpg");
	Texture2D::EnableGenerateMipmaps(true);

	m_fboDepthMapFromLight.Create(FrameBufferObject::FBO_2D_DEPTH, 1024, 1024);
	m_fboTextureProjected.Create(FrameBufferObject::FBO_2D_COLOR, 512, 512);
	m_pSubScene = SceneManager::getInstance().getScenePointer("terrain");
	assert(m_pSubScene);

	m_pSplineLightPosition = new SplineGL();
	m_pSplineLightPosition->AddPoint( vec3( 5.0, 5.0, 5.0) );
	m_pSplineLightPosition->AddPoint( vec3(-5.0, 2.0, 5.0) );
	m_pSplineLightPosition->AddPoint( vec3(-5.0, 0.5,-5.0) );
	m_pSplineLightPosition->AddPoint( vec3( 5.0, 2.0,-5.0) );
	m_pSplineLightPosition->BuildSplines(true);
	m_pSplineLightPosition->setSpeed(0.06f);

	m_pSplineLightLookAt = new SplineGL();
	m_pSplineLightLookAt->AddPoint( vec3( 3.0, 1.5, 3.0) );
	m_pSplineLightLookAt->AddPoint( vec3(-4.0, 3.0, 3.0) );
	m_pSplineLightLookAt->AddPoint( vec3(-3.0, 0.5,-3.0) );
	m_pSplineLightLookAt->AddPoint( vec3( 3.0, 1.6,-3.0) );
	m_pSplineLightLookAt->BuildSplines(true);
	m_pSplineLightLookAt->setSpeed(0.06f);

	m_fObjectRotation = 0.0f;

	LoadCameraTrajFromFile("shadow_mapping.txt");
}

void SceneTestShadowMapping::Destroy()
{
	if(m_pSplineLightLookAt)
		delete m_pSplineLightLookAt;
	if(m_pSplineLightPosition)
		delete m_pSplineLightPosition;
	SceneBase::Destroy();
}

void SceneTestShadowMapping::Idle(float fElapsedTime)
{
	Inputs& inputs = Inputs::getInstance();
/*
	if( inputs.Key('z') )			m_vLightPos.z += 0.1f;
	if( inputs.Key('s') )			m_vLightPos.z -= 0.1f;
	if( inputs.Key('q') )			m_vLightPos.x += 0.1f;
	if( inputs.Key('d') )			m_vLightPos.x -= 0.1f;
	if( inputs.Key('r') )			m_vLightPos.y += 0.1f;
	if( inputs.Key('f') )			m_vLightPos.y -= 0.1f;
*/
	m_pSplineLightLookAt->Idle(fElapsedTime);
	m_vLightLookAt = m_pSplineLightLookAt->getPoint();

	m_pSplineLightPosition->Idle(fElapsedTime);
	m_vLightPos = m_pSplineLightPosition->getPoint();

	m_fObjectRotation += 10.0f * fElapsedTime;

	m_pSubScene->Idle(fElapsedTime);
}

void SceneTestShadowMapping::Reset()
{
	VarManager& var = VarManager::getInstance();

	Camera::getInstance().setEye(vec3(0.0f, 0.0f, 0.0f));

	m_fObjectRotation = 0.0f;
	m_pSplineLightLookAt->setTime(0.0f);
	m_pSplineLightPosition->setTime(0.0f);

	SceneBase::Reset();
}


void SceneTestShadowMapping::SetLightCameraMatrices()
{
	VarManager& var = VarManager::getInstance();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(90.0, 1.0, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	m_vLightPos.x,		m_vLightPos.y,		m_vLightPos.z,
				m_vLightLookAt.x,	m_vLightLookAt.y,	m_vLightLookAt.z,
				0.0,				1.0,				0.0	);
	Frustum::getInstance().Extract(m_vLightPos);
}


void SceneTestShadowMapping::RestoreLightCameraMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void SceneTestShadowMapping::PreRender()
{
	Frustum& frust = Frustum::getInstance();

	// On place la caméra à la lumière et on récupère les matrices
	SetLightCameraMatrices();

	// Récupération des matrices MV et proj de la lumière
	mat4& matLightMV = frust.getModelviewMatrix();
	matLightMV.inverse(m_matLightModelviewInv);
	mat4& matLightProj = frust.getProjectionMatrix();
	m_matLightModelviewProj =  matLightProj * matLightMV;

	// Affichage de la scène du point de vue de la lumière
	m_fboDepthMapFromLight.Begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene(true);
	m_fboDepthMapFromLight.End();

	// On remet les matrices comme avant
	RestoreLightCameraMatrices();

}



void SceneTestShadowMapping::Render()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();


	Sky::getInstance().DrawSky( Camera::getInstance().getEye(), m_pSkybox->getHandle() );

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadMatrixf( Frustum::getInstance().getModelviewInvMatrix() );

	glActiveTexture(GL_TEXTURE1);
	glLoadMatrixf( m_matLightModelviewProj );
	glMatrixMode(GL_MODELVIEW);


//	DrawAxes();

	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 spotDirection = m_vLightLookAt-m_vLightPos;
	spotDirection.normalize();
//	vec3 spotDirection(1.0f, 0.0f, 0.0f);
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, m_vLightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f); // de 0 à 90°
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f/512);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f/512);
//	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);



	glPushMatrix();
	glMultMatrixf(m_matLightModelviewInv);
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidCone(0.7, 1.3, 16, 16);
	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(1.0f, 1.0f, 0.1f); 
	glutSolidSphere(0.7, 16, 4);
	glPopMatrix();


	
	glColor3f(1.0f, 1.0f, 1.0f);
	
//	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/3);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
//	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);



	m_pShader->Activate();
	{
		m_pShader->UniformTexture("texDiffuseProjected", 3);
		m_pShader->UniformTexture("texDepthMapFromLight", 2);
		m_pShader->UniformTexture("texDiffuse", 0);
		m_pShader->UniformTexture("texNormalHeightMap", 1);

		m_pShader->Uniform("mode", 2);
		m_pShader->Uniform("tile_factor", 1.0f);
		m_pShader->Uniform("parallax_factor", 0.02f);
		m_pShader->Uniform("relief_factor", 0.08f);
		m_pShader->Uniform("depth_map_size", 1024);
		m_pShader->Uniform("enable_shadow_mapping", 1);

			RenderScene(false);


	}
	m_pShader->Deactivate();

	
	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE1);
	glLoadIdentity();
	glActiveTexture(GL_TEXTURE0);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

//	m_pSplineLightLookAt->DrawGL();

}



void SceneTestShadowMapping::RenderScene(bool bDepthMap)
{
	ResourceManager& res = ResourceManager::getInstance();
	
	if( !bDepthMap ) {
		m_fboDepthMapFromLight.Bind(2);
		res.getTexture2D("sand_up.jpg")->Bind(3);
		//m_fboTextureProjected.Bind(3);
	}
	res.getTexture2D("rocks_diffuse.jpg")->Bind(0);
	res.getTexture2D("rocks_NM_height.tga")->Bind(1);

		res.getMesh("scene_shadow.obj")->Draw(0);


	res.getTexture2D("wall_diffuse.jpg")->Bind(0);
	res.getTexture2D("wall_NM_height.tga")->Bind(1);

		res.getMesh("scene_shadow.obj")->Draw(1);


	if(!bDepthMap) m_pShader->Uniform("mode", 1);

	res.getTexture2D("woodfloor005a_diffuse.jpg")->Bind(0);
	res.getTexture2D("woodfloor005a_NM.jpg")->Bind(1);

		glPushMatrix();
		glRotatef(m_fObjectRotation, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 1.0f, 0.0f);
		glScalef(20.f, 20.f, 20.f);
		res.getMesh("bunny_1500_smooth.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-14.0f, 2.8f, -0.5f);
		glRotatef(2.0f * m_fObjectRotation, 0.0f, -1.0f, 0.0f);
		glScalef(25.f, 25.f, 25.f);
		res.getMesh("bunny_1500_smooth.obj")->Draw();
		glPopMatrix();

		res.getMesh("scene_shadow.obj")->Draw(2);
		res.getMesh("scene_shadow.obj")->Draw(3);

		glPushMatrix();
		glRotatef(2.0f * m_fObjectRotation, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 8.0f, 15.5f);
		res.getMesh("teapot.obj")->Draw();
		glPopMatrix();

	res.getTexture2D("brick_diffuse.jpg")->Bind(0);
	res.getTexture2D("brick_NM_height.tga")->Bind(1);
		res.getMesh("scene_shadow.obj")->Draw(5);

	res.getTexture2D("wall_NM_height.tga")->Unbind(1);

	if(!bDepthMap) m_pShader->Uniform("mode", 2);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.4f);	// le facteur influe sur la précision de la zone transparente

	res.getTexture2D("grate02.tga")->Bind(0);

		res.getMesh("scene_shadow.obj")->Draw(4);

	res.getTexture2D("grate01.tga")->Bind(0);

		res.getMesh("scene_shadow.obj")->Draw(6);

	res.getTexture2D("grate01.tga")->Unbind(0);

	glDisable(GL_ALPHA_TEST);

	if( !bDepthMap ) {
		m_fboDepthMapFromLight.Unbind(2);
		res.getTexture2D("sand_up.jpg")->Unbind(3);
		//m_fboTextureProjected.Unbind(3);
	}
}



