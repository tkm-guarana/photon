
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../fw/fw_matrix.h"
#include "utility.h"


namespace pm
{

// δ∈[0,1]の乱数生成
float random()
{
//	return (float)rand() / RAND_MAX;
	return (float)genrand_real1();
}

// 円と直線の交点で最も近い値を二次方程式で求める
float collide(const fw::Vector3 *center, float radious, const fw::Vector3 *base, const fw::Vector3 *dir)
{
	fw::Vector3 v1;
	fw::Vector3Subtract(&v1, base, center);
	float s1 = fw::Vector3Dot(dir, &v1); // dir(base - center)

	float len1 = fw::Vector3Length(dir);
	len1 = len1 * len1; // squared length
	float len2 = fw::Vector3Length(&v1);

	float D = s1*s1 - len1 * (len2*len2 - radious*radious); // 判別式

	if(D < 0) // 解無し
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

// 三角形と直線の交点を求める
float collide(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, const fw::Vector3 *base, const fw::Vector3 *dir)
{
	fw::Vector3 normal; // 法線計算
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
		return -1.0f; // ほぼ平行
	}

	float t = 0.0f;
	{
		fw::Vector3 sub;
		fw::Vector3Subtract(&sub, v0, base);

		t = fw::Vector3Dot(&sub, &normal) / s1; // 交点を求める

		if(t < 0)
		{
			return -1.0f; // 後ろにある
		}
	}

	fw::Vector3 pos = *base + *dir * t; // 交点算出
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

	*dir -= (normal * fw::Vector3Dot(&normal, dir) * 2); // 反射方向を計算
}

// 三角形の反射ベクトル計算
void reflect(const fw::Vector3 *v0, const fw::Vector3 *v1, const fw::Vector3 *v2, fw::Vector3 *dir)
{
	fw::Vector3 normal; // 法線計算

	fw::Vector3 line0, line1;
	fw::Vector3Subtract(&line0, v2, v0);
	fw::Vector3Subtract(&line1, v1, v0);
	fw::Vector3Cross(&normal, &line0, &line1);
	fw::Vector3Normalize(&normal);

	*dir -= normal * fw::Vector3Dot(&normal, dir) * 2; // 反射方向を計算

}

// 入射ベクトルと法線から屈折ベクトルを作成
// η1、η2は屈折率
void refract(fw::Vector3 *dest, const fw::Vector3 *in, const fw::Vector3 *normal, float eta1, float eta2)
{
	float ln = fw::Vector3Dot(in, normal);
	float eta = eta1 / eta2; // 比

	float term = -sqrt(1.0f - eta*eta*(1.0f - ln*ln));
	*dest = *normal * term - (*in - *normal * ln) * eta;
}



// 法線を軸とした基底の変換
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
