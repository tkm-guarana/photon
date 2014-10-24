
#include <vector>
#include <set>
#include <memory>
#include <iostream>

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
	
	w.transpose();
	v.transpose();
	p.transpose();
	s.transpose();

	Vector3 color(1, 1, 1);

	for (auto photon : photons)
	{
		Vector4 t;
		t << photon.position, 1.0f;
		t = w * t;
		t = t / t(3);
		t = v * t;
		t = t / t(3);
		t = p * t;
		t = t / t(3);
		t = s * t;
		t = t / t(3);

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
