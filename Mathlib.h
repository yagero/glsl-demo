/***************************************************************************
 * Mathlib
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 ***************************************************************************
 * Update 2004/08/19
 *
 * added ivec2, ivec3 & ivec4 methods
 * vec2, vec3 & vec4 data : added texture coords (s,t,p,q) and color enums (r,g,b,a)
 * mat3 & mat4 : added multiple float constructor ad modified methods returning mat3 or mat4
 * optimisations like "x / 2.0f" replaced by faster "x * 0.5f"
 * defines of multiples usefull maths values and radian/degree conversions
 * vec2 : added methods : set, reset, compare, dot, closestPointOnLine, closestPointOnSegment,
 *                        projectionOnLine, lerp, angle
 * vec3 : added methods : set, reset, compare, dot, cross, closestPointOnLine, closestPointOnSegment,
 *                        projectionOnLine, lerp, angle
 * vec4 : added methods : set, reset, compare
 ***************************************************************************
 */

#ifndef __HMATHLIB__
#define __HMATHLIB__

#include <math.h>

#define EPSILON				0.000001f
#define M_PI				3.141592653589793238462643383279f		// PI
#define M_PIDIV2			1.570796326794896619231321691639f		// PI / 2
#define M_2PI				6.283185307179586476925286766559f		// 2 * PI
#define M_PI2				9.869604401089358618834490999876f		// PI au carre
#define M_PIDIV180			0.01745329251994329576923690768488f		// PI / 180
#define M_180DIVPI			57.295779513082320876798154814105f		// 180 / PI

#define DegToRad(a)	(a)*=M_PIDIV180
#define RadToDeg(a)	(a)*=M_180DIVPI
#define RADIANS(a)	((a)*M_PIDIV180)
#define DEGRES(a)	((a)*M_180DIVPI)


const float INV_RAND_MAX = 1.0 / (RAND_MAX + 1);
inline float random(float max=1.0) { return max * rand() * INV_RAND_MAX; }
inline float random(float min, float max) { return min + (max - min) * INV_RAND_MAX * rand(); }
inline int random(int max=RAND_MAX) { return rand()%(max+1); }

class vec2;
class vec3;
class vec4;
class mat3;
class mat4;
class quat;
class ivec2;
class ivec3;
class ivec4;

/*****************************************************************************/
/*                                                                           */
/* vec2                                                                      */
/*                                                                           */
/*****************************************************************************/

class vec2 {
public:
	vec2(void) : x(0), y(0) { }
	vec2(float _x,float _y) : x(_x), y(_y) { }
	vec2(const float *_v) : x(_v[0]), y(_v[1]) { }
	vec2(const vec2 &_v) : x(_v.x), y(_v.y) { }
	vec2(const vec3 &_v);
	vec2(const vec4 &_v);

	int operator==(const vec2 &_v) { return (fabs(this->x - _v.x) < EPSILON && fabs(this->y - _v.y) < EPSILON); }
	int operator!=(const vec2 &_v) { return !(*this == _v); }

	vec2 &operator=(float _f) { this->x=_f; this->y=_f; return (*this); }
	const vec2 operator*(float _f) const { return vec2(this->x * _f,this->y * _f); }
	const vec2 operator/(float _f) const {
		if(fabs(_f) < EPSILON) return *this;
		_f = 1.0f / _f;
		return (*this) * _f;
	}
	const vec2 operator+(const vec2 &_v) const { return vec2(this->x + _v.x,this->y + _v.y); }
	const vec2 operator-() const { return vec2(-this->x,-this->y); }
	const vec2 operator-(const vec2 &_v) const { return vec2(this->x - _v.x,this->y - _v.y); }

	vec2 &operator*=(float _f) { return *this = *this * _f; }
	vec2 &operator/=(float _f) { return *this = *this / _f; }
	vec2 &operator+=(const vec2 &_v) { return *this = *this + _v; }
	vec2 &operator-=(const vec2 &_v) { return *this = *this - _v; }

	float operator*(const vec2 &_v) const { return this->x * _v.x + this->y * _v.y; }

	operator float*() { return this->v; }
	operator const float*() const { return this->v; }
//	float &operator[](int _i) { return this->v[_i]; }
//	const float &operator[](int _i) const { return this->v[_i]; }

	void set(float _x,float _y) { this->x = _x; this->y = _y; }
	void reset(void) { this->x = this->y = 0; }
	float length(void) const { return sqrtf(this->x * this->x + this->y * this->y); }
	float normalize(void) {
		float inv,l = this->length();
		if(l < EPSILON) return 0.0f;
		inv = 1.0f / l;
		this->x *= inv;
		this->y *= inv;
		return l;
	}
	float dot(const vec2 &v) { return ((this->x*v.x) + (this->y*v.y)); } // Produit scalaire
	bool compare(const vec2 &_v,float epsi=EPSILON) { return (fabs(this->x - _v.x) < epsi && fabs(this->y - _v.y) < epsi); }
	// retourne les coordonnée du point le plus proche de *this sur la droite passant par vA et vB
	vec2 closestPointOnLine(const vec2 &vA, const vec2 &vB) { return (((vB-vA) * this->projectionOnLine(vA, vB)) + vA); }
	// retourne les coordonnée du point le plus proche de *this sur le segment vA,vB
	vec2 closestPointOnSegment(const vec2 &vA, const vec2 &vB) {
		float factor = this->projectionOnLine(vA, vB);
		if (factor <= 0.0f) return vA;
		if (factor >= 1.0f) return vB;
		return (((vB-vA) * factor) + vA);
	}
	// retourne le facteur de la projection de *this sur la droite passant par vA et vB
	float projectionOnLine(const vec2 &vA, const vec2 &vB) {
		vec2 v(vB - vA);
		return v.dot(*this - vA) / v.dot(v);
	}
	// Fonction d'interpolation linéaire entre 2 vecteurs
	vec2 lerp(vec2 &u, vec2 &v, float factor) { return ((u * (1 - factor)) + (v * factor)); }
	vec2 lerp(vec2 &u, vec2 &v, vec2& factor) { return (vec2((u.x * (1 - factor.x)) + (v.x * factor.x), (u.y * (1 - factor.y)) + (v.y * factor.y))); }
	float angle(void) { return (float)atan2(this->y,this->x); }
	float angle(const vec2 &v) { return (float)atan2(v.y-this->y,v.x-this->x); }

	union {
		struct {float x,y;};
		struct {float s,t;};
		float v[2];
	};
};

inline vec2 operator*(float fl, const vec2& v)	{ return vec2(v.x*fl, v.y*fl);}

inline float Dot(const vec2& a, const vec2& b) { return(a.x*b.x+a.y*b.y); }

/*****************************************************************************/
/*                                                                           */
/* vec3                                                                      */
/*                                                                           */
/*****************************************************************************/

class vec3 {
public:
	vec3(void) : x(0), y(0), z(0) { }
	vec3(float _x,float _y,float _z) : x(_x), y(_y), z(_z) { }
	vec3(const float *_v) : x(_v[0]), y(_v[1]), z(_v[2]) { }
	vec3(const vec2 &_v,float _z) : x(_v.x), y(_v.y), z(_z) { }
	vec3(const vec3 &_v) : x(_v.x), y(_v.y), z(_v.z) { }
	vec3(const vec4 &_v);

	int operator==(const vec3 &_v) { return (fabs(this->x - _v.x) < EPSILON && fabs(this->y - _v.y) < EPSILON && fabs(this->z - _v.z) < EPSILON); }
	int operator!=(const vec3 &_v) { return !(*this == _v); }

	vec3 &operator=(float _f) { this->x=_f; this->y=_f; this->z=_f; return (*this); }
	const vec3 operator*(float _f) const { return vec3(this->x * _f,this->y * _f,this->z * _f); }
	const vec3 operator/(float _f) const {
		if(fabs(_f) < EPSILON) return *this;
		_f = 1.0f / _f;
		return (*this) * _f;
	}
	const vec3 operator+(const vec3 &_v) const { return vec3(this->x + _v.x,this->y + _v.y,this->z + _v.z); }
	const vec3 operator-() const { return vec3(-this->x,-this->y,-this->z); }
	const vec3 operator-(const vec3 &_v) const { return vec3(this->x - _v.x,this->y - _v.y,this->z - _v.z); }

	vec3 &operator*=(float _f) { return *this = *this * _f; }
	vec3 &operator/=(float _f) { return *this = *this / _f; }
	vec3 &operator+=(const vec3 &_v) { return *this = *this + _v; }
	vec3 &operator-=(const vec3 &_v) { return *this = *this - _v; }

	float operator*(const vec3 &_v) const { return this->x * _v.x + this->y * _v.y + this->z * _v.z; }
	float operator*(const vec4 &_v) const;

	operator float*() { return this->v; }
	operator const float*() const { return this->v; }
//	float &operator[](int _i) { return this->v[_i]; }
//	const float &operator[](int _i) const { return this->v[_i]; }

	void set(float _x,float _y,float _z) { this->x = _x; this->y = _y; this->z = _z; }
	void reset(void) { this->x = this->y = this->z = 0; }
	float length(void) const { return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z); }
	float normalize(void) {
		float inv,l = this->length();
		if(l < EPSILON) return 0.0f;
		inv = 1.0f / l;
		this->x *= inv;
		this->y *= inv;
		this->z *= inv;
		return l;
	}
	void cross(const vec3 &v1,const vec3 &v2) {
		this->x = v1.y * v2.z - v1.z * v2.y;
		this->y = v1.z * v2.x - v1.x * v2.z;
		this->z = v1.x * v2.y - v1.y * v2.x;
	}
	void cross(const vec3 &v2) {
		float x = this->y * v2.z - this->z * v2.y;
		float y = this->z * v2.x - this->x * v2.z;
		this->z = this->x * v2.y - this->y * v2.x;
		this->y = y;
		this->x = x;
	}
	float dot(const vec3 &v) { return ((this->x*v.x) + (this->y*v.y) + (this->z*v.z)); } // Produit scalaire
	bool compare(const vec3 &_v,float epsi=EPSILON) { return (fabs(this->x - _v.x) < epsi && fabs(this->y - _v.y) < epsi && fabs(this->z - _v.z) < epsi); }
	float angle(vec3 &v) { // retourne l'angle en radians entre *this et v
		float angle = (float)fabs(acos(this->dot(v)/(this->length()*v.length())));
		if(angle < EPSILON) return 0;
		return angle;
	}
	// retourne les coordonnée du point le plus proche de *this sur la droite passant par vA et vB
	vec3 closestPointOnLine(const vec3 &vA, const vec3 &vB) { return (((vB-vA) * this->projectionOnLine(vA, vB)) + vA); }
	// retourne les coordonnée du point le plus proche de *this sur le segment vA,vB
	vec3 closestPointOnSegment(const vec3 &vA, const vec3 &vB) {
		float factor = this->projectionOnLine(vA, vB);
		if (factor <= 0.0f) return vA;
		if (factor >= 1.0f) return vB;
		return (((vB-vA) * factor) + vA);
	}
	// retourne le facteur de la projection de *this sur la droite passant par vA et vB
	float projectionOnLine(const vec3 &vA, const vec3 &vB) {
		vec3 v(vB - vA);
		return v.dot(*this - vA) / v.dot(v);
	}
	// Fonction d'interpolation linéaire entre 2 vecteurs
	vec3 lerp(vec3 &u, vec3 &v, float factor) { return ((u * (1 - factor)) + (v * factor)); }
	vec3 lerp(vec3 &u, vec3 &v, vec3& factor) { return (vec3(	(u.x * (1 - factor.x)) + (v.x * factor.x),
																(u.y * (1 - factor.y)) + (v.y * factor.y),
																(u.z * (1 - factor.z)) + (v.z * factor.z))); }

	union {
		struct {float x,y,z;};
		struct {float s,t,p;};
		struct {float r,g,b;};
		float v[3];
	};
};

inline vec3 operator*(float fl, const vec3& v)	{ return vec3(v.x*fl, v.y*fl, v.z*fl);}


inline float Dot(const vec3& a, const vec3& b) { return(a.x*b.x+a.y*b.y+a.z*b.z); }
inline vec3 Cross(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline vec2::vec2(const vec3 &_v) {
	this->x = _v.x;
	this->y = _v.y;
}

// This calculates a vector between 2 points and returns the result
inline vec3& Vector(const vec3 &vp1, const vec3 &vp2) {
	vec3 *ret = new vec3(vp1.x - vp2.x, vp1.y - vp2.y, vp1.z - vp2.z);
	return *ret;
}

/*****************************************************************************/
/*                                                                           */
/* vec4                                                                      */
/*                                                                           */
/*****************************************************************************/

class vec4 {
public:
	vec4(void) : x(0), y(0), z(0), w(1) { }
	vec4(float _x,float _y,float _z,float _w) : x(_x), y(_y), z(_z), w(_w) { }
	vec4(const float *_v) : x(_v[0]), y(_v[1]), z(_v[2]), w(_v[3]) { }
	vec4(const vec3 &_v) : x(_v.x), y(_v.y), z(_v.z), w(1) { }
	vec4(const vec3 &_v,float _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) { }
	vec4(const vec4 &_v) : x(_v.x), y(_v.y), z(_v.z), w(_v.w) { }

	int operator==(const vec4 &_v) { return (fabs(this->x - _v.x) < EPSILON && fabs(this->y - _v.y) < EPSILON && fabs(this->z - _v.z) < EPSILON && fabs(this->w - _v.w) < EPSILON); }
	int operator!=(const vec4 &_v) { return !(*this == _v); }

	vec4 &operator=(float _f) { this->x=_f; this->y=_f; this->z=_f; this->w=_f; return (*this); }
	const vec4 operator*(float _f) const { return vec4(this->x * _f,this->y * _f,this->z * _f,this->w * _f); }
	const vec4 operator/(float _f) const {
		if(fabs(_f) < EPSILON) return *this;
		_f = 1.0f / _f;
		return (*this) * _f;
	}
	const vec4 operator+(const vec4 &_v) const { return vec4(this->x + _v.x,this->y + _v.y,this->z + _v.z,this->w + _v.w); }
	const vec4 operator-() const { return vec4(-x,-y,-z,-w); }
	const vec4 operator-(const vec4 &_v) const { return vec4(this->x - _v.x,this->y - _v.y,this->z - _v.z,this->w - _v.w); }

	vec4 &operator*=(float _f) { return *this = *this * _f; }
	vec4 &operator/=(float _f) { return *this = *this / _f; }
	vec4 &operator+=(const vec4 &_v) { return *this = *this + _v; }
	vec4 &operator-=(const vec4 &_v) { return *this = *this - _v; }

	float operator*(const vec3 &_v) const { return this->x * _v.x + this->y * _v.y + this->z * _v.z + this->w; }
	float operator*(const vec4 &_v) const { return this->x * _v.x + this->y * _v.y + this->z * _v.z + this->w * _v.w; }

	operator float*() { return this->v; }
	operator const float*() const { return this->v; }
//	float &operator[](int _i) { return this->v[_i]; }
//	const float &operator[](int _i) const { return this->v[_i]; }

	void set(float _x,float _y,float _z,float _w) { this->x=_x; this->y=_y; this->z=_z; this->w=_w; }
	void reset(void) { this->x = this->y = this->z = this->w = 0; }
	bool compare(const vec4 &_v,float epsi=EPSILON) { return (fabs(this->x - _v.x) < epsi && fabs(this->y - _v.y) < epsi && fabs(this->z - _v.z) < epsi && fabs(this->w - _v.w) < epsi); }

	vec4 lerp(const vec4 &u, const vec4 &v, float factor) { return ((u * (1 - factor)) + (v * factor)); }

	union {
		struct {float x,y,z,w;};
		struct {float s,t,p,q;};
		struct {float r,g,b,a;};
		struct {float fov,ratio,znear,zfar;};
		float v[4];
	};
};

inline vec4 operator*(float fl, const vec4& v)	{ return vec4(v.x*fl, v.y*fl, v.z*fl,  v.w*fl);}

inline vec3::vec3(const vec4 &_v) {
	this->x = _v.x;
	this->y = _v.y;
	this->z = _v.z;
}

inline float vec3::operator*(const vec4 &_v) const {
	return this->x * _v.x + this->y * _v.y + this->z * _v.z + _v.w;
}

inline vec2::vec2(const vec4 &_v) {
	this->x = _v.x;
	this->y = _v.y;
}

/*****************************************************************************/
/*                                                                           */
/* mat3                                                                      */
/*                                                                           */
/*****************************************************************************/

class mat3 {
public:
	mat3(void) {
		mat[0] = 1.0; mat[3] = 0.0; mat[6] = 0.0;
		mat[1] = 0.0; mat[4] = 1.0; mat[7] = 0.0;
		mat[2] = 0.0; mat[5] = 0.0; mat[8] = 1.0;
	}
	mat3(float m0, float m1, float m2,
		 float m3, float m4, float m5,
		 float m6, float m7, float m8) {
		mat[0] = m0; mat[3] = m3; mat[6] = m6;
		mat[1] = m1; mat[4] = m4; mat[7] = m7;
		mat[2] = m2; mat[5] = m5; mat[8] = m8;
	}
	mat3(const float *m) {
		mat[0] = m[0]; mat[3] = m[3]; mat[6] = m[6];
		mat[1] = m[1]; mat[4] = m[4]; mat[7] = m[7];
		mat[2] = m[2]; mat[5] = m[5]; mat[8] = m[8];
	}
	mat3(const mat3 &m) {
		mat[0] = m[0]; mat[3] = m[3]; mat[6] = m[6];
		mat[1] = m[1]; mat[4] = m[4]; mat[7] = m[7];
		mat[2] = m[2]; mat[5] = m[5]; mat[8] = m[8];
	}
	mat3(const mat4 &m);
	
	vec3 operator*(const vec3 &v) const {
		return vec3(mat[0] * v[0] + mat[3] * v[1] + mat[6] * v[2],
					mat[1] * v[0] + mat[4] * v[1] + mat[7] * v[2],
					mat[2] * v[0] + mat[5] * v[1] + mat[8] * v[2]);
	}
	vec4 operator*(const vec4 &v) const {
		return vec4(mat[0] * v[0] + mat[3] * v[1] + mat[6] * v[2],
					mat[1] * v[0] + mat[4] * v[1] + mat[7] * v[2],
					mat[2] * v[0] + mat[5] * v[1] + mat[8] * v[2],
					v[3]);
	}
	mat3 operator*(float f) const {
		return mat3(mat[0] * f, mat[1] * f, mat[2] * f,
					mat[3] * f,	mat[4] * f,	mat[5] * f,
					mat[6] * f,	mat[7] * f,	mat[8] * f);
	}
	mat3 operator*(const mat3 &m) const {
		return mat3(mat[0] * m[0] + mat[3] * m[1] + mat[6] * m[2],
					mat[1] * m[0] + mat[4] * m[1] + mat[7] * m[2],
					mat[2] * m[0] + mat[5] * m[1] + mat[8] * m[2],
					mat[0] * m[3] + mat[3] * m[4] + mat[6] * m[5],
					mat[1] * m[3] + mat[4] * m[4] + mat[7] * m[5],
					mat[2] * m[3] + mat[5] * m[4] + mat[8] * m[5],
					mat[0] * m[6] + mat[3] * m[7] + mat[6] * m[8],
					mat[1] * m[6] + mat[4] * m[7] + mat[7] * m[8],
					mat[2] * m[6] + mat[5] * m[7] + mat[8] * m[8]);
	}
	mat3 operator+(const mat3 &m) const {
		return mat3(mat[0] + m[0], mat[1] + m[1], mat[2] + m[2],
					mat[3] + m[3], mat[4] + m[4], mat[5] + m[5],
					mat[6] + m[6], mat[7] + m[7], mat[8] + m[8]);
	}
	mat3 operator-(const mat3 &m) const {
		return mat3(mat[0] - m[0], mat[1] - m[1], mat[2] - m[2],
					mat[3] - m[3], mat[4] - m[4], mat[5] - m[5],
					mat[6] - m[6], mat[7] - m[7], mat[8] - m[8]);
	}
	
	mat3 &operator*=(float f) { return *this = *this * f; }
	mat3 &operator*=(const mat3 &m) { return *this = *this * m; }
	mat3 &operator+=(const mat3 &m) { return *this = *this + m; }
	mat3 &operator-=(const mat3 &m) { return *this = *this - m; }
	
	operator float*() { return mat; }
	operator const float*() const { return mat; }
	
	float &operator[](int i) { return mat[i]; }
	const float operator[](int i) const { return mat[i]; }
	
	mat3 transpose(void) const {
		return mat3(mat[0], mat[3], mat[6],
					mat[1], mat[4], mat[2],
					mat[7], mat[5], mat[8]);
	}
	float det(void) const {
		return ((mat[0] * mat[4] * mat[8]) +
				(mat[3] * mat[7] * mat[2]) +
				(mat[6] * mat[1] * mat[5]) -
				(mat[6] * mat[4] * mat[2]) -
				(mat[3] * mat[1] * mat[8]) -
				(mat[0] * mat[7] * mat[5]));
	}
	mat3 inverse(void) const {
		float idet = 1.0f / det();
		return mat3((mat[4] * mat[8] - mat[7] * mat[5]) * idet,
					-(mat[1] * mat[8] - mat[7] * mat[2]) * idet,
					 (mat[1] * mat[5] - mat[4] * mat[2]) * idet,
					-(mat[3] * mat[8] - mat[6] * mat[5]) * idet,
					 (mat[0] * mat[8] - mat[6] * mat[2]) * idet,
					-(mat[0] * mat[5] - mat[3] * mat[2]) * idet,
					 (mat[3] * mat[7] - mat[6] * mat[4]) * idet,
					-(mat[0] * mat[7] - mat[6] * mat[1]) * idet,
					 (mat[0] * mat[4] - mat[3] * mat[1]) * idet);
	}
	
	void zero(void) {
		mat[0] = 0.0; mat[3] = 0.0; mat[6] = 0.0;
		mat[1] = 0.0; mat[4] = 0.0; mat[7] = 0.0;
		mat[2] = 0.0; mat[5] = 0.0; mat[8] = 0.0;
	}
	void identity(void) {
		mat[0] = 1.0; mat[3] = 0.0; mat[6] = 0.0;
		mat[1] = 0.0; mat[4] = 1.0; mat[7] = 0.0;
		mat[2] = 0.0; mat[5] = 0.0; mat[8] = 1.0;
	}
	void rotate(const vec3 &v,float angle) {
		rotate(v.x,v.y,v.z,angle);
	}
	void rotate(float x,float y,float z,float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		float l = sqrtf(x * x + y * y + z * z);
		if(l < EPSILON) l = 1.0f;
		else l = 1.0f / l;
		x *= l;
		y *= l;
		z *= l;
		float xy = x * y;
		float yz = y * z;
		float zx = z * x;
		float xs = x * s;
		float ys = y * s;
		float zs = z * s;
		float c1 = 1.0f - c;
		mat[0] = c1 * x * x + c;	mat[3] = c1 * xy - zs;		mat[6] = c1 * zx + ys;
		mat[1] = c1 * xy + zs;		mat[4] = c1 * y * y + c;	mat[7] = c1 * yz - xs;
		mat[2] = c1 * zx - ys;		mat[5] = c1 * yz + xs;		mat[8] = c1 * z * z + c;
	}
	void rotate_x(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		mat[0] = 1.0; mat[3] = 0.0; mat[6] = 0.0;
		mat[1] = 0.0; mat[4] = c; mat[7] = -s;
		mat[2] = 0.0; mat[5] = s; mat[8] = c;
	}
	void rotate_y(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		mat[0] = c; mat[3] = 0.0; mat[6] = s;
		mat[1] = 0.0; mat[4] = 1.0; mat[7] = 0.0;
		mat[2] = -s; mat[5] = 0.0; mat[8] = c;
	}
	void rotate_z(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		mat[0] = c; mat[3] = -s; mat[6] = 0.0;
		mat[1] = s; mat[4] = c; mat[7] = 0.0;
		mat[2] = 0.0; mat[5] = 0.0; mat[8] = 1.0;
	}
	void scale(float x,float y,float z) {
		mat[0] = x; mat[3] = 0; mat[6] = 0;
		mat[1] = 0; mat[4] = y; mat[7] = 0;
		mat[2] = 0; mat[5] = 0; mat[8] = z;
	}
	void scale(const vec3 &v) {
		scale(v.x,v.y,v.z);
	}
	void orthonormalize(void) {
		vec3 x(mat[0],mat[1],mat[2]);
		vec3 y(mat[3],mat[4],mat[5]);
		vec3 z;
		x.normalize();
		z.cross(x,y);
		z.normalize();
		y.cross(z,x);
		y.normalize();
		mat[0] = x.x; mat[3] = y.x; mat[6] = z.x;
		mat[1] = x.y; mat[4] = y.y; mat[7] = z.y;
		mat[2] = x.z; mat[5] = y.z; mat[8] = z.z;
	}

	float mat[9];
};

/*****************************************************************************/
/*                                                                           */
/* mat4                                                                      */
/*                                                                           */
/*****************************************************************************/

class mat4 {
public:
	mat4(void) {
		this->mat[0] = 1.0; this->mat[4] = 0.0; this->mat[8]  = 0.0; this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] = 1.0; this->mat[9]  = 0.0; this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = 1.0; this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	mat4(float m0, float m1, float m2, float m3,
		 float m4, float m5, float m6, float m7,
		 float m8, float m9, float m10,float m11,
		 float m12,float m13,float m14,float m15) {
		this->mat[0] = m0; this->mat[4] = m4; this->mat[8]  = m8;  this->mat[12] = m12;
		this->mat[1] = m1; this->mat[5] = m5; this->mat[9]  = m9;  this->mat[13] = m13;
		this->mat[2] = m2; this->mat[6] = m6; this->mat[10] = m10; this->mat[14] = m14;
		this->mat[3] = m3; this->mat[7] = m7; this->mat[11] = m11; this->mat[15] = m15;
	}
	mat4(const vec3 &v) {
		translate(v);
	}
	mat4(float x,float y,float z) {
		translate(x,y,z);
	}
	mat4(const vec3 &axis,float angle) {
		rotate(axis,angle);
	}
	mat4(float x,float y,float z,float angle) {
		rotate(x,y,z,angle);
	}
	mat4(const mat3 &m) {
		this->mat[0] = m[0]; this->mat[4] = m[3]; this->mat[8]  = m[6]; this->mat[12] = 0.0;
		this->mat[1] = m[1]; this->mat[5] = m[4]; this->mat[9]  = m[7]; this->mat[13] = 0.0;
		this->mat[2] = m[2]; this->mat[6] = m[5]; this->mat[10] = m[8]; this->mat[14] = 0.0;
		this->mat[3] = 0.0;  this->mat[7] = 0.0;  this->mat[11] = 0.0;  this->mat[15] = 1.0;
	}
	mat4(const float *m) {
		this->mat[0] = m[0]; this->mat[4] = m[4]; this->mat[8]  = m[8];  this->mat[12] = m[12];
		this->mat[1] = m[1]; this->mat[5] = m[5]; this->mat[9]  = m[9];  this->mat[13] = m[13];
		this->mat[2] = m[2]; this->mat[6] = m[6]; this->mat[10] = m[10]; this->mat[14] = m[14];
		this->mat[3] = m[3]; this->mat[7] = m[7]; this->mat[11] = m[11]; this->mat[15] = m[15];
	}
	mat4(const mat4 &m) {
		this->mat[0] = m[0]; this->mat[4] = m[4]; this->mat[8]  = m[8];  this->mat[12] = m[12];
		this->mat[1] = m[1]; this->mat[5] = m[5]; this->mat[9]  = m[9];  this->mat[13] = m[13];
		this->mat[2] = m[2]; this->mat[6] = m[6]; this->mat[10] = m[10]; this->mat[14] = m[14];
		this->mat[3] = m[3]; this->mat[7] = m[7]; this->mat[11] = m[11]; this->mat[15] = m[15];
	}
	
	vec3 operator*(const vec3 &v) const {
		return vec3(this->mat[0] * v[0] + this->mat[4] * v[1] + this->mat[8]  * v[2] + this->mat[12],
					this->mat[1] * v[0] + this->mat[5] * v[1] + this->mat[9]  * v[2] + this->mat[13],
					this->mat[2] * v[0] + this->mat[6] * v[1] + this->mat[10] * v[2] + this->mat[14]);
	}
	vec4 operator*(const vec4 &v) const {
		return vec4(this->mat[0] * v[0] + this->mat[4] * v[1] + this->mat[8]  * v[2] + this->mat[12] * v[3],
					this->mat[1] * v[0] + this->mat[5] * v[1] + this->mat[9]  * v[2] + this->mat[13] * v[3],
					this->mat[2] * v[0] + this->mat[6] * v[1] + this->mat[10] * v[2] + this->mat[14] * v[3],
					this->mat[3] * v[0] + this->mat[7] * v[1] + this->mat[11] * v[2] + this->mat[15] * v[3]);
	}
	mat4 operator*(float f) const {
		return mat4(this->mat[0]  * f, this->mat[1]  * f, this->mat[2]  * f, this->mat[3]  * f,
					this->mat[4]  * f, this->mat[5]  * f, this->mat[6]  * f, this->mat[7]  * f,
					this->mat[8]  * f, this->mat[9]  * f, this->mat[10] * f, this->mat[11] * f,
					this->mat[12] * f, this->mat[13] * f, this->mat[14] * f, this->mat[15] * f);
	}
	mat4 operator*(const mat4 &m) const {
		return mat4(this->mat[0] * m[0]  + this->mat[4] * m[1]  + this->mat[8]  * m[2]  + this->mat[12] * m[3],
					this->mat[1] * m[0]  + this->mat[5] * m[1]  + this->mat[9]  * m[2]  + this->mat[13] * m[3],
					this->mat[2] * m[0]  + this->mat[6] * m[1]  + this->mat[10] * m[2]  + this->mat[14] * m[3],
					this->mat[3] * m[0]  + this->mat[7] * m[1]  + this->mat[11] * m[2]  + this->mat[15] * m[3],
					this->mat[0] * m[4]  + this->mat[4] * m[5]  + this->mat[8]  * m[6]  + this->mat[12] * m[7],
					this->mat[1] * m[4]  + this->mat[5] * m[5]  + this->mat[9]  * m[6]  + this->mat[13] * m[7],
					this->mat[2] * m[4]  + this->mat[6] * m[5]  + this->mat[10] * m[6]  + this->mat[14] * m[7],
					this->mat[3] * m[4]  + this->mat[7] * m[5]  + this->mat[11] * m[6]  + this->mat[15] * m[7],
					this->mat[0] * m[8]  + this->mat[4] * m[9]  + this->mat[8]  * m[10] + this->mat[12] * m[11],
					this->mat[1] * m[8]  + this->mat[5] * m[9]  + this->mat[9]  * m[10] + this->mat[13] * m[11],
					this->mat[2] * m[8]  + this->mat[6] * m[9]  + this->mat[10] * m[10] + this->mat[14] * m[11],
					this->mat[3] * m[8]  + this->mat[7] * m[9]  + this->mat[11] * m[10] + this->mat[15] * m[11],
					this->mat[0] * m[12] + this->mat[4] * m[13] + this->mat[8]  * m[14] + this->mat[12] * m[15],
					this->mat[1] * m[12] + this->mat[5] * m[13] + this->mat[9]  * m[14] + this->mat[13] * m[15],
					this->mat[2] * m[12] + this->mat[6] * m[13] + this->mat[10] * m[14] + this->mat[14] * m[15],
					this->mat[3] * m[12] + this->mat[7] * m[13] + this->mat[11] * m[14] + this->mat[15] * m[15]);
	}
	mat4 operator+(const mat4 &m) const {
		return mat4(this->mat[0]  + m[0],  this->mat[1]  + m[1],  this->mat[2]  + m[2],  this->mat[3]  + m[3],
					this->mat[4]  + m[4],  this->mat[5]  + m[5],  this->mat[6]  + m[6],  this->mat[7]  + m[7],
					this->mat[8]  + m[8],  this->mat[9]  + m[9],  this->mat[10] + m[10], this->mat[11] + m[11],
					this->mat[12] + m[12], this->mat[13] + m[13], this->mat[14] + m[14], this->mat[15] + m[15]);
	}
	mat4 operator-(const mat4 &m) const {
		return mat4(this->mat[0]  - m[0],  this->mat[1]  - m[1],  this->mat[2]  - m[2],  this->mat[3]  - m[3],
					this->mat[4]  - m[4],  this->mat[5]  - m[5],  this->mat[6]  - m[6],  this->mat[7]  - m[7],
					this->mat[8]  - m[8],  this->mat[9]  - m[9],  this->mat[10] - m[10], this->mat[11] - m[11],
					this->mat[12] - m[12], this->mat[13] - m[13], this->mat[14] - m[14], this->mat[15] - m[15]);
	}
	
	mat4 &operator*=(float f) { return *this = *this * f; }
	mat4 &operator*=(const mat4 &m) { return *this = *this * m; }
	mat4 &operator+=(const mat4 &m) { return *this = *this + m; }
	mat4 &operator-=(const mat4 &m) { return *this = *this - m; }
	
	operator float*() { return this->mat; }
	operator const float*() const { return this->mat; }
	
	float &operator[](int i) { return this->mat[i]; }
	const float operator[](int i) const { return this->mat[i]; }
	
	mat4 rotation(void) const {
		return mat4(this->mat[0], this->mat[1], this->mat[2], 0,
					this->mat[4], this->mat[5], this->mat[6], 0,
					this->mat[8], this->mat[9], this->mat[10],0,
					0, 0, 0, 1);
	}
	mat4 transpose(void) const {
		return mat4(this->mat[0], this->mat[4], this->mat[8],  this->mat[12],
					this->mat[1], this->mat[5], this->mat[9],  this->mat[13],
					this->mat[2], this->mat[6], this->mat[10], this->mat[14],
					this->mat[3], this->mat[7], this->mat[11], this->mat[15]);
	}
	mat4 transpose_rotation(void) const {
		return mat4(this->mat[0],  this->mat[4],  this->mat[8],  this->mat[3],
					this->mat[1],  this->mat[5],  this->mat[9],  this->mat[7],
					this->mat[2],  this->mat[6],  this->mat[10], this->mat[11],
					this->mat[12], this->mat[13], this->mat[14], this->mat[15]);
	}
	
	float det(void) const {
		return ((this->mat[0] * this->mat[5] * this->mat[10]) +
				(this->mat[4] * this->mat[9] * this->mat[2])  +
				(this->mat[8] * this->mat[1] * this->mat[6])  -
				(this->mat[8] * this->mat[5] * this->mat[2])  -
				(this->mat[4] * this->mat[1] * this->mat[10]) -
				(this->mat[0] * this->mat[9] * this->mat[6]));
	}
	
	void inverse(mat4& ret) {
//		mat4 *ret = new mat4();
		float idet = 1.0f / det();
		ret[0]  =  (this->mat[5] * this->mat[10] - this->mat[9] * this->mat[6]) * idet;
		ret[1]  = -(this->mat[1] * this->mat[10] - this->mat[9] * this->mat[2]) * idet;
		ret[2]  =  (this->mat[1] * this->mat[6]  - this->mat[5] * this->mat[2]) * idet;
		ret[3]  = 0.0;
		ret[4]  = -(this->mat[4] * this->mat[10] - this->mat[8] * this->mat[6]) * idet;
		ret[5]  =  (this->mat[0] * this->mat[10] - this->mat[8] * this->mat[2]) * idet;
		ret[6]  = -(this->mat[0] * this->mat[6]  - this->mat[4] * this->mat[2]) * idet;
		ret[7]  = 0.0;
		ret[8]  =  (this->mat[4] * this->mat[9] - this->mat[8] * this->mat[5]) * idet;
		ret[9]  = -(this->mat[0] * this->mat[9] - this->mat[8] * this->mat[1]) * idet;
		ret[10] =  (this->mat[0] * this->mat[5] - this->mat[4] * this->mat[1]) * idet;
		ret[11] = 0.0;
		ret[12] = -(this->mat[12] * (ret)[0] + this->mat[13] * (ret)[4] + this->mat[14] * (ret)[8]);
		ret[13] = -(this->mat[12] * (ret)[1] + this->mat[13] * (ret)[5] + this->mat[14] * (ret)[9]);
		ret[14] = -(this->mat[12] * (ret)[2] + this->mat[13] * (ret)[6] + this->mat[14] * (ret)[10]);
		ret[15] = 1.0;
//		return *ret;
	}
/*	
// MATINVERSE BY FRANKOI
void mat_inverse (float *in, float *out)
{
    float det, oneOverDet;

  det = in[0] * ((in[5] * in[10]) - (in[6] * in[9]))
    + in[1] * ((in[6] * in[8]) - (in[4] * in[10]))
    + in[2] * ((in[4] * in[9]) - (in[5] * in[8]));

  oneOverDet = 1.0f / det;

  out[0] = ((in[5] * in[10])- (in[6] * in[9])) * oneOverDet;
  out[1] = ((in[2] * in[9]) - (in[1] * in[10]))* oneOverDet;
  out[2] = ((in[1] * in[6]) - (in[2] * in[5])) * oneOverDet;
  out[3] = 0.0f;

  out[4] = ((in[6] * in[8]) - (in[4] * in[10]))* oneOverDet;
  out[5] = ((in[0] * in[10])- (in[2] * in[8])) * oneOverDet;
  out[6] = ((in[2] * in[4]) - (in[0] * in[6])) * oneOverDet;
  out[7] = 0.0f;

  out[8] = ((in[4] * in[9]) - (in[5] * in[8])) * oneOverDet;
  out[9] = ((in[1] * in[8]) - (in[0] * in[9])) * oneOverDet;
  out[10]= ((in[0] * in[5]) - (in[1] * in[4])) * oneOverDet;
  out[11]= 0.0f;

  out[12] = 0.0f;
  out[13] = 0.0f;
  out[14] = 0.0f;
  out[15] = 1.0f;
}
*/
	void zero(void) {
		this->mat[0] = 0.0; this->mat[4] = 0.0; this->mat[8] = 0.0; this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] = 0.0; this->mat[9] = 0.0; this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = 0.0; this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 0.0;
	}
	void identity(void) {
		this->mat[0] = 1.0; this->mat[4] = 0.0; this->mat[8]  = 0.0; this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] = 1.0; this->mat[9]  = 0.0; this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = 1.0; this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void rotate(const vec3 &axis,float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		vec3 v = axis;
		v.normalize();
		float xx = v.x * v.x;
		float yy = v.y * v.y;
		float zz = v.z * v.z;
		float xy = v.x * v.y;
		float yz = v.y * v.z;
		float zx = v.z * v.x;
		float xs = v.x * s;
		float ys = v.y * s;
		float zs = v.z * s;
		this->mat[0] = (1.0f - c) * xx + c;  this->mat[4] = (1.0f - c) * xy - zs; this->mat[8] = (1.0f - c) * zx + ys; this->mat[12] = 0.0;
		this->mat[1] = (1.0f - c) * xy + zs; this->mat[5] = (1.0f - c) * yy + c;  this->mat[9] = (1.0f - c) * yz - xs; this->mat[13] = 0.0;
		this->mat[2] = (1.0f - c) * zx - ys; this->mat[6] = (1.0f - c) * yz + xs; this->mat[10] = (1.0f - c) * zz + c; this->mat[14] = 0.0;
		this->mat[3] = 0.0;                  this->mat[7] = 0.0;                  this->mat[11] = 0.0;                 this->mat[15] = 1.0;
	}
	void rotate(float x,float y,float z,float angle) {
		rotate(vec3(x,y,z),angle);
	}
	void rotate_x(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		this->mat[0] = 1.0; this->mat[4] = 0.0; this->mat[8]  = 0.0; this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] =  c;  this->mat[9]  = -s;  this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] =  s;  this->mat[10] =  c;  this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void rotate_y(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		this->mat[0] =  c;  this->mat[4] = 0.0; this->mat[8]  =  s;  this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] = 1.0; this->mat[9]  = 0.0; this->mat[13] = 0.0;
		this->mat[2] = -s;  this->mat[6] = 0.0; this->mat[10] =  c;  this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void rotate_z(float angle) {
		DegToRad(angle);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		this->mat[0] =  c;  this->mat[4] = -s;  this->mat[8]  = 0.0; this->mat[12] = 0.0;
		this->mat[1] =  s;  this->mat[5] =  c;  this->mat[9]  = 0.0; this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = 1.0; this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void scale(const vec3 &v) {
		this->mat[0] = v.x; this->mat[4] = 0.0; this->mat[8]  = 0.0; this->mat[12] = 0.0;
		this->mat[1] = 0.0; this->mat[5] = v.y; this->mat[9]  = 0.0; this->mat[13] = 0.0;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = v.z; this->mat[14] = 0.0;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void scale(float x,float y,float z) {
		scale(vec3(x,y,z));
	}
	void translate(const vec3 &v) {
		this->mat[0] = 1.0; this->mat[4] = 0.0; this->mat[8]  = 0.0; this->mat[12] = v.x;
		this->mat[1] = 0.0; this->mat[5] = 1.0; this->mat[9]  = 0.0; this->mat[13] = v.y;
		this->mat[2] = 0.0; this->mat[6] = 0.0; this->mat[10] = 1.0; this->mat[14] = v.z;
		this->mat[3] = 0.0; this->mat[7] = 0.0; this->mat[11] = 0.0; this->mat[15] = 1.0;
	}
	void translate(float x,float y,float z) {
		translate(vec3(x,y,z));
	}
	void reflect(const vec4 &plane) {
		float x = plane.x;
		float y = plane.y;
		float z = plane.z;
		float x2 = x * 2.0f;
		float y2 = y * 2.0f;
		float z2 = z * 2.0f;
		this->mat[0] = 1.0f - x * x2; this->mat[4] = -y * x2;       this->mat[8] = -z * x2;        this->mat[12] = -plane.w * x2;
		this->mat[1] = -x * y2;       this->mat[5] = 1.0f - y * y2; this->mat[9] = -z * y2;        this->mat[13] = -plane.w * y2;
		this->mat[2] = -x * z2;       this->mat[6] = -y * z2;       this->mat[10] = 1.0f - z * z2; this->mat[14] = -plane.w * z2;
		this->mat[3] = 0.0;           this->mat[7] = 0.0;           this->mat[11] = 0.0;           this->mat[15] = 1.0;
	}
	void reflect(float x,float y,float z,float w) {
		reflect(vec4(x,y,z,w));
	}
	
	void perspective(float fov,float aspect,float znear,float zfar) {
		float y = (float)tan(fov * M_PI / 360.0f);
		float x = y * aspect;
		this->mat[0] = 1.0f / x; this->mat[4] = 0.0;      this->mat[8] = 0.0;                               this->mat[12] = 0.0;
		this->mat[1] = 0.0;      this->mat[5] = 1.0f / y; this->mat[9] = 0.0;                               this->mat[13] = 0.0;
		this->mat[2] = 0.0;      this->mat[6] = 0.0;      this->mat[10] = -(zfar + znear) / (zfar - znear); this->mat[14] = -(2.0f * zfar * znear) / (zfar - znear);
		this->mat[3] = 0.0;      this->mat[7] = 0.0;      this->mat[11] = -1.0;                             this->mat[15] = 0.0;
	}
	void look_at(const vec3 &eye,const vec3 &dir,const vec3 &up) {
		vec3 x,y,z;
		mat4 m0,m1;
		z = eye - dir;
		z.normalize();
		x.cross(up,z);
		x.normalize();
		y.cross(z,x);
		y.normalize();
		m0[0] = x.x; m0[4] = x.y; m0[8] = x.z; m0[12] = 0.0;
		m0[1] = y.x; m0[5] = y.y; m0[9] = y.z; m0[13] = 0.0;
		m0[2] = z.x; m0[6] = z.y; m0[10] = z.z; m0[14] = 0.0;
		m0[3] = 0.0; m0[7] = 0.0; m0[11] = 0.0; m0[15] = 1.0;
		m1.translate(-eye);
		*this = m0 * m1;
	}
	void look_at(const float *eye,const float *dir,const float *up) {
		look_at(vec3(eye),vec3(dir),vec3(up));
	}

	float mat[16];
};

inline mat3::mat3(const mat4 &m) {
	this->mat[0] = m[0]; this->mat[3] = m[4]; this->mat[6] = m[8];
	this->mat[1] = m[1]; this->mat[4] = m[5]; this->mat[7] = m[9];
	this->mat[2] = m[2]; this->mat[5] = m[6]; this->mat[8] = m[10];
}

/*****************************************************************************/
/*                                                                           */
/* quat                                                                      */
/*                                                                           */
/*****************************************************************************/

class quat {
public:
	quat(void) : x(0), y(0), z(0), w(1) { }
	quat(const vec3 &dir,float angle) {
		set(dir,angle);
	}
	quat(float _x,float _y,float _z,float angle) {
		set(_x,_y,_z,angle);
	}
	quat(const mat3 &m) {
		float trace = m[0] + m[4] + m[8];
		if(trace > 0.0) {
			float s = sqrtf(trace + 1.0f);
			this->q[3] = 0.5f * s;
			s = 0.5f / s;
			this->q[0] = (m[5] - m[7]) * s;
			this->q[1] = (m[6] - m[2]) * s;
			this->q[2] = (m[1] - m[3]) * s;
		} else {
			static int next[3] = { 1, 2, 0 };
			int i = 0;
			if(m[4] > m[0]) i = 1;
			if(m[8] > m[3 * i + i]) i = 2;
			int j = next[i];
			int k = next[j];
			float s = sqrtf(m[3 * i + i] - m[3 * j + j] - m[3 * k + k] + 1.0f);
			this->q[i] = 0.5f * s;
			if(s != 0) s = 0.5f / s;
			this->q[3] = (m[3 * j + k] - m[3 * k + j]) * s;
			this->q[j] = (m[3 * i + j] + m[3 * j + i]) * s;
			this->q[k] = (m[3 * i + k] + m[3 * k + i]) * s;
		}
	}
	quat& operator*(const quat &q) {
		quat *ret = new quat();
		ret->x = w * q.x + x * q.x + y * q.z - z * q.y;
		ret->y = w * q.y + y * q.w + z * q.x - x * q.z;
		ret->z = w * q.z + z * q.w + x * q.y - y * q.x;
		ret->w = w * q.w - x * q.x - y * q.y - z * q.z;
		return *ret;
	}
	void set(const vec3 &dir,float angle) {
		float l = dir.length();
		if(l != 0.0) {
			l = 1.0f / l;
			DegToRad(angle);
			l *= (float)sin(angle * 0.5f);
			this->x = dir[0] * l;
			this->y = dir[1] * l;
			this->z = dir[2] * l;
			this->w = (float)cos(angle * 0.5f);
		} else {
			this->x = this->y = this->z = 0.0;
			this->w = 1.0;
		}
	}
	void set(float _x,float _y,float _z,float angle) {
		set(vec3(_x,_y,_z),angle);
	}
	void reset(void) { this->x = this->y = this->z = 0; this->w = 1; }
	void slerp(const quat &q0,const quat &q1,float t) {
		float k0,k1,cosomega = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
		quat q;
		if(cosomega < 0.0) {
			cosomega = -cosomega;
			q.x = -q1.x;
			q.y = -q1.y;
			q.z = -q1.z;
			q.w = -q1.w;
		} else {
			q.x = q1.x;
			q.y = q1.y;
			q.z = q1.z;
			q.w = q1.w;
		}
		if(1.0 - cosomega > 1e-6) {
			float omega = (float)acos(cosomega);
			float sinomega = (float)sin(omega);
			k0 = (float)sin((1.0f - t) * omega) / sinomega;
			k1 = (float)sin(t * omega) / sinomega;
		} else {
			k0 = 1.0f - t;
			k1 = t;
		}
		this->x = q0.x * k0 + q.x * k1;
		this->y = q0.y * k0 + q.y * k1;
		this->z = q0.z * k0 + q.z * k1;
		this->w = q0.w * k0 + q.w * k1;
	}

	mat3 to_matrix(void) const {
		float x2 = this->x + this->x;
		float y2 = this->y + this->y;
		float z2 = this->z + this->z;
		float xx = this->x * x2;
		float yy = this->y * y2;
		float zz = this->z * z2;
		float xy = this->x * y2;
		float yz = this->y * z2;
		float xz = this->z * x2;
		float wx = this->w * x2;
		float wy = this->w * y2;
		float wz = this->w * z2;
		return mat3(1.0f - (yy + zz), xy + wz, xz - wy,
					xy - wz, 1.0f - (xx + zz), yz + wx,
					xz + wy, yz - wx, 1.0f - (xx + yy));
	}

	operator float*() { return this->q; }
	operator const float*() const { return this->q; }
//	float &operator[](int _i) { return this->q[_i]; }
//	const float &operator[](int _i) const { return this->q[_i]; }

	union {
		struct {float x,y,z,w;};
		float q[4];
	};
};

/*****************************************************************************/
/*                                                                           */
/* ivec2                                                                     */
/*                                                                           */
/*****************************************************************************/

class ivec2 {
public:
	ivec2(void) : a(0), b(0) { }
	ivec2(long _a,long _b) : a(_a), b(_b) { }
	ivec2(const long *iv) : a(iv[0]), b(iv[1]) { }
	ivec2(const ivec2 &iv) : a(iv.a), b(iv.b) { }

	int operator==(const ivec2 &iv) { return ((this->a == iv.a) && (this->b == iv.b)); }
	int operator!=(const ivec2 &iv) { return !(*this == iv); }

	ivec2 &operator=(long _i) { this->x=_i; this->y=_i; return (*this); }
	const ivec2 operator*(long _i) const { return ivec2(this->a * _i,this->b * _i); }
	const ivec2 operator/(long _i) const { return ivec2(this->a / _i,this->b / _i); }
	const ivec2 operator+(const ivec2 &iv) const { return ivec2(this->a + iv.a,this->b + iv.b); }
	const ivec2 operator-() const { return ivec2(-this->a,-this->b); }
	const ivec2 operator-(const ivec2 &iv) const { return ivec2(this->a - iv.a,this->b - iv.b); }

	ivec2 &operator*=(long _i) { return *this = *this * _i; }
	ivec2 &operator/=(long _i) { return *this = *this / _i; }
	ivec2 &operator+=(const ivec2 &iv) { return *this = *this + iv; }
	ivec2 &operator-=(const ivec2 &iv) { return *this = *this - iv; }

	long operator*(const ivec2 &iv) const { return this->a * iv.a + this->b * iv.b; }

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
//	long &operator[](int _i) { return this->i[_i]; }
//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b) { this->a = _a; this->b = _b; }
	void reset(void) { this->a = this->b = 0; }
	void swap(ivec2 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; }
	void swap(ivec2 *iv) { this->swap(*iv); }

	union {
		struct {long a,b;};
		struct {long x,y;};
		long i[2];
	};
};

/*****************************************************************************/
/*                                                                           */
/* ivec3                                                                     */
/*                                                                           */
/*****************************************************************************/

class ivec3 {
public:
	ivec3(void) : a(0), b(0), c(0) { }
	ivec3(long _a,long _b,long _c) : a(_a), b(_b), c(_c) { }
	ivec3(const long *iv) : a(iv[0]), b(iv[1]), c(iv[2]) { }
	ivec3(const ivec3 &iv) : a(iv.a), b(iv.b), c(iv.c) { }
	ivec3(const ivec4 &iv);

	int operator==(const ivec3 &iv) { return ((this->a == iv.a) && (this->b == iv.b) && (this->c == iv.c)); }
	int operator!=(const ivec3 &iv) { return !(*this == iv); }

	ivec3 &operator=(long _i) { this->x=_i; this->y=_i; this->z=_i; return (*this); }
	const ivec3 operator*(long _i) const { return ivec3(this->a * _i,this->b * _i,this->c * _i); }
	const ivec3 operator/(long _i) const { return ivec3(this->a / _i,this->b / _i,this->c / _i); }
	const ivec3 operator+(const ivec3 &iv) const { return ivec3(this->a + iv.a,this->b + iv.b,this->c + iv.c); }
	const ivec3 operator-() const { return ivec3(-this->a,-this->b,-this->c); }
	const ivec3 operator-(const ivec3 &iv) const { return ivec3(this->a - iv.a,this->b - iv.b,this->c - iv.c); }

	ivec3 &operator*=(long _i) { return *this = *this * _i; }
	ivec3 &operator/=(long _i) { return *this = *this / _i; }
	ivec3 &operator+=(const ivec3 &iv) { return *this = *this + iv; }
	ivec3 &operator-=(const ivec3 &iv) { return *this = *this - iv; }

	long operator*(const ivec3 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c; }
	long operator*(const ivec4 &iv) const;

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
//	long &operator[](int _i) { return this->i[_i]; }
//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b,long _c) { this->a = _a; this->b = _b; this->c = _c; }
	void reset(void) { this->a = this->b = this->c = 0; }
	void swap(ivec3 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; tmp=c; c=iv.c; iv.c=tmp; }
	void swap(ivec3 *iv) { this->swap(*iv); }

	union {
		struct {long a,b,c;};
		struct {long x,y,z;};
		struct {long red,green,blue;};
		long i[3];
	};
};

/*****************************************************************************/
/*                                                                           */
/* ivec4                                                                     */
/*                                                                           */
/*****************************************************************************/

class ivec4 {
public:
	ivec4(void) : a(0), b(0), c(0), d(1) { }
	ivec4(long _a,long _b,long _c,long _d) : a(_a), b(_b), c(_c), d(_d) { }
	ivec4(const long *iv) : a(iv[0]), b(iv[1]), c(iv[2]), d(iv[3]) { }
	ivec4(const ivec3 &iv) : a(iv.a), b(iv.b), c(iv.c), d(1) { }
	ivec4(const ivec3 &iv,long _d) : a(iv.a), b(iv.b), c(iv.c), d(_d) { }
	ivec4(const ivec4 &iv) : a(iv.a), b(iv.b), c(iv.c), d(iv.d) { }

	int operator==(const ivec4 &iv) { return ((this->a == iv.a) && (this->b == iv.b) && (this->c == iv.c) && (this->d == iv.d)); }
	int operator!=(const ivec4 &iv) { return !(*this == iv); }

	ivec4 &operator=(long _i) { this->x=_i; this->y=_i; this->z=_i; this->w=_i; return (*this); }
	const ivec4 operator*(long _i) const { return ivec4(this->a * _i,this->b * _i,this->c * _i,this->d * _i); }
	const ivec4 operator/(long _i) const { return ivec4(this->a / _i,this->b / _i,this->c / _i,this->d / _i); }
	const ivec4 operator+(const ivec4 &iv) const { return ivec4(this->a + iv.a,this->b + iv.b,this->c + iv.c,this->d + iv.d); }
	const ivec4 operator-() const { return ivec4(-this->a,-this->b,-this->c,-this->d); }
	const ivec4 operator-(const ivec4 &iv) const { return ivec4(this->a - iv.a,this->b - iv.b,this->c - iv.c,this->d - iv.d); }

	ivec4 &operator*=(long _i) { return *this = *this * _i; }
	ivec4 &operator/=(long _i) { return *this = *this / _i; }
	ivec4 &operator+=(const ivec4 &iv) { return *this = *this + iv; }
	ivec4 &operator-=(const ivec4 &iv) { return *this = *this - iv; }

	long operator*(const ivec3 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c + this->d; }
	long operator*(const ivec4 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c + this->d * iv.d; }

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
//	long &operator[](int _i) { return this->i[_i]; }
//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b,long _c,long _d) { this->a = _a; this->b = _b; this->c = _c; this->d = _d; }
	void reset(void) { this->a = this->b = this->c = this->d = 0; }
	void swap(ivec4 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; tmp=c; c=iv.c; iv.c=tmp; tmp=d; d=iv.d; iv.d=tmp; }
	void swap(ivec4 *iv) { this->swap(*iv); }

	union {
		struct {long a,b,c,d;};
		struct {long x,y,z,w;};
		struct {long red,green,blue,alpha;};
		long i[4];
	};
};

inline ivec3::ivec3(const ivec4 &iv) {
	this->a = iv.a;
	this->b = iv.b;
	this->c = iv.c;
}

inline long ivec3::operator*(const ivec4 &iv) const {
	return this->a * iv.a + this->b * iv.b + this->c * iv.c + iv.d;
}

#endif // __HMATHLIB__

