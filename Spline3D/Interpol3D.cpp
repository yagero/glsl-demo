#include "Interpol3D.h"
#include "InterpolNonFonct.h"
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>


Interpol3D::Interpol3D() {
	m_bClosed = false;
	m_bBuilt = false;
	m_pSpline1 = NULL;
	m_pSpline2 = NULL;
}


void Interpol3D::BuildSplines() {

	m_bBuilt = false;

	if(m_pSpline1) delete m_pSpline1;
	if(m_pSpline2) delete m_pSpline2;
	m_pSpline1 = NULL;
	m_pSpline2 = NULL;

	if(m_tCtrlPts.size() > 1)
	{
		std::vector<vec2> tab1(m_tCtrlPts.size());
		std::vector<vec2> tab2(m_tCtrlPts.size());

		for(unsigned int i=0;i<m_tCtrlPts.size();i++) {
			tab1[i].x = m_tCtrlPts[i].x;
			tab1[i].y = m_tCtrlPts[i].y;
	          
			tab2[i].x = m_tCtrlPts[i].x;
			tab2[i].y = m_tCtrlPts[i].z;      
		}

		m_pSpline1 = new CInterpolNonFonct(tab1);
		m_pSpline2 = new CInterpolNonFonct(tab2);


		tab1.clear();
		tab2.clear();

		m_bBuilt = true;
	}
	
}


vec3 Interpol3D::getPoint(float t)
{
	if(m_bBuilt) {
		assert(m_pSpline1);
		assert(m_pSpline2);
		vec2 pt1 = m_pSpline1->getpoint(t);
		vec2 pt2 = m_pSpline2->getpoint(t);
		return vec3(pt1.x, pt1.y, pt2.y);
	}
	else {
		return vec3(0.0f, 0.0f, 0.0f);
	}
}


Interpol3D::~Interpol3D() {
	if(m_pSpline1)
		delete m_pSpline1;
	if(m_pSpline2)
		delete m_pSpline2;
	m_bBuilt = false;
}

bool Interpol3D::AddPoint(const vec3& pt)
{
	m_bBuilt = false;
	m_tCtrlPts.push_back(pt);
	return true;
}

bool Interpol3D::DeleteLastPoint()
{
	m_bBuilt = false;
	if(m_tCtrlPts.size() > 0)
		m_tCtrlPts.erase(m_tCtrlPts.end()-1);
	return true;
}

bool Interpol3D::Clear()
{
	m_bBuilt = false;
	m_tCtrlPts.clear();
	return true;
}

bool Interpol3D::Close()
{
	m_bBuilt = false;
	m_bClosed = true;

	if(m_tCtrlPts.size() > 1) {
		AddPoint( m_tCtrlPts[0] );
		return true;
	}

	m_bClosed = false;
	return false;
}









/*
bool Interpol3D::AddPoint(vec3 pt, int id) {
	// On ouvre la spline :
	bool bPrevState = _bClosed;
	SetClosed(false);

	// Gestion des erreurs :
	if(id > (int)_nCtrlPts) return false;
	if(id < 0)	id = 0;

	// Nouveau tableau :
	vec3 * NewTab = new vec3[_nCtrlPts+1];

	// Remplissage :
	for(int i=0; i<id; i++)				NewTab[i]	= _tCtrlPts[i];
	for(int i=_nCtrlPts-1; i>=id; i--)	NewTab[i+1] = _tCtrlPts[i];
	NewTab[id] = pt;

	_nCtrlPts++;			// un point en plus

	if(_tCtrlPts) delete [] _tCtrlPts;	// ancien tableau
	_tCtrlPts = NewTab;		// allocation du pointeur

	CreateSplines();		// on recrée les splines
	SetClosed(bPrevState);	// on remet la spline à son état précédent

	return true;
}
*/

/*
bool Interpol3D::DeletePoint(int id) {
	// On ouvre la spline :
	bool bPrevState = _bClosed;
	SetClosed(false);

	// Gestion des erreurs :
	if(id >= (int)_nCtrlPts)	return false;
	if(id < 0)					return false;

	// Nouveau tableau :
	vec3 * NewTab = new vec3[_nCtrlPts-1];

	// Remplissage :
	for(int i=0; i<id; i++)					NewTab[i]	= _tCtrlPts[i];
	for(int i=id+1; i<(int)_nCtrlPts; i++)	NewTab[i-1]	= _tCtrlPts[i];

	_nCtrlPts--;			// un point en moins

	if(_tCtrlPts) delete [] _tCtrlPts;	// ancien tableau
	_tCtrlPts = NewTab;		// allocation du pointeur

	CreateSplines();		// on recrée les splines
	SetClosed(bPrevState);	// on remet la spline à son état précédent

	return true;
}*/

/*
bool Interpol3D::SetClosed(bool Closed) {
 if(_bClosed == Closed) return false;	// dans ce cas, on change rien !

 // on dit que la spline est ouverte :
 _bClosed = false;

 if(_nCtrlPts > 1) {	// si il ya plus d'1 point de contrôle :
	if(Closed) {		// on ferme la spline
		AddPoint( _tCtrlPts[0], _nCtrlPts );
	}
	else {				// on ouvre la spline
		// Nouveau tableau :
		vec3 * NewTab = new vec3[_nCtrlPts-1];

		// Copie :
		for(unsigned int i=0; i<_nCtrlPts-1; i++)	NewTab[i] = _tCtrlPts[i];

		_nCtrlPts--;			// un point en moins
		if(_tCtrlPts) delete [] _tCtrlPts;	// ancien tableau
		_tCtrlPts = NewTab;		// allocation du pointeur
	}

	_bClosed = Closed;	// on sauvegarde l'état
	CreateSplines();	// on recréé les splines

	return true;
 }
 return false;
}*/

/*
void Interpol3D::MovePoint(int id, vec3 pt) {
 _tCtrlPts[ id ] += pt;

 if(id == 0 && bIsClosed()) {
	 _tCtrlPts[ _nCtrlPts - 1 ] += pt;
 }

 CreateSplines();
}
*/


/*
BOOL Interpol3D::WriteInFile(FILE * hFile) {
 CHAR c;

 if(_bClosed)	c = 'c';
 else			c = 'o';

 SetClosed(FALSE);

 // On écrit le nombre de points de contrôles :
 fwrite(&_nCtrlPts, sizeof(INT), 1, hFile);

 // On écrit le type de la courbe :
 fwrite(&c, sizeof(CHAR), 1, hFile);

 // On écrit les points de contrôles :
 for(UINT i= 0; i<_nCtrlPts; i++) {
	fwrite(&_tCtrlPts[i].x, sizeof(FLOAT), 1, hFile);
	fwrite(&_tCtrlPts[i].y, sizeof(FLOAT), 1, hFile);
	fwrite(&_tCtrlPts[i].z, sizeof(FLOAT), 1, hFile);
 }

 if(c == 'c')	SetClosed(TRUE);

 return TRUE;
}

BOOL Interpol3D::ReadInFile(FILE * hFile) {
 // On lit le nombre de points de contrôles :
 INT nCtrlPts;
 fread(&nCtrlPts, sizeof(INT), 1, hFile);

 // On lit le type de la courbe :
 CHAR c;
 fread(&c, sizeof(CHAR), 1, hFile);

 // On écrit les points de contrôles :
 for(INT i= 0; i<nCtrlPts; i++) {
	CPoint3d Pt;
	fread(&Pt.x, sizeof(FLOAT), 1, hFile);
	fread(&Pt.y, sizeof(FLOAT), 1, hFile);
	fread(&Pt.z, sizeof(FLOAT), 1, hFile);

	AddPoint(Pt, i);
 }

 if(c == 'c')	SetClosed(TRUE);
 else			SetClosed(FALSE);

 // On créé les splines :
 CreateSplines();

 return TRUE;
}*/


