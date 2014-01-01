// InterpolNonFonct.cpp: implementation of the CInterpolNonFonct class.
//
//////////////////////////////////////////////////////////////////////

#include "InterpolNonFonct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterpolNonFonct::CInterpolNonFonct(const std::vector<vec2>& controlpoints)
{
	std::vector<vec2> ctrlptsa(controlpoints.size());
	std::vector<vec2> ctrlptsb(controlpoints.size());


	for(int i=0; i<(int)controlpoints.size(); i++)
	{
		ctrlptsa[i] = vec2((float)i, controlpoints[i].x);
		ctrlptsb[i] = vec2((float)i, controlpoints[i].y);
	}

	Splineya = new CInterpolSpline(ctrlptsa);
	Splineyb = new CInterpolSpline(ctrlptsb);

	ctrlptsa.clear();
	ctrlptsb.clear();

}

CInterpolNonFonct::~CInterpolNonFonct()
{
	delete Splineya;
	delete Splineyb;

}

vec2 CInterpolNonFonct::getpoint(float t)
{
	vec2 pta = Splineya->getpoint(t);
	vec2 ptb = Splineyb->getpoint(t);
	return vec2(pta.y, ptb.y);
}
