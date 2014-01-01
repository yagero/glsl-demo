#ifndef BOUNDINGBOX
#define BOUNDINGBOX

#include "Mathlib.h"

class BoundingBox
{
public:
	BoundingBox() {min.reset(); max.reset();}
	BoundingBox(const vec3& _min, const vec3& _max) {min=_min; max=_max;}

	inline bool ContainsPoint(const vec3& point) const	{
		return (point.x>=min.x && point.y>=min.y && point.z>=min.z && point.x<=max.x && point.y<=max.y && point.z<=max.z);
	};

	inline void Add(const vec3& v)	{
		if(v.x > max.x)	max.x = v.x;
		if(v.x < min.x)	min.x = v.x;
		if(v.y > max.y)	max.y = v.y;
		if(v.y < min.y)	min.y = v.y;
		if(v.z > max.z)	max.z = v.z;
		if(v.z < min.z)	min.z = v.z;
	};

	inline void Add(const BoundingBox& bb)	{
		if(bb.max.x > max.x)	max.x = bb.max.x;
		if(bb.min.x < min.x)	min.x = bb.min.x;
		if(bb.max.y > max.y)	max.y = bb.max.y;
		if(bb.min.y < min.y)	min.y = bb.min.y;
		if(bb.max.z > max.z)	max.z = bb.max.z;
		if(bb.min.z < min.z)	min.z = bb.min.z;
	}

	inline void Translate(const vec3& v) {
		min += v;
		max += v;
	}

	inline vec3	getCenter()	const	{return (max+min)/2;}

	vec3 min, max;
};

#endif


