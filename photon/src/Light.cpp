
#include <random>
#include <memory>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Geometry"

#include "Geometry.h"
#include "Photon.h"
#include "Light.h"


class PointLight : public Light
{
private:
	Vector3 position;
	std::mt19937 random;
	std::uniform_real_distribution<> urd;

public:
	PointLight();
	PointLight(const Vector3 &_position);
	virtual ~PointLight() = default;

	virtual void Emitte(Photon &photon);
};

PointLight::PointLight() : position(0, 0, 0), urd(0, 1)
{
}

PointLight::PointLight(const Vector3 &_position) : position(_position), urd(0, 1)
{
}

void PointLight::Emitte(Photon &photon)
{
	float r = urd(random);

	float x, y, z;
	do
	{
		x = 2 * urd(random) - 1;
		y = 2 * urd(random) - 1;
		z = 2 * urd(random) - 1;
	}
	while (x*x + y*y + z*z > 1);

	photon.direction << x, y, z;
	photon.direction.normalize();

	photon.position = this->position;
	photon.spectrum << 1.0f, 1.0f, 1.0f;
	photon.incidence << 0.0f, 0.0f, 0.0f;
}


class SquareLight : public Light
{
private:
	float width;
	float height;
	Vector3 position;
	std::mt19937 random;
	std::uniform_real_distribution<> urd;

public:
	SquareLight(const Vector3 &_position, float _width, float _height);
	virtual ~SquareLight() = default;
	
	virtual void Emitte(Photon &photon);
};

SquareLight::SquareLight(const Vector3 &_position, float _width, float _height) :
	position(_position), width(_width), height(_height), urd(0, 1)
{
}

void SquareLight::Emitte(Photon &photon)
{
	// 位置を無造作に選ぶ
	photon.position <<
		this->position(0) + (urd(random) - 0.5f) * width,
		this->position(1) + (urd(random) - 0.5f) * height,
		this->position(2);

	// 拡散方向
	float theta = acos(sqrt(urd(random)));
	float phai = 2.0f * 3.1415f * urd(random);

	photon.direction <<
		sin(theta) * cos(phai),
		-cos(theta),
		sin(theta) * sin(phai);
	photon.direction.normalize();
	photon.spectrum << 1.0f, 1.0f, 1.0f;
	photon.incidence << 0.0f, 0.0f, 0.0f;
}




std::unique_ptr<Light> CreatePointLight(const Vector3 &position)
{
	return std::unique_ptr<Light>(new PointLight(position));
}

std::unique_ptr<Light> CreateSquareLight(const Vector3 &position, float width, float height)
{
	return std::unique_ptr<Light>(new SquareLight(position, width, height));
}
