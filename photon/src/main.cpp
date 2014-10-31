
#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <mutex>

#include <SDL2/SDL.h>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Geometry"

#include "Geometry.h"
#include "Photon.h"
#include "Material.h"
#include "SceneObject.h"
#include "Light.h"
#include "DimensionalTree.h"
#include "Scene.h"
#include "Canvas.h"
#include "Visualizer.h"



#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window;

	window = SDL_CreateWindow("SDL Application",
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  SCREEN_WIDTH, SCREEN_HEIGHT,
							  SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_RGBA8888,
											 SDL_TEXTUREACCESS_STREAMING,
											 SCREEN_WIDTH,
											 SCREEN_HEIGHT);

	auto canvas = CreateSDLCanvas(texture);
	Vector3 white(0, 0, 0);
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		for (int j = 0; j < SCREEN_WIDTH; j++)
			canvas->SetColor(j, i, white);

	Scene scene;
	Vector3 pl(0.0f, 6.0f, 1.0f);

	scene.lights.push_back(CreateSquareLight(pl, 2.0f, 2.0f));

	scene.objects.push_back(CreateSphere(Vector3(0.0f, 1.0f, 0.0f), 1.0f));
	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 0.0f, 3.0f), Vector3(3.0f, 0.0f, -6.0f), Vector3(3.0f, 0.0f, 3.0f)));
	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 0.0f, 3.0f), Vector3(-3.0f, 0.0f, -6.0f), Vector3(3.0f, 0.0f, -6.0f)));

	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, 3.0f),
										   Vector3(-3.0f, 0.0f, 3.0f),
										   Vector3( 3.0f, 0.0f, 3.0f)));
	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, 3.0f),
										   Vector3( 3.0f, 0.0f, 3.0f),
										   Vector3( 3.0f, 6.0f, 3.0f)));

	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(-3.0f, 0.0f, -6.0f), Vector3(-3.0f, 0.0f, 3.0f)));
	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(-3.0f, 0.0f, 3.0f), Vector3(-3.0f, 6.0f, 3.0f)));

	scene.objects.push_back(CreateTriangle(Vector3(3.0f, 6.0f, 3.0f), Vector3(3.0f, 0.0f, 3.0f), Vector3(3.0f, 0.0f, -6.0f)));
	scene.objects.push_back(CreateTriangle(Vector3(3.0f, 6.0f, 3.0f), Vector3(3.0f, 0.0f, -6.0f), Vector3(3.0f, 6.0f, -6.0f)));

	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(-3.0f, 6.0f, 3.0f), Vector3(3.0f, 6.0f, 3.0f)));
	scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(3.0f, 6.0f, 3.0f), Vector3(3.0f, 6.0f, -6.0f)));

	// scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(3.0f, 0.0f, -6.0f), Vector3(-3.0f, 0.0f, -6.0f)// ));
	// scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f), Vector3(3.0f, 6.0f, -6.0f), Vector3(3.0f, 0.0f, -6.0f))
		// );
	scene.Emitte(100000, 3);

	std::cout << "emitting photon was finished" << std::endl;

	Visualizer visualizer(*canvas, scene);

	Vector3 eye;
	Vector3 look;
	Vector3 up;
	Matrix4 view;
	Matrix4 projection;

	// ƒJƒƒ‰À•WŒn
	eye << 0.0f, 2.0f, -10.0f;
	look << 0.0f, 1.0f, 0.0f;
	up << 0.0f, 1.0f, 0.0f;

	Matrix4ViewL(view, eye, look, up); // ok
	Matrix4PerspectiveL(projection, 60.0f * 3.1415f / 180.0f,
						(float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT),
						1.0f, 100.0f);

	visualizer.world = Matrix4::Identity();
	visualizer.view = view;
	visualizer.projection = projection;

#if 1
	visualizer.Render();
#else
	visualizer.RenderPoints();
#endif

	canvas = nullptr;

	SDL_Event event;
	bool loop = true;

	while (loop)
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				loop = false;
				break;
			}
		}	

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		// SDL_RenderDrawPoint(renderer, 600, 400);
		// SDL_RenderDrawLine(renderer, 0, 0, 400, 300);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
    
    return 0;
}
