#include "ResourceManager.h"



void ResourceManager::Destroy()
{
	for(ResDB::iterator it = m_ResDB.begin(); it != m_ResDB.end(); it++) {
		delete (*it).second;
		(*it).second = NULL;
	}
	m_ResDB.clear();
}


void* ResourceManager::LoadResource(RES_TYPE type, const std::string& name)
{
	// La ressource est déja chargée, on ne la recharge pas :
	if(m_ResDB.find(name) != m_ResDB.end())
		return m_ResDB.find(name)->second;

	// on charge la ressource
	ResourceBase* ptr = NULL;
	switch(type) {
	case TEXTURE2D: {
		ptr = new Texture2D();
		break;}
	case TEXTURECUBEMAP: {
		ptr = new TextureCubemap();
		break;}
	case SHADER: {
		ptr = new Shader();
		break;}
	case MESH: {
		ptr = new Mesh();
		break;}
	}

	ptr->Load(name);

	if(!ptr) return NULL;

	m_ResDB[name] = ptr;
	return ptr;
}

void* ResourceManager::NewResource(ResourceBase* data, const std::string& name)
{
	if(!data) return NULL;

	// La ressource est déja chargée, on ne la recharge pas :
	if(m_ResDB.find(name) != m_ResDB.end())
		return m_ResDB.find(name)->second;

	m_ResDB[name] = data;
	return data;
}

void ResourceManager::Reload()
{
	for(ResDB::iterator it = m_ResDB.begin(); it != m_ResDB.end(); it++) {
		if((*it).second->DoesSupportReloading())
		{
			(*it).second->Destroy();
			(*it).second->Load((*it).first);
		}
	}
}


