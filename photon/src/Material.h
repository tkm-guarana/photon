
#ifndef _MATERIAL_H_
#define _MATERIAL_H_


class Material
{
public:
	Color diffuse;
	Color specular;
	Color refraction;

	/* Material() = default; */
	/* ~Material() = default; */

	virtual float BRDF(const Vector3 &normal, const Vector3 &in, const Vector3 &out)
	{
		return 0.0f;
	}
};

class Lambert : public Material
{
	virtual float BRDF(const Vector3 &normal, const Vector3 &in, const Vector3 &out)
	{
		float lambert = normal.dot(in);
		if (lambert < 0.0f)
			lambert = 0.0f;

		return lambert * Average(this->diffuse);
	}
};


#endif /* _MATERIAL_H_ */
