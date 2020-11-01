//object.h
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ray.h"
#include "vector3D.h"
#include "color.h"
#include "material.h"
#include "world.h"
#include "lightsource.h"

class Object
{
protected:
	Material *material;
	World *world;
	bool isSolid;
public:
	Object(Material *mat, World *wrld): material(mat), world(wrld) {}
	virtual bool intersect(Ray& ray) const = 0;
	virtual Color shade(const Ray& ray) const
	{
		return material->shade(ray, isSolid);
	}
	Material* getMaterial() const {return material;}
};

#endif
