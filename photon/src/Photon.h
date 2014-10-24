
#ifndef _PHOTO_H_
#define _PHOTO_H_

// 光学的状態
enum PhotonBehavior
{
	None,				// 衝突なし
	BRDF,				// Bidirectional Reflection Distribution Function
	Absorption,			// 吸収
	DiffuseReflection,	// 拡散反射
	SpecularReflection,	// 鏡面反射
	Refraction,			// 屈折
};

struct Photon
{
	Vector3 position;
	Vector3 direction;
	Vector3 spectrum;
	Vector3 incidence;
	PhotonBehavior behavior;
	
	friend std::ostream& operator << (std::ostream &os, const Photon &p)
	{
		os << p.position << std::endl;
		os << p.direction << std::endl;

		return os;
	}
};


#endif /* _PHOTO_H_ */
