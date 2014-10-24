

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
public:
	Sphere() : position(0.0f, 0.0f, 0.0f), radious(1.0f) {}
	Sphere(const fw::Vector3 &pos, float r) : position(pos), radious(r) {}
	virtual ~Sphere() {}

	virtual float distance(const fw::Vector3 &base, const fw::Vector3 &dir) const;
	virtual bool photon(Photon &photon);
	virtual bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum);
	virtual fw::Vector3 normal(const fw::Vector3 &intersection);
};

float Sphere::distance(const fw::Vector3 &base, const fw::Vector3 &dir) const
{
	return collide(&position, radious, &base, &dir);
}

// 戻り値 反射、透過 true 吸収 false
bool Sphere::photon(Photon &photon)
{
	float t = collide(&position, radious, &photon.position, &photon.direction);
	if(t < 0)
	{
		return false; // 交差しない
	}

	// 材質により反射、透過、吸収のいずれかを選択
	// 反射ならば以下のコードを実行する

	photon.position += photon.direction * t; // 移動する

	// spectrumの変化を計算する

	float p = random();
	if(p < 0.3) // 反射
	{
		fw::Vector3 normal = photon.position - position;
	//	fw::Vector3Subtract(&normal, &photon.position, &position);
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // 反射方向を計算
		
		return true;
	}
	else // 吸収
	{
		// 以後入射方向として扱う incidence
		// directionで代用すればいいか？
	//	photon.direction = photon.direction * -1;
//		photon.incidence = photon.direction * -1; // 後でunary oeperator作れ

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

		photon.spectrum -= fw::Vector3(0.2f, 0.8f, 0.7f); 
		return true;
	}


	return true;
}

bool Sphere::trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum)
{
	float t = collide(&position, radious, &base, &dir);
	if(t < 0)
	{
		return false; // 交差しない
	}

	base += dir * t; // 移動する

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

Object* CreateSphere(const fw::Vector3 &pos, float radious)
{
	return new Sphere(pos, radious);
}



// 三角形
class Triangle : public Object
{
private:
	fw::Vector3 v0, v1, v2;
public:
	Triangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2) : v0(vtx0), v1(vtx1), v2(vtx2) {}

	virtual ~Triangle() {}

	virtual float distance(const fw::Vector3 &base, const fw::Vector3 &dir) const;
	virtual bool photon(Photon &photon);
	virtual bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum);
	virtual fw::Vector3 normal(const fw::Vector3 &intersection);
};

float Triangle::distance(const fw::Vector3 &base, const fw::Vector3 &dir) const
{
	return collide(&v0, &v1, &v2, &base, &dir);
}

bool Triangle::photon(Photon &photon)
{
	float t = collide(&v0, &v1, &v2, &photon.position, &photon.direction);
	if(t < 0)
	{
		return false; // 交差しない
	}

	// 材質により反射、透過、吸収のいずれかを選択
	// 反射ならば以下のコードを実行する

	photon.position += photon.direction * t; // 移動する

	// spectrumの変化を計算する

	float p = random();
	if(p < 0.8) // 反射
	{
		fw::Vector3 normal; // 法線計算
		{
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
		}
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // 反射方向を計算

		/*
		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);

		fw::Vector3Normalize(&photon.direction);
		*/
		return true;
	}
	else // 吸収
	{
		// photon.direction = photon.direction * -1;
//		photon.incidence = photon.direction * -1; // いらないかも・・・

		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);

		fw::Vector3Normalize(&photon.direction);

		fw::Matrix4 basis;
		{
			fw::Vector3 normal; // 法線計算
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
			fw::Vector3Normalize(&normal);

			pm::ortho_basis(&basis, &normal); // 法線を軸とする正規直交基底
		}
		fw::Vector3TransformCoordinate(&photon.direction, &basis);

		photon.spectrum -= fw::Vector3(0.7f, 0.8f, 0.2f); 
		return false;
	}


	return true;
}

bool Triangle::trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &spectrum)
{
	float t = collide(&v0, &v1, &v2, &base, &dir);
	if(t < 0)
	{
		return false; // 交差しない
	}

	base += dir * t; // 移動する

	// 反射ベクトルに書き換える
	reflect(&v0, &v1, &v2, &dir);

	return true;
}

fw::Vector3 Triangle::normal(const fw::Vector3 &intersection)
{
	fw::Vector3 n; // 法線計算

	fw::Vector3 line0, line1;
	fw::Vector3Subtract(&line0, &v2, &v0);
	fw::Vector3Subtract(&line1, &v1, &v0);
	fw::Vector3Cross(&n, &line0, &line1);
	fw::Vector3Normalize(&n);

	return n;
}

Object* CreateTriangle(const fw::Vector3 &vtx0, const fw::Vector3 &vtx1, const fw::Vector3 &vtx2)
{
	return new Triangle(vtx0, vtx1, vtx2);
}





}