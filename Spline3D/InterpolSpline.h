
#ifndef INTERPOLSPLINE_H
#define INTERPOLSPLINE_H

#include "Interpol.h"

class CInterpolSpline : public CInterpol 
{
public:
	CInterpolSpline(const std::vector<vec2>& controlpoints);

	void calculderivsecondes(float yp0, float ypnm1);
	vec2 getpoint(float t);
	virtual ~CInterpolSpline();
private:
	std::vector<float> dersec;
};

#endif
