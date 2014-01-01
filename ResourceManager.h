#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Singleton.h"
#include <iostream>
#include <map>
#include <hash_map>
#include <string>

#include "Texture2D.h"
#include "TextureCubemap.h"
#include "Mesh.h"
#include "Shader.h"
#include <assert.h>

class ResourceBase;

SINGLETON_BEGIN( ResourceManager )
public:
	enum RES_TYPE {TEXTURE2D, TEXTURECUBEMAP, MESH, SHADER};

	void* LoadResource(RES_TYPE type, const std::string& name);
	void* NewResource(ResourceBase* data, const std::string& name);

	inline ResourceBase*	getResource(const std::string& name)		{assert(m_ResDB.find(name)!=m_ResDB.end()); return m_ResDB.find(name)->second;}
	inline Texture2D*		getTexture2D(const std::string& name)		{return (Texture2D*)getResource(name);}
	inline TextureCubemap*	getTextureCubemap(const std::string& name)	{return (TextureCubemap*)getResource(name);}
	inline Mesh*			getMesh(const std::string& name)			{return (Mesh*)getResource(name);}
	inline Shader*			getShader(const std::string& name)			{return (Shader*)getResource(name);}

	void Reload();

	void Destroy();

protected:
	~ResourceManager() {Destroy();}

private:
	typedef std::map<std::string, ResourceBase*> ResDB;
	ResDB	m_ResDB;

SINGLETON_END()

#endif

