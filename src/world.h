#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include "object.h"
#include "lightsource.h"
#include "color.h"
#include "ray.h"
#include "vector3D.h"

class World
{
private:
	std::vector<Object*> objectList;
	std::vector<LightSource*> lightSourceList;

	Color ambient;
	Color specular;
	Color background; //Background color to shade rays that miss all objects

public:
	World():
		objectList(0), lightSourceList(0), ambient(0), background(0), specular(0)
	{}
	void setBackground(const Color& bk) { background = bk;}
	Color getBackground() { return background;}
	void setAmbient(const Color& amb) {ambient = amb;}
	Color getAmbient() {return ambient;}
	void setSpecular(const Color& spc) {specular = spc;}
	Color getSpecular() {return specular;}
	void addLight(LightSource* ls)
	{
		lightSourceList.push_back(ls);
	}
	std::vector<LightSource*> getLightSourceList(){return lightSourceList;}
	void addObject(Object *obj)
	{
		objectList.push_back(obj);
	}
	float firstIntersection(Ray& ray);
	Color shade_ray(Ray& ray);
};
#endif
