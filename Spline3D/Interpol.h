#ifndef INTERPOL_H
#define INTERPOL_H

#ifdef WIN32
#include <windows.h>
#endif
#include "../Mathlib.h"
#include <vector>
#include <iostream>

class CInterpol 
{
protected:
	std::vector<vec2> ctrlpoints;	//tableau des points de contrôle

public:
	CInterpol();
	CInterpol(const std::vector<vec2>& controlpoints);
	virtual ~CInterpol();

	virtual vec2 getpoint(float t)=0;

};
#endif 
