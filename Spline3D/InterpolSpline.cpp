// InterpolSpline.cpp: implementation of the CInterpolSpline class.
//
//////////////////////////////////////////////////////////////////////

#include "InterpolSpline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterpolSpline::CInterpolSpline(const std::vector<vec2>& controlpoints) : CInterpol(controlpoints)
{
	int nbctrlpoints = (int)controlpoints.size();

	//Calcul des dérivés seconde
	dersec.resize(nbctrlpoints);

	float yp0=(ctrlpoints[1].y-ctrlpoints[0].y)/(ctrlpoints[1].x-ctrlpoints[0].x);
	float ypnm1=(ctrlpoints[nbctrlpoints-1].y-ctrlpoints[nbctrlpoints-2].y)/(ctrlpoints[nbctrlpoints-1].x-ctrlpoints[nbctrlpoints-2].x);
	
	calculderivsecondes(yp0,ypnm1);
}

CInterpolSpline::~CInterpolSpline()
{
	dersec.clear();

}

vec2 CInterpolSpline::getpoint(float t)
{
	vec2 ret;
	ret.x = ctrlpoints[0].x+t*(ctrlpoints[ctrlpoints.size()-1].x-ctrlpoints[0].x);

	int i=1;
	while (ctrlpoints[i].x<ret.x)
		i++;

	float xi=ctrlpoints[i-1].x;
	float xip1=ctrlpoints[i].x;

	float A=(xip1-ret.x)/(xip1-xi);
	float B=(ret.x-xi)/(xip1-xi);

	float C=((A*A*A-A)*((xip1-xi)*(xip1-xi)))/6.0f;
	float D=((B*B*B-B)*((xip1-xi)*(xip1-xi)))/6.0f;

	
	ret.y = A*ctrlpoints[i-1].y+B*ctrlpoints[i].y+C*dersec[i-1]+D*dersec[i];

	return ret;
}

void CInterpolSpline::calculderivsecondes(float yp0, float ypnm1)
{
	int nbctrlpoints = (int)ctrlpoints.size();
	int 	i,k;
	float p,qn,sig,un;

	std::vector<float> u(nbctrlpoints);

	//evite la pente initiale trop importante
	if (yp0 > 1e30)
		dersec[0]=u[0]=0.0f;
	else
	{
		dersec[0] = -0.5;
		u[0]=(3.0f/(float)(ctrlpoints[1].x-ctrlpoints[0].x))*((float)(ctrlpoints[1].y-ctrlpoints[0].y)/(float)(ctrlpoints[1].x-ctrlpoints[0].x)-yp0);
	}
	for (i=1;i<nbctrlpoints-1;i++)
	{
		sig=(float)(ctrlpoints[i].x-ctrlpoints[i-1].x)/(float)(ctrlpoints[i+1].x-ctrlpoints[i-1].x);
		p=sig*dersec[i-1]+2.0f;
		dersec[i]=(sig-1.0f)/p;
		u[i]=(float)(ctrlpoints[i+1].y-ctrlpoints[i].y)/(float)(ctrlpoints[i+1].x-ctrlpoints[i].x) - (float)(ctrlpoints[i].y-ctrlpoints[i-1].y)/(float)(ctrlpoints[i].x-ctrlpoints[i-1].x);
		u[i]=(6.0f*u[i]/(float)(ctrlpoints[i+1].x-ctrlpoints[i-1].x)-sig*u[i-1])/p;
	}

	//evite la pente finale trop importante
	if (ypnm1 > 0.99e30)
		qn=un=0.0f;
	else {
		qn=0.5f;
		un=(3.0f/(float)(ctrlpoints[nbctrlpoints-1].x-ctrlpoints[nbctrlpoints-2].x))*(ypnm1-(float)(ctrlpoints[nbctrlpoints-1].y-ctrlpoints[nbctrlpoints-2].y)/(float)(ctrlpoints[nbctrlpoints-1].x-ctrlpoints[nbctrlpoints-2].x));
	}
	dersec[nbctrlpoints-1]=(un-qn*u[nbctrlpoints-2])/(qn*dersec[nbctrlpoints-2]+1.0f);
	for (k=nbctrlpoints-2;k>=0;k--)		dersec[k]=dersec[k]*dersec[k+1]+u[k];
	u.clear();
}
