//sphere.cpp

#include "sphere.h"
#include <vector>
#include <iostream>
using namespace std;


bool Sphere::intersect(Ray& r) const
{
	Vector3D centerVector = r.getOrigin() - position;
	double a = 1.0;
	double b = 2*dotProduct(r.getDirection(), centerVector);
	double c = dotProduct(centerVector, centerVector) - radius*radius;
	double discriminant = b*b - 4.0*a*c;
	

	//now check if discr. is posivtive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{

		if(discriminant == 0)
		{
			double t;
			t = -b/(2.0*a);
			r.setParameter(t, this);
			
			Vector3D normal = position - r.getPosition();
			normal.normalize();
			r.setNormal(normal);
			
			return true;
		}
		else
		{
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			bool b1 = r.setParameter(t1, this);
			bool b2 = r.setParameter(t2, this);

			if(b1||b2 == true){				
				
				Vector3D normal = position - r.getPosition();
				normal.normalize();
				r.setNormal(normal);
			}

			return b1||b2;    
		}
	}
	return false;

}

//Overriding the shading method
Color Sphere::shade(const Ray& ray) const{
	Color color = material->shade(ray, isSolid);
	
	//ambient shading
	Color ambientColor = material->ka * world->getAmbient();
	

	//Lambertian shading and specular shading over list of light sources

	//Getting normal at the point
	Vector3D normal = position - ray.getPosition();
	normal.normalize();

	//Iterating over each light source
	//Getting vector from light source to the point
	std::vector<LightSource*> lightSourceList = world->getLightSourceList();
	
	Color diffuseColor = Color(0.0);
	Color specularColor = Color(0.0);

	for(LightSource* light: lightSourceList){

		//Lambertian shading
		Vector3D lightRay = ray.getPosition() - light->getPosition();
		lightRay.normalize();

		double dotResult = dotProduct(normal, lightRay);

		if(dotResult >= 0){
			double coeff = material->kd * dotResult;
			diffuseColor = diffuseColor + (coeff * light->getIntensity());

		}

		//Specular shading
		Vector3D halfwayVector = ray.getDirection() + lightRay;
		halfwayVector = halfwayVector / halfwayVector.length();

		double halfwayDot = dotProduct(normal, halfwayVector);
		halfwayDot = pow(halfwayDot, material->n);
		if(halfwayDot >= 0){
			specularColor = specularColor + (material->ks * world->getSpecular() * halfwayDot);
		}

	}

	
	//Specular shading
	

	return ambientColor + diffuseColor + specularColor;
}