//triangle.cpp

#include "triangle.h"
#include <iostream>
using namespace std;	

double getDeterminant(double A[3][3]){
	return (A[0][0] * ((A[1][1] * A[2][2]) - (A[1][2]*A[2][1]))) - (A[0][1] * ((A[1][0] * A[2][2]) - (A[2][0]*A[1][2]))) + (A[0][2] * ((A[1][0] * A[2][1]) - (A[2][0]*A[1][1])));	
}

bool Triangle::intersect(Ray& r) const
{
	Vector3D rayOrigin = r.getOrigin();
	Vector3D rayDirection = r.getDirection();
	double A[3][3] = {{vertex1.X() - vertex2.X(), vertex1.X() - vertex3.X(), rayDirection.X()}, 
					  {vertex1.Y() - vertex2.Y(), vertex1.Y() - vertex3.Y(), rayDirection.Y()},
					  {vertex1.Z() - vertex2.Z(), vertex1.Z() - vertex3.Z(), rayDirection.Z()}};
	double detA = getDeterminant(A);

	Vector3D sideOne = vertex2 - vertex1;
	Vector3D sideTwo = vertex3 - vertex1;
	

	//now check if discr. is non zero
	if(detA !=0)
	{

		double betaMatrix[3][3] = {{vertex1.X() - rayOrigin.X(), vertex1.X() - vertex3.X(), rayDirection.X()},
								   {vertex1.Y() - rayOrigin.Y(), vertex1.Y() - vertex3.Y(), rayDirection.Y()},
								   {vertex1.Z() - rayOrigin.Z(), vertex1.Z() - vertex3.Z(), rayDirection.Z()}};


		double gammaMatrix[3][3] = {{vertex1.X() - vertex2.X(), vertex1.X() - rayOrigin.X(), rayDirection.X()},
									{vertex1.Y() - vertex2.Y(), vertex1.Y() - rayOrigin.Y(), rayDirection.Y()},
									{vertex1.Z() - vertex2.Z(), vertex1.Z() - rayOrigin.Z(), rayDirection.Z()}};

		
		double tMatrix[3][3] = {{vertex1.X() - vertex2.X(), vertex1.X() - vertex3.X(), vertex1.X() - rayOrigin.X()},
								{vertex1.Y() - vertex2.Y(), vertex1.Y() - vertex3.Y(), vertex1.Y() - rayOrigin.Y()},
								{vertex1.Z() - vertex2.Z(), vertex1.Z() - vertex3.Z(), vertex1.Z() - rayOrigin.Z()}};

		double beta = getDeterminant(betaMatrix) / detA;
		double gamma = getDeterminant(gammaMatrix) / detA;
		double t = getDeterminant(tMatrix) / detA;

		// std::cout << beta << std::endl;
		// std::cout << gamma << std::endl;
		// std::cout << t << std::endl;


		if(beta > 0 && gamma > 0 && beta + gamma < 1){
			r.setParameter(t, this);
			Vector3D normal = crossProduct(sideTwo, sideOne);

			normal.normalize();
			
			r.setNormal(normal);

			return true;
		}
			
		else{
			return false;
		}
	
	}

	return false;
}


//Overriding the shading method
Color Triangle::shade(const Ray& ray) const{
	Color color = material->shade(ray, isSolid);

	//ambient shading
	Color ambientColor = material->ka * world->getAmbient();
	

	//Lambertian shading and specular shading over list of light sources

	//Getting normal at the point
	Vector3D sideOne = vertex2 - vertex1;
	Vector3D sideTwo = vertex3 - vertex1;
	
	//Vector3D normal = crossProduct(sideOne, sideTwo);
	Vector3D normal = crossProduct(sideTwo, sideOne);
	
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
			// std::cout<<diffuseColor.R()<<std::endl;
			// std::cout<<diffuseColor.G()<<std::endl;
			// std::cout<<diffuseColor.B()<<std::endl;


		}

		//Specular shading
		Vector3D halfwayVector = ray.getDirection() + lightRay;
		halfwayVector = halfwayVector / halfwayVector.length();

		double halfwayDot = dotProduct(normal, halfwayVector);
		halfwayDot = pow(halfwayDot, material->n);
		if(halfwayDot >= 0){
			specularColor = specularColor + (material->ks * world->getSpecular() * halfwayDot);
			// std::cout<<specularColor.R()<<std::endl;
			// std::cout<<specularColor.G()<<std::endl;
			// std::cout<<specularColor.B()<<std::endl;
		}

	}

	
	//Specular shading
	

	return ambientColor + diffuseColor + specularColor;
}