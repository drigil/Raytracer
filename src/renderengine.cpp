#include "renderengine.h"


#define NUM_SAMPLES 32

const Color RenderEngine::trace(const int i, const int j)
{
	Vector3D ray_dir = camera->get_ray_direction(i, j);
	Ray ray(camera->get_position(), ray_dir);
	return world->shade_ray(ray);
}

bool RenderEngine::renderLoop()
{
	static int i = 0;
	for(int j = 0; j<camera->getHeight(); j++)
	{	
		// Color color = Color(0);
		// for(int p = 0; p<NUM_SAMPLES; p++){
		// 	int jitter = rand() % 100 + 1;
		// 	float scaledJitter = (float)jitter / 100.0f;
		// 	for(int q = 0; q<NUM_SAMPLES; q++){
		// 		color = color + trace(i + ((p + jitter)/NUM_SAMPLES), j + ((q + jitter)/NUM_SAMPLES));
		// 	}
		// }
		// color = color * (1 / ((float)NUM_SAMPLES * (float)NUM_SAMPLES));

		Color color = trace(i, j);
		color.clamp();
		camera->drawPixel(i, j, color);
	}

	if(++i == camera->getWidth())
	{
		i = 0;
		return true;
	}
	return false;
}
