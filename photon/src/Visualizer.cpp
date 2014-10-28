
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


void Visualizer::Render(Canvas &canvas, Scene &scene)
{
	Matrix4 screen = canvas.GetScreenMatrix();
	Size size = canvas.GetScreenSize();
	int width = size(0);
	int height = size(1);

	Matrix4 inverse_world = this->world.inverse();
	Matrix4 inverse_view = this->view.inverse();
	Matrix4 inverse_projection = this->projection.inverse();
	Matrix4 inverse_screen = screen.inverse();
	
	Matrix4 inverse_affine = inverse_world * inverse_view * inverse_projection * inverse_screen;
	Vector4 source(0.0f, 0.0f, 0.0f, 1.0f);
	
	Vector4 s2 = inverse_view * source;
	s2 = s2 / s2(3);
	Vector3 origin(s2(0), s2(1), s2(2));

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Vector4 destination;
			destination << float(x), float(y), 0.0f, 1.0f;
			destination = inverse_affine * destination;
			destination = destination / destination(3);

			Vector3 d2;
			d2 << destination(0), destination(1), destination(2);
			Vector3 direction;
			direction = d2 - origin;
			direction.normalize();

			Vector3 color(0, 0, 0);
			scene.Trace(origin, direction, color, 0);

			canvas.SetColor(x, y, color);
		}
	}
}


void Visualizer::RenderPoints(std::vector<Photon> photons,
							  Canvas &canvas)
{
	// これは、Canvasクラスに移動する
	Matrix4 w = this->world;
	Matrix4 v = this->view;
	Matrix4 p = this->projection;
	Matrix4 s = canvas.GetScreenMatrix();
	Size size = canvas.GetScreenSize();
	int width = size(0);
	int height = size(1);
	
	for (auto photon : photons)
	{
		Vector4 t;
		t << photon.position, 1.0f;
		t = s * p * v * w * t;
		t = t / t(3); // homogenious vector

		int x = int(t(0));
		int y = int(t(1));

		if (0 <= x && x < width &&
			0 <= y && y < height)
		{
			canvas.SetColor(x, y, photon.spectrum);
		}
	}
}
