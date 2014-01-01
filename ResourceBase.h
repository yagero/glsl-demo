#ifndef RESOURCEBASE_H
#define RESOURCEBASE_H


// -------------------------------
// Resource abstraites
// -------------------------------

class ResourceBase
{
public:
	virtual bool Load(const std::string& name) = 0;
	virtual void Destroy() = 0;

	virtual bool DoesSupportReloading()	const {return false;}
};

#endif

