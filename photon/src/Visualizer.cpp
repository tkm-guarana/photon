
#include <vector>
#include <set>
#include <memory>
#include <iostream>
#include <thread>
#include <mutex>

#include <SDL2/SDL.h>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Geometry"

#include "Scheduler.h"
#include "Geometry.h"
#include "Photon.h"
#include "Material.h"
#include "SceneObject.h"
#include "Light.h"
#include "DimensionalTree.h"
#include "Scene.h"
#include "Canvas.h"
#include "Visualizer.h"



Visualizer::Visualizer(Canvas &_canvas, Scene &_scene) :
	canvas(_canvas), scene(_scene)
{
}

void Visualizer::Render()
{
	Matrix4 screen = this->canvas.GetScreenMatrix();
	Size size = this->canvas.GetScreenSize();
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

	// Scheduler<Visualizer::ThreadParam> scheduler;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// ThreadParam param;
			// param.x = x;
			// param.y = y;
			// param.origin = origin;
			// param.affine = inverse_affine;

			// scheduler.Add(param);

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
			this->scene.Trace(origin, direction, color, 0);

			this->canvas.SetColor(x, y, color);
		}
	}
}

void Visualizer::RenderWorker(ThreadParam &param)
{
	Vector4 destination;
	destination << float(param.x), float(param.y), 0.0f, 1.0f;
	destination = param.affine * destination;
	destination = destination / destination(3);

	Vector3 d2;
	d2 << destination(0), destination(1), destination(2);
	Vector3 direction;
	direction = d2 - param.origin;
	direction.normalize();

	Vector3 color(0, 0, 0);
	this->scene.Trace(param.origin, direction, color, 0);

	this->canvas.SetColor(param.x, param.y, color);	
}


void Visualizer::RenderPoints()
{
	// これは、Canvasクラスに移動する
	Matrix4 w = this->world;
	Matrix4 v = this->view;
	Matrix4 p = this->projection;
	Matrix4 s = this->canvas.GetScreenMatrix();
	Size size = this->canvas.GetScreenSize();
	int width = size(0);
	int height = size(1);
	
	for (auto photon : this->scene.tree.points)
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
			this->canvas.SetColor(x, y, photon.spectrum);
		}
	}
}

void Visualizer::RenderLine(const Vector3 &begin,
							const Vector3 &end,
							const Vector3 &color)
{
	Matrix4 w = this->world;
	Matrix4 v = this->view;
	Matrix4 p = this->projection;
	Matrix4 s = this->canvas.GetScreenMatrix();

	Vector4 b, e;
	b << begin, 1.0f;
	e << end, 1.0f;

	b = s * p * v * w * b;
	b = b / b(3);
	e = s * p * v * w * e;
	e = e / e(3);

	float x0 = b(0);
	float y0 = b(1);
	float x1 = e(0);
	float y1 = e(1);

	float xb, xe, yb, ye;

	if (x0 < x1)
	{
		xb = x0;
		xe = x1;
		yb = y0;
		ye = y1;
	}
	else
	{
		xb = x1;
		xe = x0;
		yb = y1;
		ye = y0;
	}

	float xd = (xe - xb);
	float m = (y1 - y0) / xd;
	
	for (int i = 0, e = int(xd); i < e; i++)
	{
		this->canvas.SetColor(xb + i,
							  yb + i * m,
							  color);
	}
}
