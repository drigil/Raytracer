#include "world.h"
#include <iostream>
using namespace std;

#define MAX_LIMIT 3

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
				//if(shadowRay.didHit()){
				if(shadowRay.didHit() && (shadowRay.intersected()->getMaterial()->matType==0|| shadowRay.intersected()->getMaterial()->matType==1)){
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
				Color newColor =  Color(-0.01) + World::shade_ray(reflectedRay);

				//std::cout << ray.getPosition().X() << " " << ray.getPosition().Y()  << " " << ray.getPosition().Z() << " " <<std::endl;
				// newColor.r = newColor.r + 0.1;
				// newColor.g = newColor.g + 0.1;
				// newColor.b = newColor.b + 0.1;
				
				return newColor;
		}

		//Dielectric with beers law
		else if(ray.intersected()->getMaterial()->matType==2){
			Vector3D reflectedDirection = (2 * dotProduct(ray.getDirection(), ray.getNormal()) * ray.getNormal()) - (ray.getDirection());
			Ray reflectedRay = Ray(ray.getPosition(), -reflectedDirection);
			reflectedRay.setLevel(ray.getLevel() + 1);
			Color reflectedColor = World::shade_ray(reflectedRay);

			Vector3D normal = ray.getNormal();
			Vector3D direction = -ray.getDirection();
			double eta = ray.intersected()->getMaterial()->eta;
			double dotVal = dotProduct(direction, normal);

			//double det = (1 - (((ray.intersected()->getMaterial()->eta)*(ray.intersected()->getMaterial()->eta)) * (1 - ((dotProduct(ray.getDirection(), ray.getNormal())) * (dotProduct(ray.getDirection(), ray.getNormal()))))));
			double kr = 1;
			double kg = 1;
			double kb = 1;

			//Ray refractedRay;
			Color refractedColor = Color(0);
			
			if(dotVal < 0){

				//dotVal = -dotVal;
				double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));
				Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));
				Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
				refractedRay.setLevel(ray.getLevel() + 1);
				refractedColor = World::shade_ray(refractedRay);	
				dotVal = -dotVal;

			}

			else{
				normal = -normal;
				eta = 1/eta;
				double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));

				//Applying Beer's Law
				double ar = 0.2;
				double ag = 0.2;
				double ab = 0.2;
				double distanceTravelled = (ray.getOrigin() - ray.getPosition()).length();

				kr = exp(-ar * distanceTravelled);
				kg = exp(-ag * distanceTravelled);
				kb = exp(-ab * distanceTravelled);

				if(det > 0){

					Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));
					Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
					refractedRay.setLevel(ray.getLevel() + 1);
					refractedColor = World::shade_ray(refractedRay);
					//dotVal = dotProduct(-refractedDirection, normal);
					
				}

				else{
					//Color newColor =  (ray.intersected()->getMaterial()->kr *  World::shade_ray(reflectedRay));
					//Color newColor =  World::shade_ray(reflectedRay);
					reflectedColor.r = reflectedColor.r * kr;
					reflectedColor.g = reflectedColor.g * kg;
					reflectedColor.b = reflectedColor.b * kb;
					
					return reflectedColor;
				}

			}


			//eta = 1/eta;
			double Ro = ((eta - 1)*(eta - 1)) / ((eta + 1)*(eta + 1));
			if(eta > 1){
				if(Ro > 1){
					reflectedColor.r = reflectedColor.r * kr;
					reflectedColor.g = reflectedColor.g * kg;
					reflectedColor.b = reflectedColor.b * kb;
					return reflectedColor;
				}
				dotVal = sqrt(1 - Ro);	
			}
			
			double R = Ro + ((1 - Ro)*pow(1-dotVal, 5));
			//std::cout << 1-R << std::endl;
			
			double k = ray.intersected()->getMaterial()->kr;

			//Using Beer's Law
			reflectedColor.r = reflectedColor.r * kr * R;
			reflectedColor.g = reflectedColor.g * kb * R;
			reflectedColor.b = reflectedColor.b * kg * R;

			//std::cout << reflectedColor.r << " " << reflectedColor.g << " " << reflectedColor.b;


			// //Using Beer's Law
			refractedColor.r = refractedColor.r * kr * (1 - R);
			refractedColor.g = refractedColor.g * kg * (1 - R);
			refractedColor.b = refractedColor.b * kb * (1 - R);
			//std::cout << kr << " " << kg << " " << kb << std::endl;

			Color newColor =  refractedColor + reflectedColor;
			return refractedColor + reflectedColor;
			
		}


		// else if(ray.intersected()->getMaterial()->matType == 3){

		// 	Vector3D reflectedDirection = (2 * dotProduct(ray.getDirection(), ray.getNormal()) * ray.getNormal()) - (ray.getDirection());
		// 	Ray reflectedRay = Ray(ray.getPosition(), -reflectedDirection);
		// 	reflectedRay.setLevel(ray.getLevel() + 1);

		// 	Vector3D normal = ray.getNormal();
		// 	Vector3D direction = -ray.getDirection();
		// 	double eta = ray.intersected()->getMaterial()->eta;
		// 	double dotVal = dotProduct(direction, normal);

		// 	//double det = (1 - (((ray.intersected()->getMaterial()->eta)*(ray.intersected()->getMaterial()->eta)) * (1 - ((dotProduct(ray.getDirection(), ray.getNormal())) * (dotProduct(ray.getDirection(), ray.getNormal()))))));
			
		// 	if(dotVal < 0){

		// 		//dotVal = -dotVal;
		// 	}

		// 	else{
		// 		normal = -normal;
		// 		eta = 1/eta;
		// 	}

		// 	double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));
		// 	Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));

		// 	if(det > 0){
				
		// 		Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
		// 		refractedRay.setLevel(ray.getLevel() + 1);		

		// 		Color reflectedColor = World::shade_ray(reflectedRay);

		// 		//Using input coefficients instead
		// 		double reflCoeff = ray.intersected()->getMaterial()->kr;
		// 		reflectedColor.r = reflectedColor.r * reflCoeff;
		// 		reflectedColor.g = reflectedColor.g * reflCoeff;
		// 		reflectedColor.b = reflectedColor.b * reflCoeff;

		// 		Color refractedColor = World::shade_ray(refractedRay);

		// 		//Using input coefficients instead
		// 		double refrCoeff = ray.intersected()->getMaterial()->kt;
		// 		refractedColor.r = refractedColor.r * refrCoeff;
		// 		refractedColor.g = refractedColor.g * refrCoeff;
		// 		refractedColor.b = refractedColor.b * refrCoeff;


		// 		Color newColor =  refractedColor + reflectedColor + Color(-0.005);
		// 		//std::cout << newColor.r << " " << newColor.g << " " << newColor.b << std::endl;
		// 		return newColor;
		// 	}

		// 	else{
		// 		Color newColor =  World::shade_ray(reflectedRay) + Color(-0.007);
		// 		return newColor;
		// 	}
		// }


		//Dielectric without Beers Law and schlicks apprximation
		else if(ray.intersected()->getMaterial()->matType == 3){
			Vector3D reflectedDirection = (2 * dotProduct(ray.getDirection(), ray.getNormal()) * ray.getNormal()) - (ray.getDirection());
			Ray reflectedRay = Ray(ray.getPosition(), -reflectedDirection);
			reflectedRay.setLevel(ray.getLevel() + 1);
			Color reflectedColor = World::shade_ray(reflectedRay);

			Vector3D normal = ray.getNormal();
			Vector3D direction = -ray.getDirection();
			double eta = ray.intersected()->getMaterial()->eta;
			double dotVal = dotProduct(direction, normal);

			//double det = (1 - (((ray.intersected()->getMaterial()->eta)*(ray.intersected()->getMaterial()->eta)) * (1 - ((dotProduct(ray.getDirection(), ray.getNormal())) * (dotProduct(ray.getDirection(), ray.getNormal()))))));
			

			//Ray refractedRay;
			Color refractedColor = Color(0);
			
			if(dotVal < 0){

				//dotVal = -dotVal;
				//eta = 1/eta;
				double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));
				Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));
				Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
				refractedRay.setLevel(ray.getLevel() + 1);
				refractedColor = World::shade_ray(refractedRay);	
				//dotVal = -dotVal;

			}

			else{
				normal = -normal;
				eta = 1/eta;
				double det = 1 - ((eta*eta) * (1 - (dotVal * dotVal)));

				if(det > 0){

					Vector3D refractedDirection = (eta * (direction - (normal*(dotVal)))) - (normal * sqrt(det));
					Ray refractedRay = Ray(ray.getPosition(), -refractedDirection);	
					refractedRay.setLevel(ray.getLevel() + 1);
					refractedColor = World::shade_ray(refractedRay);
					
				}

				else{
					//Color newColor =  (ray.intersected()->getMaterial()->kr *  World::shade_ray(reflectedRay));
					//Color newColor =  World::shade_ray(reflectedRay);
					double reflCoeff = ray.intersected()->getMaterial()->kr;
					reflectedColor = reflectedColor * (float)reflCoeff;
					return reflectedColor + Color(-0.02);
				}

			}
			
			double reflCoeff = ray.intersected()->getMaterial()->kr;
			reflectedColor = reflectedColor * (float)reflCoeff;

			double refrCoeff = ray.intersected()->getMaterial()->kt;
			refractedColor = refractedColor * (float)refrCoeff;

			Color newColor =  reflectedColor + refractedColor + Color(-0.01);
			return newColor;
			
		}

	return background;
}
