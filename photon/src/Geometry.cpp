//
//  Geometry.cpp
//  photon
//
//  Created by takemi hirasawa on 2014/10/07.
//  Copyright (c) 2014年 FLAMA Inc. All rights reserved.
//

#include <iostream>

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "Geometry.h"

// ビュー行列作成
void Matrix4ViewL(Matrix4 &mat,
				  const Vector3 &eye,
				  const Vector3 &look,
				  const Vector3 &up)
{
	Vector3 ax, ay, az;
	az = look - eye;
	az.normalize();
	ax = up.cross(az);
	ax.normalize();
	ay = az.cross(ax);

	float dx = -eye.dot(ax);
	float dy = -eye.dot(ay);
	float dz = -eye.dot(az);

	Matrix4 mm;
	mm <<
		ax(0), ay(0), az(0), 0,
		ax(1), ay(1), az(1), 0,
		ax(2), ay(2), az(2), 0,
		dx, dy, dz, 1;

	mat = mm.transpose();
}

// パースペクティブ プロジェクション行列作成
void Matrix4PerspectiveL(Matrix4 &mat,
						 float fov,
						 float aspect,
						 float znear,
						 float zfar)
{
	float z1 = zfar / (zfar - znear);
	float z2 = -z1 * znear;
	float tf = tanf(fov);
	float tfa = tf / aspect;
	Matrix4 mm;
	mm <<
		tfa,  0,  0, 0,
		0,   tf,  0, 0,
		0,    0, z1, 1,
		0,    0, z2, 0;

	mat = mm.transpose();
}

// スクリーン行列作成
void Matrix4Screen(Matrix4 &mat,
				   float width,
				   float height)
{
	float hw = width / 2.0f;
	float hh = height / 2.0f;
	Matrix4 mm;

	mm <<
		hw,  0, 0, 0,
		 0,-hh, 0, 0,
		 0,  0, 1, 0,
		hw, hh, 0, 1;

	
	mat = mm.transpose();
}

// 法線を軸とした基底の変換
void Matrix4OrthoBasis(Matrix4 &basis,
					   const Vector3 &normal)
{
	
	Vector3 ax;
	Vector3 ay(normal);
	Vector3 az;
	Vector3 v;

	if(-0.6f < normal(0) && normal(0) < 0.6f)
	{
		v << 1.0f, 0.0f, 0.0f;
	}
	else if(-0.6f < normal(1) && normal(1) < 0.6f)
	{
		v << 0.0f, 1.0f, 0.0f;
	}
	else if(-0.6f < normal(2) && normal(2) < 0.6f)
	{
		v << 0.0f, 0.0f, 1.0f;
	}
	else
	{
		v << 1.0f, 0.0f, 0.0f;
	}

	ax = v.cross(ay);
	ax.normalize();
	az = ax.cross(ay);

	basis <<
		ax(0), ay(0), az(0), 0.0f,
		ax(1), ay(1), az(1), 0.0f,
		ax(2), ay(2), az(2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f;
}

void Vector3TransformCoordinate(Vector3 &v, Matrix4 &m)
{
	float x, y, z, w;
	x = v(0) * m(0,0) + v(1) * m(1,0) + v(2) * m(2,0) + m(3,0); // w = 1とする
	y = v(0) * m(0,1) + v(1) * m(1,1) + v(2) * m(2,1) + m(3,1);
	z = v(0) * m(0,2) + v(1) * m(1,2) + v(2) * m(2,2) + m(3,2);
	w = v(0) * m(0,3) + v(1) * m(1,3) + v(2) * m(2,3) + m(3,3);
	v <<
		x / w,
		y / w,
		z / w;
}
