
#include <vector>
#include <set>
#include <memory>
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


void Visualizer::RenderPoints(std::vector<Photon> photons, Canvas &canvas)
{
	// これは、Canvasクラスに移動する
	Matrix4 w = this->world;
	Matrix4 v = this->view;
	Matrix4 p = this->projection;
	Matrix4 s;
	Matrix4Screen(s, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Vector3 color(1, 1, 1);

	for (auto photon : photons)
	{
		Vector3 t = photon.position;
		Vector3TransformCoordinate(t, w);
		Vector3TransformCoordinate(t, v);
		Vector3TransformCoordinate(t, p);
		Vector3TransformCoordinate(t, s);

		int x = int(t(0));
		int y = int(t(1));

		if(0 < x && x < SCREEN_WIDTH &&
		   0 < y && y < SCREEN_HEIGHT)
		{
			int t = y * SCREEN_WIDTH + x;
			canvas.SetColor(x, y, photon.spectrum);
		}
	}
}
