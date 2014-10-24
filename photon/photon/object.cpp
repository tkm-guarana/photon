
#include <assert.h>

#include "../fw/fw_matrix.h"
#include "object.h"
#include "utility.h"

namespace pm
{


class Sphere : public Object
{
private:
	fw::Vector3 position;
	float		radious;
	fw::Vector3	diffuse;
	fw::Vector3	specular;
	Material	*material_info;

public:
//	Sphere(const fw::Vector3 &pos, float r) : position(pos), radious(r), diffuse(0.8f, 0.2f, 0.4f), specular(0.1f, 0.6f, 0.3f) {}
//	Sphere(const fw::Vector3 &pos, float r) : position(pos), radious(r), diffuse(0.3f, 0.0f, 0.0f), specular(0.7f, 1.0f, 1.0f) {}
	Sphere(const fw::Vector3 &pos, float radious, Material *mtrl);
	virtual ~Sphere() {}

	virtual float distance(const fw::Vector3 &base, const fw::Vector3 &dir) const;
	virtual byte photon(Photon &photon);
	virtual bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum);
	virtual fw::Vector3 normal(const fw::Vector3 &intersection);
	virtual bool refract(fw::Vector3 &base, fw::Vector3 &dir);
	virtual Material* material();
};

Sphere::Sphere(const fw::Vector3 &pos, float r, Material *mtrl) :
	position(pos), radious(r), material_info(mtrl)
{
	assert(mtrl);	
}

float Sphere::distance(const fw::Vector3 &base, const fw::Vector3 &dir) const
{
	return collide(&position, radious, &base, &dir);
}

// ñﬂÇËíl îΩéÀÅAìßâﬂ true ãzé˚ false
byte Sphere::photon(Photon &photon)
{
	float t = collide(&position, radious, &photon.position, &photon.direction);
	if(t < 0)
	{
		return NoCollision; // åç∑ÇµÇ»Ç¢
	}

	// çﬁéøÇ…ÇÊÇËîΩéÀÅAìßâﬂÅAãzé˚ÇÃÇ¢Ç∏ÇÍÇ©ÇëIë
	// îΩéÀÇ»ÇÁÇŒà»â∫ÇÃÉRÅ[ÉhÇé¿çsÇ∑ÇÈ

	photon.position += photon.direction * t; // à⁄ìÆÇ∑ÇÈ

	// ïΩãœägéUîΩéÀó¶
	float diffuse_reflectance = average(material_info->diffuse);//(diffuse.x + diffuse.y + diffuse.z) / 3.0f;
	// ïΩãœãæñ îΩéÀó¶
	float specular_reflectance = average(material_info->specular);
	// ã¸ê‹ó¶
	float refractive_index = average(material_info->refraction);

	byte result = 0;
	float p = random();
	if(p <= diffuse_reflectance)
	{	// ägéUîΩéÀ
		photon.incidence = photon.direction * -1; // ì¸éÀï˚å¸Çï€ë∂

		// ñ≥ë¢çÏÇ…îΩéÀï˚å¸ÇëIÇ‘
		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);
		fw::Vector3Normalize(&photon.direction);

		// ñ@ê¸ÇÃäÓíÍÇ…ïœä∑
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
	{	// ãæñ îΩéÀ
		fw::Vector3 normal = photon.position - position;
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // îΩéÀï˚å¸ÇåvéZ

		photon.spectrum.x = material_info->specular.x;
		photon.spectrum.y = material_info->specular.y;
		photon.spectrum.z = material_info->specular.z;

		result = SpecularReflect;
	}
	else if(diffuse_reflectance + specular_reflectance < p && p < diffuse_reflectance + specular_reflectance + refractive_index)
	{	// ã¸ê‹
		fw::Vector3 normal = photon.position - position;
		fw::Vector3Normalize(&normal);
		fw::Vector3 refract_in = photon.direction * -1;
		pm::refract(&photon.direction, &refract_in, &normal, 1.0f, 2.0f); //refractive_index);
		photon.position += normal * -0.0000001f; // è≠ÇµÇ∏ÇÁÇ∑

		float tt = collide(&position, radious, &photon.position, &photon.direction);
		photon.position += photon.direction * tt; // ãÖëÃì‡à⁄ìÆ

		refract_in = photon.direction * -1;
//		refract(&photon.direction, &refract_in, &normal, refractive_index, 1.0f); // äOÇ÷
		pm::refract(&photon.direction, &refract_in, &normal, 2.0f, 1.0f); // äOÇ÷

		photon.position += normal * 0.0000001f; // è≠ÇµÇ∏ÇÁÇ∑

		result = Refraction;
	}
	else
	{	// ãzé˚
		photon.incidence = photon.direction * -1; // å„Ç≈unary oeperatorçÏÇÍ

		// material_info->diffuseÇÃêFÇ≈ó«Ç¢ÇÒÇæÇÎÇ§Ç©ÅH
		photon.spectrum.x = material_info->diffuse.x;
		photon.spectrum.y = material_info->diffuse.y;
		photon.spectrum.z = material_info->diffuse.z;

		result = Absorption;
	}
	
	return result;
}

bool Sphere::trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum)
{
	float t = collide(&position, radious, &base, &dir);
	if(t < 0)
	{
		return false; // åç∑ÇµÇ»Ç¢
	}

	base += dir * t; // à⁄ìÆÇ∑ÇÈ

	reflect(&position, &base, &dir);

	return true;
}

fw::Vector3 Sphere::normal(const fw::Vector3 &intersection)
{
	fw::Vector3 n;
	n = intersection - position;
	fw::Vector3Normalize(&n);

	return n;
}

bool Sphere::refract(fw::Vector3 &base, fw::Vector3 &dir)
{
	float t = collide(&position, radious, &base, &dir);
	if(t < 0)
	{
		return false; // åç∑ÇµÇ»Ç¢
	}

	base += dir * t; // à⁄ìÆÇ∑ÇÈ

	fw::Vector3 normal;
	normal = base - position;
	fw::Vector3Normalize(&normal);

	fw::Vector3 in = dir * -1;

	if(fw::Vector3Dot(&in, &normal) > 0)
	{	// ì¸ÇÈ
		pm::refract(&dir, &in, &normal, 1.0f, 2.0f);
		base -= normal * 0.0001f; // ¡Æ≤Ç∏ÇÁÇ∑
	}
	else
	{	// èoÇÈ
		normal *= -1;
		pm::refract(&dir, &in, &normal, 2.0f, 1.0f);
		base -= normal * 0.0001f;
	}

	return true;	
}

Material* Sphere::material()
{
	return material_info;
}

Object* CreateSphere(const fw::Vector3 &pos, float radious, Material *mtrl)
{
	return new Sphere(pos, radious, mtrl);
}



// éOäpå`
class Triangle : public Object
{
private:
	fw::Vector3 v0, v1, v2;
	fw::Vector3 diffuse;
	fw::Vector3 specular;
	Material	*material_info;
public:
	Triangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2, Material *mtrl);
	virtual ~Triangle() {}

	virtual float distance(const fw::Vector3 &base, const fw::Vector3 &dir) const;
	virtual byte photon(Photon &photon);
	virtual bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum);
	virtual fw::Vector3 normal(const fw::Vector3 &intersection);
	virtual bool refract(fw::Vector3 &base, fw::Vector3 &dir);
	virtual Material* material();
};

Triangle::Triangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2, Material *mtrl) :
	v0(vtx0), v1(vtx1), v2(vtx2), diffuse(0.3f, 0.4f, 0.9f), specular(0.0f, 0.0f, 0.0f), material_info(mtrl)
{
	assert(mtrl);
}


float Triangle::distance(const fw::Vector3 &base, const fw::Vector3 &dir) const
{
	return collide(&v0, &v1, &v2, &base, &dir);
}

byte Triangle::photon(Photon &photon)
{
	float t = collide(&v0, &v1, &v2, &photon.position, &photon.direction);
	if(t < 0)
	{
		return NoCollision; // åç∑ÇµÇ»Ç¢
	}

	// çﬁéøÇ…ÇÊÇËîΩéÀÅAìßâﬂÅAãzé˚ÇÃÇ¢Ç∏ÇÍÇ©ÇëIë
	// îΩéÀÇ»ÇÁÇŒà»â∫ÇÃÉRÅ[ÉhÇé¿çsÇ∑ÇÈ

	photon.position += photon.direction * t; // à⁄ìÆÇ∑ÇÈ

	// ïΩãœägéUîΩéÀó¶
	float diffuse_reflectance = average(material_info->diffuse);
	// ïΩãœãæñ îΩéÀó¶
	float specular_reflectance = average(material_info->specular);

	byte result = 0;
	float p = random();
	if(p <= diffuse_reflectance)
	{	// ägéUîΩéÀ
		photon.incidence = photon.direction * -1; // ì¸éÀï˚å¸Çï€ë∂

		// ñ≥ë¢çÏÇ…îΩéÀï˚å¸ÇëIÇ‘
		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);
		fw::Vector3Normalize(&photon.direction);

		// äÓíÍïœä∑çsóÒê∂ê¨
		fw::Matrix4 basis;
		{
			fw::Vector3 normal; // ñ@ê¸åvéZ
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
			fw::Vector3Normalize(&normal);

			pm::ortho_basis(&basis, &normal); // ñ@ê¸Çé≤Ç∆Ç∑ÇÈê≥ãKíºåäÓíÍ
		}
		fw::Vector3TransformCoordinate(&photon.direction, &basis);

		photon.spectrum.x = material_info->diffuse.x;
		photon.spectrum.y = material_info->diffuse.y;
		photon.spectrum.z = material_info->diffuse.z;

		/*
		photon.spectrum.x -= 1.0f - material_info->diffuse.x;
		photon.spectrum.y -= 1.0f - material_info->diffuse.y;
		photon.spectrum.z -= 1.0f - material_info->diffuse.z;

		photon.spectrum.x = std::max(photon.spectrum.x, 0.0f);
		photon.spectrum.y = std::max(photon.spectrum.y, 0.0f);
		photon.spectrum.z = std::max(photon.spectrum.z, 0.0f);
		*/

		result = DiffuseReflect;
	}
	else if(diffuse_reflectance < p && p < diffuse_reflectance + specular_reflectance)
	{	// ãæñ îΩéÀ
		photon.incidence = photon.direction * -1; // ì¸éÀï˚å¸Çï€ë∂

		fw::Vector3 normal; // ñ@ê¸åvéZ
		{
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
		}
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // îΩéÀï˚å¸ÇåvéZ

		photon.spectrum.x = material_info->specular.x;
		photon.spectrum.y = material_info->specular.y;
		photon.spectrum.z = material_info->specular.z;

		/*
		photon.spectrum.x -= 1.0f - specular.x;
		photon.spectrum.y -= 1.0f - specular.y;
		photon.spectrum.z -= 1.0f - specular.z;

		photon.spectrum.x = std::max(photon.spectrum.x, 0.0f);
		photon.spectrum.y = std::max(photon.spectrum.y, 0.0f);
		photon.spectrum.z = std::max(photon.spectrum.z, 0.0f);
		*/
		result = SpecularReflect;
	}
	else
	{	// ãzé˚
		photon.incidence = photon.direction * -1; // å„Ç≈unary oeperatorçÏÇÍ

		// material_info->diffuseÇÃêFÇ≈ó«Ç¢ÇÒÇæÇÎÇ§Ç©ÅH
		photon.spectrum.x = material_info->diffuse.x;
		photon.spectrum.y = material_info->diffuse.y;
		photon.spectrum.z = material_info->diffuse.z;

		result = Absorption;
	}
	
	return result;
}

bool Triangle::trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum)
{
	float t = collide(&v0, &v1, &v2, &base, &dir);
	if(t < 0)
	{
		return false; // åç∑ÇµÇ»Ç¢
	}

	base += dir * t; // à⁄ìÆÇ∑ÇÈ

	// îΩéÀÉxÉNÉgÉãÇ…èëÇ´ä∑Ç¶ÇÈ
	reflect(&v0, &v1, &v2, &dir);

	return true;
}

fw::Vector3 Triangle::normal(const fw::Vector3 &intersection)
{
	fw::Vector3 n; // ñ@ê¸åvéZ

	fw::Vector3 line0, line1;
	fw::Vector3Subtract(&line0, &v2, &v0);
	fw::Vector3Subtract(&line1, &v1, &v0);
	fw::Vector3Cross(&n, &line0, &line1);
	fw::Vector3Normalize(&n);

	return n;
}

bool Triangle::refract(fw::Vector3 &base, fw::Vector3 &dir)
{
	return false;
}

Material* Triangle::material()
{
	return material_info;
}

Object* CreateTriangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2, Material *mtrl)
{
	return new Triangle(vtx0, vtx1, vtx2, mtrl);
}





}
