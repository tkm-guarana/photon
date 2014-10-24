
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../fw/fw_matrix.h"
#include "utility.h"


namespace pm
{

// ��[0,1]�̗�������
float random()
{
//	return (float)rand() / RAND_MAX;
	return (float)genrand_real1();
}

// �~�ƒ����̌�_�ōł��߂��l��񎟕������ŋ��߂�
float collide(const fw::Vector3 *center, float radious, const fw::Vector3 *base, const fw::Vector3 *dir)
{
	fw::Vector3 v1;
	fw::Vector3Subtract(&v1, base, center);
	float s1 = fw::Vector3Dot(dir, &v1); // dir(base - center)

	float len1 = fw::Vector3Length(dir);
	len1 = len1 * len1; // squared length
	float len2 = fw::Vector3Length(&v1);

	float D = s1*s1 - len1 * (len2*len2 - radious*radious); // ���ʎ�

	if(D < 0) // �𖳂�
	{
		return -1.0f;
	}

	float t1 = (-s1 + sqrt(D)) / len1;
	float t2 = (-s1 - sqrt(D)) / len1;

	float t = -1.0f;

 	if(t1 > 0)
	{
		if(t2 > 0) t = (t1 < t2) ? t1 : t2;
		else t = t1;
	}
	else
	{
		if(t2 > 0) t = t2;
	}

	return t;
}

// �O�p�`�ƒ����̌�_�����߂�
float collide(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, const fw::Vector3 *base, const fw::Vector3 *dir)
{
	fw::Vector3 normal; // �@���v�Z
	{
		fw::Vector3 line0, line1;
		fw::Vector3Subtract(&line0, v2, v0);
		fw::Vector3Subtract(&line1, v1, v0);
		fw::Vector3Cross(&normal, &line0, &line1);
		fw::Vector3Normalize(&normal);
	}

	float s1 = fw::Vector3Dot(dir, &normal);
	if(s1 < 0.00001f && s1 > -0.00001f)
	{
		return -1.0f; // �قڕ��s
	}

	float t = 0.0f;
	{
		fw::Vector3 sub;
		fw::Vector3Subtract(&sub, v0, base);

		t = fw::Vector3Dot(&sub, &normal) / s1; // ��_�����߂�

		if(t < 0)
		{
			return -1.0f; // ���ɂ���
		}
	}

	fw::Vector3 pos = *base + *dir * t; // ��_�Z�o
	{
		fw::Vector3 d0;
		fw::Vector3 d1;
		fw::Vector3 crs;
		float dot = 0.0f;

		d0 = pos - *v0;
		d1 = *v1 - *v0;
		fw::Vector3Cross(&crs, &d0, &d1);
		dot = fw::Vector3Dot(&crs, &normal);
		if(dot < 0.0f)
		{
			return -1.0f;
		}
		d0 = pos - *v1;
		d1 = *v2 - *v1;
		fw::Vector3Cross(&crs, &d0, &d1);
		dot = fw::Vector3Dot(&crs, &normal);
		if(dot < 0.0f)
		{
			return -1.0f;
		}
		d0 = pos - *v2;
		d1 = *v0 - *v2;
		fw::Vector3Cross(&crs, &d0, &d1);
		dot = fw::Vector3Dot(&crs, &normal);
		if(dot < 0.0f)
		{
			return -1.0f;
		}
	}

	return t;
}

void reflect(const fw::Vector3 *center, const fw::Vector3 *intersection, fw::Vector3 *dir)
{
	fw::Vector3 normal = *intersection - *center;

	*dir -= (normal * fw::Vector3Dot(&normal, dir) * 2); // ���˕������v�Z
}

// �O�p�`�̔��˃x�N�g���v�Z
void reflect(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, fw::Vector3 *dir)
{
	fw::Vector3 normal; // �@���v�Z

	fw::Vector3 line0, line1;
	fw::Vector3Subtract(&line0, v2, v0);
	fw::Vector3Subtract(&line1, v1, v0);
	fw::Vector3Cross(&normal, &line0, &line1);
	fw::Vector3Normalize(&normal);

	*dir -= normal * fw::Vector3Dot(&normal, dir) * 2; // ���˕������v�Z

}

// ���˃x�N�g���Ɩ@��������܃x�N�g�����쐬
// ��1�A��2�͋��ܗ�
void refract(fw::Vector3 *dest, const fw::Vector3 *in, const fw::Vector3 *normal, float eta1, float eta2)
{
	float ln = fw::Vector3Dot(in, normal);
	float eta = eta1 / eta2; // ��

	float term = -sqrt(1.0f - eta*eta*(1.0f - ln*ln));
	*dest = *normal * term - (*in - *normal * ln) * eta;
}



// �@�������Ƃ������̕ϊ�
void ortho_basis(fw::Matrix4 *basis, const fw::Vector3 *normal)
{
	fw::Matrix4Identity(basis);
	fw::Vector3 ax, ay(*normal), az;
	fw::Vector3 v;
	if(-0.6f < normal->x && normal->x < 0.6f)
	{
		v = fw::Vector3(1.0f, 0.0f, 0.0f);
	}
	else if(-0.6f < normal->y && normal->y < 0.6f)
	{
		v = fw::Vector3(0.0f, 1.0f, 0.0f);
	}
	else if(-0.6f < normal->z && normal->z < 0.6f)
	{
		v = fw::Vector3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		v = fw::Vector3(1.0f, 0.0f, 0.0f);
	}

	fw::Vector3Cross(&ax, &v, &ay);
	fw::Vector3Normalize(&ax);
	fw::Vector3Cross(&az, &ax, &ay);

	basis->_11 = ax.x;
	basis->_12 = ay.x;
	basis->_13 = az.x;
	basis->_21 = ax.y;
	basis->_22 = ay.y;
	basis->_23 = az.y;
	basis->_31 = ax.z;
	basis->_32 = ay.z;
	basis->_33 = az.z;
}


}
