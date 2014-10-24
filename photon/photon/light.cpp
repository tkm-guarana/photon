
#include <assert.h>

#include "../fw/fw_matrix.h"
#include "light.h"
#include "photon.h"
#include "utility.h"

namespace pm
{

// ägéUì_åıåπ
class PointLight : public Light
{
private:
	fw::Vector3 position;
public:
	PointLight() : position(0.0f, 0.0f, 0.0f) {}
	PointLight(const fw::Vector3 &pos) : position(pos) {}
	virtual ~PointLight() {}

	virtual void emitte(Photon &photon); // ï˙èo
};


void PointLight::emitte(Photon &photon)
{
	float x, y, z;
	do
	{
		x = 2 * random() - 1;
		y = 2 * random() - 1;
		z = 2 * random() - 1;
	}
	while(x*x + y*y + z*z > 1);
	photon.direction.x = x;
	photon.direction.y = y;
	photon.direction.z = z;

	fw::Vector3Normalize(&photon.direction);

	photon.position = position;

	photon.spectrum.x = 1.0f;
	photon.spectrum.y = 1.0f;
	photon.spectrum.z = 1.0f;
	photon.incidence.x = photon.incidence.y = photon.incidence.z = 0.0f;
}

Light* CreatePointLight(fw::Vector3 &position)
{
	return new PointLight(position);
}



// ägéUïΩñ åıåπ
class SquareLight : public Light
{
private:
	float width, height;
	fw::Vector3 position;
public:
	SquareLight(const fw::Vector3 &pos, float w, float h) : position(pos), width(w), height(h) {}
	virtual ~SquareLight() {}
	
	virtual void emitte(Photon &photon);
};


void SquareLight::emitte(Photon &photon)
{
	// à íuÇñ≥ë¢çÏÇ…ëIÇ‘
	photon.position = position;
	photon.position.x = (random() - 0.5f) * width;
	photon.position.z = (random() - 0.5f) * height;

	// ägéUï˚å¸
	float theta = acos(sqrt(random()));
	float phai = 2.0f * 3.1415f * random();

	photon.direction.x = sin(theta) * cos(phai);
	photon.direction.y = -cos(theta);
	photon.direction.z = sin(theta) * sin(phai);
	fw::Vector3Normalize(&photon.direction);

	photon.spectrum.x = 1.0f;
	photon.spectrum.y = 1.0f;
	photon.spectrum.z = 1.0f;
	photon.incidence.x = photon.incidence.y = photon.incidence.z = 0.0f;
}

Light* CreateSquareLight(fw::Vector3 &position, float width, float height)
{
	return new SquareLight(position, width, height);
}




}