#ifndef INTERPOLNONFONCT_H
#define INTERPOLNONFONCT_H


#include "InterpolSpline.h"
#include "../Mathlib.h"
#include <vector>
#include <iostream>

class CInterpolNonFonct
{
public:
	CInterpolNonFonct(const std::vector<vec2>& controlpoints);
	virtual ~CInterpolNonFonct();

	CInterpolSpline * Splineya;
	CInterpolSpline * Splineyb;

	vec2 getpoint(float t);
};

#endif
