#include "world.h"
#include <iostream>
using namespace std;

#define MAX_LIMIT 5

float World::firstIntersection(Ray& ray)
{
	for(int i=0; i<objectList.size(); i++)
		objectList[i]->intersect(ray);
	return ray.getParameter();
}

Color World::shade_ray(Ray& ray)
{	

	if(ray.getLevel() >= MAX_LIMIT){
		return background;
	}

	firstIntersection(ray);
	if(ray.didHit())

		//Diffuse
		if(ray.intersected()->getMaterial()->matType==0){
			Color diffuseColor = Color(0);
			for(LightSource* light: lightSourceList){
				Ray shadowRay(ray.getPosition(), light->getPosition() - ray.getPosition());
				firstIntersection(shadowRay);
				//Check if object intersected not dielectric
				if(shadowRay.didHit() && shadowRay.intersected()->getMaterial()->matType!=2){
					diffuseColor = diffuseColor + (ambient * shadowRay.intersected()->getMaterial()->ka);
				}
				else{
					diffuseColor = diffuseColor + (ray.intersected())->shade(ray);		
				}
			}

			return diffuseColor;
		}

		//Reflected 
		else if(ray.intersected()->getMaterial()->matType==1){

			
				Vector3D reflectedDirection = (2 * dotProduct(ray.getDirection(), ray.getNormal()) * ray.getNormal()) - (ray.getDirection());
				Ray reflectedRay = Ray(ray.getPosition(), -reflectedDirection);
				reflectedRay.setLevel(ray.getLevel() + 1);

				//std::cout << ray.getPosition().X() << " " << ray.getPosition().Y()  << " " << ray.getPosition().Z() << " " <<std::endl;
				Color newColor =  World::shade_ray(reflectedRay);
				// newColor.r = newColor.r + 0.1;
				// newColor.g = newColor.g + 0.1;
				// newColor.b = newColor.b + 0.1;
				
				return newColor;
		}

		//Dielectric
		else if(ray.intersected()->getMaterial()->matType==2){
			Vector3D reflectedDirection = (2 * dotProduct(ray.getDirection(), ray.getNormal()) * ray.getNormal()) - (ray.getDirection());
			Ray reflectedRay = Ray(ray.getPosition(), -reflectedDirection);
			reflectedRay.setLevel(ray.getLevel() + 1);

			Vector3D normal = ray.getNormal();
			Vector3D direction = -ray.getDirection();
			double eta = ray.intersected()->getMaterial()->eta;
			double dotVal = dotProduct(direction, normal);

			//double det = (1 - (((ray.intersected()->getMaterial()->eta)*(ray.intersected()->getMaterial()->eta)) * (1 - ((dotProduct(ray.getDirection(), ray.getNormal())) * (dotProduct(ray.getDirection(), ray.getNormal()))))));
			double kr = 1;
			double kg = 1;
			double kb = 1;
			
			if(dotVal < 0){

				dotVal = -dotVal;
			}

			else{
				normal = -normal;
				eta = 1/eta;
			}

			double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));
			Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));

			if(det > 0){
				//Vector3D refractedDirection = (ray.intersected()->getMaterial()->eta * (-ray.getDirection() - ray.getNormal()*(dotProduct(-ray.getDirection(), ray.getNormal())))) - (ray.getNormal() * sqrt(det));
				//Vector3D refractedDirection = (ray.intersected()->getMaterial()->eta * (ray.getDirection() - ray.getNormal()*(dotProduct(ray.getDirection(), ray.getNormal())))) - (ray.getNormal() * sqrt(det));
				//Vector3D refractedDirection = (((ray.intersected()->getMaterial()->eta * (dotProduct(ray.getDirection(), ray.getNormal()))) - sqrt(det)) * ray.getNormal()) - (ray.intersected()->getMaterial()->eta * ray.getDirection());
				
				Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
				refractedRay.setLevel(ray.getLevel() + 1);		
				
				//Applying Beer's Law
				double attenuationCoeff = 1;
				double ar = 1;
				double ag = 1;
				double ab = 1;
				double distanceTravelled = (ray.getOrigin() - ray.getPosition()).length();

				kr = exp(-ar * distanceTravelled);
				kg = exp(-ag * distanceTravelled);
				kb = exp(-ab * distanceTravelled);

				Color reflectedColor = World::shade_ray(reflectedRay);

				//Using input coefficients instead
				double reflCoeff = ray.intersected()->getMaterial()->kr;
				reflectedColor.r = reflectedColor.r * reflCoeff;
				reflectedColor.g = reflectedColor.g * reflCoeff;
				reflectedColor.b = reflectedColor.b * reflCoeff;

				Color refractedColor = World::shade_ray(refractedRay);

				//Using input coefficients instead
				double refrCoeff = ray.intersected()->getMaterial()->kt;
				refractedColor.r = refractedColor.r * refrCoeff;
				refractedColor.g = refractedColor.g * refrCoeff;
				refractedColor.b = refractedColor.b * refrCoeff;

				//Applying Schlick's Approximation
				// double Ro = ((eta - 1)*(eta - 1)) / ((eta + 1)*(eta + 1));
				// double R = Ro + ((1 - Ro)*pow(1-dotVal, 5));

				// //Using Beer's Law
				// reflectedColor.r = reflectedColor.r * kr * R;
				// reflectedColor.g = reflectedColor.g * kb * R;
				// reflectedColor.b = reflectedColor.b * kg * R;


				// //Using Beer's Law
				// refractedColor.r = refractedColor.r * kr * (1 - R);
				// refractedColor.g = refractedColor.g * kg * (1 - R);
				// refractedColor.b = refractedColor.b * kb * (1 - R);


				Color newColor =  refractedColor + reflectedColor;
				return newColor;
			}

			else{
				Color newColor =  (ray.intersected()->getMaterial()->kr *  World::shade_ray(reflectedRay));
				return newColor;
			}

			
		}
		

	return background;
}
