#include <cfloat>

#include <iostream>
#include <memory>
#include <random>

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Geometry.h"
#include "Photon.h"
#include "Material.h"
#include "SceneObject.h"
#include "Random.h"

#undef max
#undef min

class Sphere : public SceneObject
{
private:
	Vector3 center;
	float radious;

public:
	Sphere(const Vector3 &center, float radious);
	virtual ~Sphere() = default;

	virtual bool Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is);
	virtual void Behave(Photon &photon);
	virtual Vector3 GetNormalVector(const Vector3 &point);
};

Sphere::Sphere(const Vector3 &_center, float _radious) :
	center(_center), radious(_radious)
{
	material.diffuse << 1.0f, 0.0f, 0.0f;
}

bool Sphere::Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is)
{
	Vector3 v1;
	v1 = origin - this->center;
	float s1 = direction.dot(v1);

	float norm1 = direction.squaredNorm();
	float norm2 = v1.norm();

	float D = s1 * s1 - norm1 * (norm2 * norm2 - radious * radious);

	if (D < 0)
	{
		return false;
	}

	float a1 = (-s1 + sqrt(D)) / norm1; // answer1
	float a2 = (-s1 - sqrt(D)) / norm1; // answer2

	float a = -1.0f;
	if (a1 > 0)
	{
		if (a2 > 0)
			a = (a1 < a2) ? a1 : a2;
		else
			a = a1;
	}
	else
	{
		if (a2 > 0)
			a = a2;
	}

	if (a < 0)
		return false;

	is.distance = a;
	
	return true;
}

void Sphere::Behave(Photon &photon)
{
	Intersection is;
	if (this->Collide(photon.position, photon.direction, is))
	{
		photon.position += photon.direction * is.distance;

		float diffuseReflectance = Average(this->material.diffuse);
		float specularReflectance = Average(this->material.specular) + diffuseReflectance;
		float refractiveIndex = Average(this->material.refraction) + specularReflectance;

		float p = Random::Value();
		if (p <= diffuseReflectance) // Diffuse Reflection
		{
			// Diffuse 拡散反射
			photon.incidence = photon.direction.reverse();

			float theta = acos(sqrt(Random::Value()));
			float phai = 2.0f * 3.1415f * Random::Value();
			photon.direction <<
				sin(theta) * cos(phai),
				cos(theta),
				sin(theta) * sin(phai);
			photon.direction.normalize();

			Matrix4 basis;
			Vector3 n = this->GetNormalVector(photon.position);
			Matrix4OrthoBasis(basis, n);
			Vector3TransformCoordinate(photon.direction, basis);

			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::DiffuseReflection;
		}
		else if (p <= specularReflectance)
		{
			// Specular 鏡面反射
			Vector3 n = this->GetNormalVector(photon.position);
			photon.direction -= n * n.dot(photon.direction) * 2;

			photon.spectrum = this->material.specular;
			photon.behavior = PhotonBehavior::SpecularReflection;
		}
		else if (p <= refractiveIndex)
		{
			// Refractive 屈折
			Vector3 n = this->GetNormalVector(photon.position);
			Vector3 refract_in = photon.direction.reverse();
		}
		else // Absorption
		{
			photon.incidence = photon.direction.reverse();
			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::Absorption;
		}
	}
}

Vector3 Sphere::GetNormalVector(const Vector3 &point)
{
	Vector3 dif = point - this->center;
	dif.normalize();
	return dif;
}

std::unique_ptr<SceneObject> CreateSphere(const Vector3 &center, float radious)
{
	return std::unique_ptr<SceneObject>(new Sphere(center, radious));
}




class Triangle : public SceneObject
{
private:
	Vector3 v0, v1, v2;

public:
	Triangle(const Vector3 &_v0, const Vector3 &_v1, const Vector3 &_v2);
	virtual ~Triangle() = default;

	virtual bool Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is);
	virtual void Behave(Photon &photon);
	virtual Vector3 GetNormalVector(const Vector3 &point);
};

Triangle::Triangle(const Vector3 &_v0, const Vector3 &_v1, const Vector3 &_v2) :
	v0(_v0), v1(_v1), v2(_v2)
{
	material.diffuse << 1.0f, 1.0f, 1.0f;
}

bool Triangle::Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is)
{
	Vector3 line0 = v2 - v0;
	Vector3 line1 = v1 - v0;
	Vector3 normal = line0.cross(line1);
	normal.normalize();

	float s1 = direction.dot(normal);
	if (std::abs(s1) < 0.000001f)
		return false;

	Vector3 sub = v0 - origin;
	float s2 = sub.dot(normal) / s1;
	if (s2 < 0)
		return false;

	Vector3 p = origin + direction * s2;
	Vector3 d0, d1;
	Vector3 vc;
	float d;

	d0 = p - v0;
	d1 = v1 - v0;
	// vc = d0.cross(d1).dot(normal);
	d = d0.cross(d1).dot(normal);
	if (d < 0)
		return false;
	
	d0 = p - v1;
	d1 = v2 - v1;
	d = d0.cross(d1).dot(normal);
	if (d < 0)
		return false;

	d0 = p - v2;
	d1 = v0 - v2;
	d = d0.cross(d1).dot(normal);
	if (d < 0)
		return false;

	is.distance = s2;

	return true;
}

void Triangle::Behave(Photon &photon)
{
	Intersection is;
	if (this->Collide(photon.position, photon.direction, is))
	{	
		photon.position += photon.direction * is.distance;
		
		float diffuseReflectance = Average(this->material.diffuse);
		float specularReflectance = Average(this->material.specular) + diffuseReflectance;
		float refractiveIndex = Average(this->material.refraction) + specularReflectance;

		float p = Random::Value();
		if (p <= diffuseReflectance) // Diffuse Reflection
		{
			// Diffuse 拡散反射
			photon.incidence = photon.direction.reverse();

			float theta = acos(sqrt(Random::Value()));
			float phai = 2.0f * 3.1415f * Random::Value();
			photon.direction <<
				sin(theta) * cos(phai),
				cos(theta),
				sin(theta) * sin(phai);
			photon.direction.normalize();

			Matrix4 basis;
			Vector3 n = this->GetNormalVector(photon.position);
			Matrix4OrthoBasis(basis, n);
			Vector3TransformCoordinate(photon.direction, basis);

			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::DiffuseReflection;
		}
		else if (p <= specularReflectance)
		{
			// Specular 鏡面反射
			Vector3 n = this->GetNormalVector(photon.position);
			photon.direction -= n * n.dot(photon.direction) * 2;

			photon.spectrum = this->material.specular;
			photon.behavior = PhotonBehavior::SpecularReflection;
		}
		else
		{	// 吸収
			photon.incidence = photon.direction.reverse();
			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::Absorption;
		}
	}
}

Vector3 Triangle::GetNormalVector(const Vector3 &point)
{
	Vector3 line0 = v2 - v0;
	Vector3 line1 = v1 - v0;
	Vector3 n = line0.cross(line1);
	n.normalize();

	return n;
}

std::unique_ptr<SceneObject> CreateTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2)
{
	return std::unique_ptr<SceneObject>(new Triangle(v0, v1, v2));
}
