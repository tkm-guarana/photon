

#include <memory>
#include <iostream>
#include <memory>

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
#include "Camera.h"




Camera::Camera(Vector3 _eye, Vector3 _look, Vector3 _up) :
	eye(_eye), look(_look), up(_up), fov(60.0f), znear(1.0f), zfar(100.0f)
{
	Matrix4ViewL(this->view, this->eye, this->look, this->up);
	Matrix4PerspectiveL(this->projection, this->fov * 3.1415f / 180.0f,
						(float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT),
						this->znear, this->zfar);
}

