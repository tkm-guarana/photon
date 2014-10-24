
#ifndef _photon_object_h_
#define _photon_object_h_

#include <vector>
#include "photon.h"
#include "material.h"

namespace pm
{

enum PhotonState
{
	NoCollision		= 0x0,	// è’ìÀñ≥Çµ
	Absorption		= 0x1,	// ãzé˚
	Reflection		= 0x2,	// îΩéÀ
	Refraction		= 0x4,	// ã¸ê‹
	DiffuseReflect	= 0x3,	// ägéUîΩéÀ
	SpecularReflect	= 0x2,	// ãæñ îΩéÀ
	BRDFReflect		= 0x8	// BRDF
};

class Object
{
private:

public:
	Object() {}
	virtual ~Object() {}

	virtual float distance(const fw::Vector3 &base, const fw::Vector3 &dir) const = 0;
	virtual byte photon(Photon &photon) = 0;
	virtual bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum) = 0;
	virtual fw::Vector3 normal(const fw::Vector3 &intersection) = 0;
	virtual bool refract(fw::Vector3 &base, fw::Vector3 &dir) = 0;
	virtual Material* material() = 0;
};

Object* CreateSphere(const fw::Vector3 &pos, float radious, Material *mtrl);
Object* CreateTriangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2, Material *mtrl);

typedef std::vector<Object*>	ObjCont;


}

#endif