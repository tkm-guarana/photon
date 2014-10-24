
#ifndef _material_h_
#define _material_h_

#include "matrix.h"
/* #include "utility.h" */

namespace pm
{

	struct Material
	{
		Vector3 diffuse;	// �g�U����
		Vector3 specular;	// ���ʔ���
		Vector3 refraction;	// ����

		// �K�v�ɉ����č쐬����
		// n	�@��
		// in	���ˊp
		// out	���ˊp
		virtual float brdf(const Vector3 &n, const Vector3 &in, const Vector3 &out) = 0;
	};

#if 0
	// �����o�[�g�g�U����
	struct Lambert : public Material
	{
		virtual float brdf(const Vector3 &n, const Vector3 &in, const Vector3 &out)
		{
			float lambert = Vector3Dot(&n, &in);
			if(lambert < 0.0f)
			{
				lambert = 0.0f;
			}
			return lambert * average(diffuse);
		}
	};


	struct Phong : public Material
	{
		float gloss;

Phong() : gloss(30.0f) {}
		virtual float brdf(const Vector3 &n, const Vector3 &in, const Vector3 &out)
		{
	Vector3 r;

	r = n * Vector3Dot(&n, &in) * 2 - in; // ���˕������v�Z
		
	float rl = Vector3Dot(&r, &in);
	float nl = Vector3Dot(&n, &in);

	if(rl <= 0.0f) return 0.0f;
	if(nl <= 0.0f) return 0.0f;

	return average(diffuse) + average(specular) * (pow(rl, gloss) / nl);
}
};
#endif

}

#endif
