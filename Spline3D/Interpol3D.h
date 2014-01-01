#ifndef INTERPOL3D_H
#define INTERPOL3D_H


#ifdef WIN32
#include <windows.h>
#endif
#include "../Mathlib.h"
#include <vector>
#include <iostream>

class CInterpolNonFonct;

class Interpol3D
{
public:
	const std::vector<vec3>&	getControlPoints()	const	{return m_tCtrlPts;}
	bool						isClosed()			const	{return m_bClosed;}
	bool						isValid()			const	{return m_bBuilt && m_tCtrlPts.size() > 1;}

	Interpol3D();
//	Interpol3D(vec3 *, int);
	~Interpol3D();

	vec3 getPoint(float t); 

	bool AddPoint(const vec3& pt);
	bool DeleteLastPoint();
//	bool AddPoint(vec3 pt, int id);
	bool Clear();

//	bool DeletePoint(int id);
	void BuildSplines();
	bool Close();

//	void MovePoint(int id, vec3 pt); 
/*
	BOOL WriteInFile(FILE * hFile);
	BOOL ReadInFile(FILE * hFile);
*/

private:
	std::vector<vec3>	m_tCtrlPts;
	bool				m_bClosed;
	bool				m_bBuilt;

	CInterpolNonFonct *		m_pSpline1;
	CInterpolNonFonct *		m_pSpline2;
};    

#endif

