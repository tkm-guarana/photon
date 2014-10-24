

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

// �߂�l ���ˁA���� true �z�� false
bool Sphere::photon(Photon &photon)
{
	float t = collide(&position, radious, &photon.position, &photon.direction);
	if(t < 0)
	{
		return false; // �������Ȃ�
	}

	// �ގ��ɂ�蔽�ˁA���߁A�z���̂����ꂩ��I��
	// ���˂Ȃ�Έȉ��̃R�[�h�����s����

	photon.position += photon.direction * t; // �ړ�����

	// spectrum�̕ω����v�Z����

	float p = random();
	if(p < 0.3) // ����
	{
		fw::Vector3 normal = photon.position - position;
	//	fw::Vector3Subtract(&normal, &photon.position, &position);
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // ���˕������v�Z
		
		return true;
	}
	else // �z��
	{
		// �Ȍ���˕����Ƃ��Ĉ��� incidence
		// direction�ő�p����΂������H
	//	photon.direction = photon.direction * -1;
//		photon.incidence = photon.direction * -1; // ���unary oeperator���

		// ������ɔ��˕�����I��
		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);

		fw::Vector3Normalize(&photon.direction);

		// �@���̊��ɕϊ�
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
		return false; // �������Ȃ�
	}

	base += dir * t; // �ړ�����

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



// �O�p�`
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
		return false; // �������Ȃ�
	}

	// �ގ��ɂ�蔽�ˁA���߁A�z���̂����ꂩ��I��
	// ���˂Ȃ�Έȉ��̃R�[�h�����s����

	photon.position += photon.direction * t; // �ړ�����

	// spectrum�̕ω����v�Z����

	float p = random();
	if(p < 0.8) // ����
	{
		fw::Vector3 normal; // �@���v�Z
		{
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
		}
		fw::Vector3Normalize(&normal);
		photon.direction -= normal * fw::Vector3Dot(&normal, &photon.direction) * 2; // ���˕������v�Z

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
	else // �z��
	{
		// photon.direction = photon.direction * -1;
//		photon.incidence = photon.direction * -1; // ����Ȃ������E�E�E

		float theta = acos(sqrt(random()));
		float phai = 2.0f * 3.1415f * random();
		
		photon.direction.x = sin(theta)*cos(phai);
		photon.direction.y = cos(theta);
		photon.direction.z = sin(theta)*sin(phai);

		fw::Vector3Normalize(&photon.direction);

		fw::Matrix4 basis;
		{
			fw::Vector3 normal; // �@���v�Z
			fw::Vector3 line0, line1;
			fw::Vector3Subtract(&line0, &v2, &v0);
			fw::Vector3Subtract(&line1, &v1, &v0);
			fw::Vector3Cross(&normal, &line0, &line1);
			fw::Vector3Normalize(&normal);

			pm::ortho_basis(&basis, &normal); // �@�������Ƃ��鐳�K�������
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
		return false; // �������Ȃ�
	}

	base += dir * t; // �ړ�����

	// ���˃x�N�g���ɏ���������
	reflect(&v0, &v1, &v2, &dir);

	return true;
}

fw::Vector3 Triangle::normal(const fw::Vector3 &intersection)
{
	fw::Vector3 n; // �@���v�Z

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