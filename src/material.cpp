#include "world.h"
#include "material.h"
#include <iostream>
using namespace std;

Color Material::shade(const Ray& incident, const bool isSolid) const
{	

	//Ambient shading
	// Color ambientColor = color * ka;

	// //Diffusion shading
	// Vector3D normal = incident.getNormal() * (-1);
	// //Check how do we get normal
	// // std::cout << normal.X() <<std::endl;
	// // std::cout << normal.Y() <<std::endl;
	// // std::cout << normal.Z() <<std::endl;
	
		
	// normal.normalize();
	// Vector3D givenRay = incident.getDirection();
	// givenRay.normalize();

	// Color diffuseColor = color;
	// double dotResult = dotProduct(normal, givenRay);
	
	// if(dotResult >= 0){
	// 	double coeff = kd * dotResult;
	// 	diffuseColor = coeff * color;

	// }
	// else{
	// 	diffuseColor = color * 0;
	// }

	// return diffuseColor;
	//return Color(red, green, blue);

	return color;
	

}
