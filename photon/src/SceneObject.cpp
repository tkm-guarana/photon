
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
		float specularReflectance = Average(this->material.specular);
		float refractiveIndex = Average(this->material.refraction);

		float p = Random::Value();
		if (p <= diffuseReflectance) // Diffuse Reflection
		{
			photon.incidence = photon.direction.reverse();

			float theta = acos(sqrt(Random::Value()));
			float phai = 2.0f * 3.1415f * Random::Value();
			photon.direction <<
				sin(theta) * cos(phai),
				cos(theta),
				sin(theta) * sin(phai);
			photon.direction.normalize();

			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::DiffuseReflection;
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

#if 0
byte Sphere::photon(Photon &photon)
{
	float t = collide(&position, radious, &photon.position, &photon.direction);
	if (t < 0)
	{
		return NoCollision; // 交差しない
	}
	// 材質により反射、透過、吸収のいずれかを選択
	// 反射ならば以下のコードを実行する

	photon.position += photon.direction * t; // 移動する

	// 平均拡散反射率
	float diffuse_reflectance = average(material_info->diffuse);//(diffuse.x + diffuse.y + diffuse.z) / 3.0f;
	// 平均鏡面反射率
	float specular_reflectance = average(material_info->specular);
	// 屈折率
	float refractive_index = average(material_info->refraction);

	byte result = 0;
	float p = random();
	if(p <= diffuse_reflectance)
	{	// 拡散反射
		photon.incidence = photon.direction * -1; // 入射方向を保存

		// 無造作に反射方向を選ぶ
		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);
		fw::Vector3Normalize(&photon.direction);

		// 法線の基底に変換
		fw::Vector3 normal = photon.position - position;
		fw::Vector3Normalize(&normal);
		fw::Matrix4 basis;
		pm::ortho_basis(&basis, &normal);
		fw::Vector3TransformCoordinate(&photon.direction, &basis);

		photon.spectrum.x = material_info->diffuse.x;
		photon.spectrum.y = material_info->diffuse.y;
		photon.spectrum.z = material_info->diffuse.z;
		
		result = DiffuseReflect;
	}
	else if(diffuse_reflectance < p && p < diffuse_reflectance + specular_reflectance)
	{	// 鏡面反射
		fw::Vector3 normal = photon.position - position;
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // 反射方向を計算

		photon.spectrum.x = material_info->specular.x;
		photon.spectrum.y = material_info->specular.y;
		photon.spectrum.z = material_info->specular.z;

		result = SpecularReflect;
	}
	else if(diffuse_reflectance + specular_reflectance < p && p < diffuse_reflectance + specular_reflectance + refractive_index)
	{	// 屈折
		fw::Vector3 normal = photon.position - position;
		fw::Vector3Normalize(&normal);
		fw::Vector3 refract_in = photon.direction * -1;
		pm::refract(&photon.direction, &refract_in, &normal, 1.0f, 2.0f); //refractive_index);
		photon.position += normal * -0.0000001f; // 少しずらす

		float tt = collide(&position, radious, &photon.position, &photon.direction);
		photon.position += photon.direction * tt; // 球体内移動

		refract_in = photon.direction * -1;
//		refract(&photon.direction, &refract_in, &normal, refractive_index, 1.0f); // 外へ
		pm::refract(&photon.direction, &refract_in, &normal, 2.0f, 1.0f); // 外へ

		photon.position += normal * 0.0000001f; // 少しずらす

		result = Refraction;
	}
	else
	{	// 吸収
		photon.incidence = photon.direction * -1; // 後でunary oeperator作れ

		// material_info->diffuseの色で良いんだろうか？
		photon.spectrum.x = material_info->diffuse.x;
		photon.spectrum.y = material_info->diffuse.y;
		photon.spectrum.z = material_info->diffuse.z;

		result = Absorption;
	}
	
	return result;
}
#endif

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
	material.diffuse << 0.0f, 1.0f, 0.0f;
}

bool Triangle::Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is)
{
	Vector3 line0 = v2 - v1;
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
		float diffuseReflectance = Average(this->material.diffuse);
		float specularReflectance = Average(this->material.specular);

		float p = Random::Value();
		if (p <= diffuseReflectance) // Diffuse Reflection
		{
			photon.incidence = photon.direction.reverse();

			float theta = acos(sqrt(Random::Value()));
			float phai = 2.0f * 3.1415f * Random::Value();
			photon.direction <<
				sin(theta) * cos(phai),
				cos(theta),
				sin(theta) * sin(phai);
			photon.direction.normalize();

			Vector3 line0 = v2 - v1;
			Vector3 line1 = v1 - v0;
			Vector3 n = line0.cross(line1);
			n.normalize();

			Matrix4 basis;
			Matrix4OrthoBasis(basis, n);
			basis.transpose();

			Vector4 t;
			t << n(0), n(1), n(2), 1.0f;
			t = basis * t;
			t = t / t(3);
			photon.direction << t(0), t(1), t(2);

			photon.spectrum = this->material.diffuse;
			photon.behavior = PhotonBehavior::DiffuseReflection;
		}
	}
}

Vector3 Triangle::GetNormalVector(const Vector3 &point)
{
	Vector3 line0 = v2 - v1;
	Vector3 line1 = v1 - v0;
	Vector3 n = line0.cross(line1);
	n.normalize();

	return n;
}

std::unique_ptr<SceneObject> CreateTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2)
{
	return std::unique_ptr<SceneObject>(new Triangle(v0, v1, v2));
}
