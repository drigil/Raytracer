//sphere.h
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Triangle : public Object
{
private:
	Vector3D vertex1;
	Vector3D vertex2;
	Vector3D vertex3;
		

public:
	Triangle(const Vector3D& _vert1, const Vector3D& _vert2, const Vector3D& _vert3, Material* mat, World* wrld):

		Object(mat, wrld), vertex1(_vert1), vertex2(_vert2), vertex3(_vert3) 
		{
			isSolid = true;
		}
		
	virtual bool intersect(Ray& r) const;
	virtual Color shade(const Ray& ray) const;
};
#endif
