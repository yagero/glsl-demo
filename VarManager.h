#ifndef VARMANAGER_H
#define VARMANAGER_H

#include "Singleton.h"
#include <iostream>
#include <map>
#include <hash_map>
#include <string>

#include <assert.h>

SINGLETON_BEGIN( VarManager )
public:
	inline void set(const std::string& name, float val)		{m_VarDB[name].type = 'f'; m_VarDB[name].fval = val;}
	inline void set(const std::string& name, int val)		{m_VarDB[name].type = 'i'; m_VarDB[name].ival = val;}
	inline void set(const std::string& name, bool val)		{m_VarDB[name].type = 'b'; m_VarDB[name].bval = val;}

	inline float getf(const std::string& name)				{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].fval;}
	inline int   geti(const std::string& name)				{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].ival;}
	inline bool  getb(const std::string& name)				{assert(m_VarDB.find(name)!=m_VarDB.end()); return m_VarDB[name].bval;}

	void Destroy();

protected:
	~VarManager() {Destroy();}

private:
	struct sVar {unsigned char type; union {float fval; int ival; bool bval;};};
	std::map<std::string, sVar>	m_VarDB;

SINGLETON_END()

#endif


