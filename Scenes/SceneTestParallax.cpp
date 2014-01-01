#include "SceneTestParallax.h"

#include "../Mathlib.h"
#include "../Camera.h"
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

void SceneTestParallax::Init()
{
	ResourceManager& res = ResourceManager::getInstance();
	m_vLightPos = vec4(0.0f, 5.0f, 0.0f, 1.0f);

	m_pSkybox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "arch_posx.jpg arch_negx.jpg arch_posy.jpg arch_negy.jpg arch_posz.jpg arch_negz.jpg");
	res.LoadResource(ResourceManager::MESH, "scene_shadow.obj");
	res.LoadResource(ResourceManager::MESH, "bart.obj");
	m_pShader = (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");

	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_diffuse.jpg");

	Texture2D::EnableGenerateMipmaps(false);
	res.LoadResource(ResourceManager::TEXTURE2D, "rocks_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "four_NM_height.tga");
	Texture2D::EnableGenerateMipmaps(true);
}

void SceneTestParallax::Idle(float fElapsedTime)
{
	Inputs& inputs = Inputs::getInstance();

	if( inputs.Key('z') )			m_vLightPos.z += 0.1f;
	if( inputs.Key('s') )			m_vLightPos.z -= 0.1f;
	if( inputs.Key('q') )			m_vLightPos.x += 0.1f;
	if( inputs.Key('d') )			m_vLightPos.x -= 0.1f;
	if( inputs.Key('w') )			m_vLightPos.y += 0.1f;
	if( inputs.Key('x') )			m_vLightPos.y -= 0.1f;
}

void SceneTestParallax::Reset()
{
	VarManager& var = VarManager::getInstance();

	var.set("enable_relief", true);
	var.set("enable_parallax", true);
	var.set("parallax_factor", 0.04f);
	var.set("relief_factor", 0.2f);
	var.set("tile_factor", 1.0f);

}


void SceneTestParallax::Render()
{
	ResourceManager& res = ResourceManager::getInstance();
	VarManager& var = VarManager::getInstance();

	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);

	Sky::getInstance().DrawSky( Camera::getInstance().getEye(), m_pSkybox->getHandle() );

	DrawAxes();

	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 zeros(0.0f, 0.0f, 0.0f);

//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, m_vLightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
//	glLightfv(GL_LIGHT0, GL_AMBIENT, &white.r);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f/512);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f/512);

	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(m_vLightPos.x, m_vLightPos.y, m_vLightPos.z);
	glutSolidSphere(0.1, 16, 16);
	glPopMatrix();

	

	
//	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/4);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
//	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);




	m_pShader->Activate();
	{

		res.getTexture2D("rocks_diffuse.jpg")->Bind(0);
		res.getTexture2D("rocks_NM_height.tga")->Bind(1);

		m_pShader->UniformTexture("texDiffuse", 0);
	//	m_pShader->UniformTexture("texNormalHeightMap", 1);

		m_pShader->Uniform("mode", 3);
		m_pShader->Uniform("enable_shadow_mapping", 0);

		m_pShader->Uniform("parallax_factor", 0.1f);
		m_pShader->Uniform("relief_factor", 0.1f);
		/*
		m_pShader->Uniform("tile_factor", var.getf("tile_factor"));
		m_pShader->Uniform("relief_factor", 0.1f);
*/
		res.getMesh("scene_shadow.obj")->Draw(0);

		m_pShader->Uniform("relief_factor", 0.07f);
		m_pShader->Uniform("tile_factor", 2.0f);
	//	res.getMesh("bart.obj")->Draw();


		res.getTexture2D("wall_diffuse.jpg")->Bind(0);
		res.getTexture2D("wall_NM_height.tga")->Bind(1);
	//	m_pShader->Uniform("relief_factor", 0.1f);
	//	m_pShader->Uniform("tile_factor", 1.0f);

		res.getMesh("scene_shadow.obj")->Draw(1);


		res.getTexture2D("four_diffuse.jpg")->Bind(0);
		res.getTexture2D("four_NM_height.tga")->Bind(1);
		m_pShader->Uniform("relief_factor", 0.2f);

		//res.getMesh("scene_bump.obj")->Draw(2);
		

	}
	m_pShader->Deactivate();

	res.getTexture2D("four_diffuse.jpg")->Unbind(0);
	res.getTexture2D("four_NM_height.tga")->Unbind(1);



}



