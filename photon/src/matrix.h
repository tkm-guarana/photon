
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <math.h>
#include <iostream>

#if 1
#define INLINE inline
#else
#define INLINE
#endif



// Matrix

struct Matrix3
{
	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float m[3][3];
		float v[9];
	};

#if defined(__cplusplus)

	Matrix3() : _11(0.0f), _12(0.0f), _13(0.0f),
				 _21(0.0f), _22(0.0f), _23(0.0f),
				 _31(0.0f), _32(0.0f), _33(0.0f) {}

	Matrix3(float m11, float m12, float m13,
			 float m21, float m22, float m23,
			 float m31, float m32, float m33) :
			 _11(m11), _12(m12), _13(m13),
			 _21(m21), _22(m22), _23(m23),
			 _31(m31), _32(m32), _33(m33) {}

	void identity()
	{
		_11 = 1; _12 = 0; _13 = 0;
		_21 = 0; _22 = 1; _23 = 0;
		_31 = 0; _32 = 0; _33 = 1;
	}

#endif

};

struct Matrix4
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
		float v[16];
	};

#if defined(__cplusplus)

	Matrix4() :	_11(0.0f), _12(0.0f), _13(0.0f), _14(0.0f),
				_21(0.0f), _22(0.0f), _23(0.0f), _24(0.0f),
				_31(0.0f), _32(0.0f), _33(0.0f), _34(0.0f),
				_41(0.0f), _42(0.0f), _43(0.0f), _44(0.0f) {}

	Matrix4(float m11, float m12, float m13, float m14,
			 float m21, float m22, float m23, float m24,
			 float m31, float m32, float m33, float m34,
			 float m41, float m42, float m43, float m44) :
			 _11(m11), _12(m12), _13(m13), _14(m14),
			 _21(m21), _22(m22), _23(m23), _24(m24),
			 _31(m31), _32(m32), _33(m33), _34(m34),
			 _41(m41), _42(m42), _43(m43), _44(m44) {}

	Matrix4 operator+(const Matrix4 &m)
	{
		Matrix4 mm;
		for(int i = 0; i < 16; i++) mm.v[i] = v[i] + m.v[i];
		return mm;
	}

	Matrix4 operator-(const Matrix4 &m)
	{
		Matrix4 mm;
		for(int i = 0; i < 16; i++) mm.v[i] = v[i] - m.v[i];
		return mm;
	}

	Matrix4 operator*(const Matrix4 &m)
	{
		Matrix4 mm;
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				mm.v[j*4+i] = v[j*4]*m.v[i] + v[j*4+1]*m.v[4+i] + v[j*4+2]*m.v[8+i] + v[j*4+3]*m.v[12+i];
			}
		}
		return mm;
	}

	Matrix4 operator*(float f)
	{
		Matrix4 mm;
		for(int i = 0; i < 16; i++) mm.v[i] = v[i]*f;
		return mm;
	}

	Matrix4 operator*=(const Matrix4 &m)
	{
		return (*this) = (*this)*m;
	}

	void identity()
	{
		_11 = 1; _12 = 0; _13 = 0; _14 = 0;
		_21 = 0; _22 = 1; _23 = 0; _24 = 0;
		_31 = 0; _32 = 0; _33 = 1; _34 = 0;
		_41 = 0; _42 = 0; _43 = 0; _44 = 1;
	}

	void toStream(std::ostream& os)
	{
		std::ios::fmtflags temp = os.setf(std::ios::scientific, std::ios::floatfield | std::ios::showpos);
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				os << m[i][j] << ", ";
			}
			os << std::endl;
		}
		os.setf(temp);
	}

#endif

};




// Vector

struct Vector3
{
	union
	{
		struct { float x, y, z; };
		float v[3];
	};

#if defined(__cplusplus)

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	Vector3(float vv[]) : x(vv[0]), y(vv[1]), z(vv[2]) {}

	Vector3 operator+(const Vector3 &v) const	{ return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator-(const Vector3 &v)	const	{ return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator*(float f) const			{ return Vector3(x * f, y * f, z * f); }
	Vector3 operator/(float f) const			{ return Vector3(x / f, y / f, z / f); }
	Vector3 operator+=(const Vector3 &v)		{ x += v.x; y += v.y; z += v.z; return *this; }
	Vector3 operator-=(const Vector3 &v)		{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3 operator*=(float f)					{ x *= f; y *= f; z *= f; return *this; }
	Vector3 operator/=(float f)					{ x /= f; y /= f; z /= f; return *this; }

	bool operator==(const Vector3 &v) const		{ return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const Vector3 &v) const		{ return x != v.x || y != v.y || z != v.z; }

	float norm() { return sqrt(x*x+y*y+z*z); } // 長さ
	void normalize() { float l = norm(); x /= l; y /= l; z /= l; } // 正規化

	void toStream(std::ostream& os)
	{
		os << "v(" << x << "," << y << "," << z << ")"  << std::endl;
	}

#endif

};

// Vector3 predicate
// stlでソートする時とか
struct Vector3LessX
{
	bool operator()(const Vector3 &lhs, const Vector3 &rhs)
	{
		return lhs.x < rhs.x;
	}
};
struct Vector3LessY
{
	bool operator()(const Vector3 &lhs, const Vector3 &rhs)
	{
		return lhs.y < rhs.y;
	}
};
struct Vector3LessZ
{
	bool operator()(const Vector3 &lhs, const Vector3 &rhs)
	{
		return lhs.z < rhs.z;
	}
};


struct Vector4
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};

#if defined(__cplusplus)

	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vector4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
	Vector4(float vv[]) : x(vv[0]), y(vv[1]), z(vv[2]), w(vv[3]) {}
	Vector4(const Vector3 vv) : x(vv.x), y(vv.y), z(vv.z), w(0.0f) {}

	Vector4 operator+(const Vector4 &v) const	{ return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4 operator-(const Vector4 &v)	const	{ return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4 operator*(float f) const			{ return Vector4(x * f, y * f, z * f, w * f); }
	Vector4 operator/(float f) const			{ return Vector4(x / f, y / f, z / f, w * f); }
	Vector4& operator+=(const Vector4 &v)		{ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	Vector4& operator-=(const Vector4 &v)		{ x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	Vector4& operator*=(float f)					{ x *= f; y *= f; z *= f; w *= f; return *this; }
	Vector4& operator/=(float f)					{ x /= f; y /= f; z /= f; w /= f; return *this; }

	bool operator==(const Vector4 &v) const		{ return x == v.x && y == v.y && z == v.z && w == v.w; }
	bool operator!=(const Vector4 &v) const		{ return x != v.x || y != v.y || z != v.z || w != v.w; }

	float norm() { return sqrt(x*x + y*y + z*z + w*w); }

	Vector4 operator*(const Matrix4 &m)
	{
		return Vector4(	x*m._11 + y*m._12 + z*m._13 + w*m._14,
						x*m._21 + y*m._22 + z*m._23 + w*m._24,
						x*m._31 + y*m._32 + z*m._33 + w*m._34,
						x*m._41 + y*m._42 + z*m._43 + w*m._44);
	}

	Vector4& operator*=(const Matrix4 &m) { *this = *this * m; return *this; }

	void toStream(std::ostream& os)
	{
		os << "v4(" << x << "," << y << "," << z << "," << w << ")" << std::endl;
	}

#endif

};

struct Quaternion
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};

#if defined(__cplusplus)
	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Quaternion(float xx, float yy, float zz) : x(zz), y(yy), z(zz), w(0.0f) {}
	Quaternion(float xx, float yy, float zz, float ww) : x(zz), y(yy), z(zz), w(ww) {}
	
	void toStream(std::ostream &os)
	{
		os << "quat(" << w << " : " << x << ", " << y << ", " << z << ")" << std::endl;
	}

#endif

};


// Quaternion
INLINE void QuaternoinMultiply(Quaternion *dest, Quaternion *q1, Quaternion *q2)
{
	dest->x = q1->w * q2->x + q2->w * q1->x + (q1->y * q2->z - q1->z * q2->y);
	dest->y = q1->w * q2->y + q2->w * q1->y + (q1->z * q2->x - q1->x * q2->z);
	dest->z = q1->w * q2->z + q2->w * q1->z + (q1->x * q2->y - q1->y * q2->x);
	dest->w = q1->w * q2->w - (q1->x * q2->x + q1->y * q2->y + q1->z * q2->z);
}

INLINE void QuaternionRotationAxis(Quaternion *q, Vector3 *axis, float theta)
{
	q->w = cos(theta/2.0f);
	q->x = axis->x * sin(theta/2.0f);
	q->y = axis->y * sin(theta/2.0f);
	q->z = axis->z * sin(theta/2.0f);
}




// vector3 utility functions

INLINE void Vector3Add(Vector3 *dest, const Vector3 *v1, const Vector3 *v2)
{
	dest->x = v1->x + v2->x;
	dest->y = v1->y + v2->y;
	dest->z = v1->z + v2->z;
}

INLINE void Vector3Subtract(Vector3 *dest, const Vector3 *v1, const Vector3 *v2)
{
	dest->x = v1->x - v2->x;
	dest->y = v1->y - v2->y;
	dest->z = v1->z - v2->z;
}

INLINE float Vector3Length(const Vector3 *v)
{
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

INLINE void Vector3Normalize(Vector3 *v)
{
	float length = Vector3Length(v);
	v->x /= length;
	v->y /= length;
	v->z /= length;	
}

INLINE float Vector3Dot(const Vector3 *v1, const Vector3 *v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

INLINE void Vector3Cross(Vector3 *dest, const Vector3 *v1, const Vector3 *v2)
{
	float x, y, z;
	x = v1->y * v2->z - v1->z * v2->y;
	y = v1->z * v2->x - v1->x * v2->z;
	z = v1->x * v2->y - v1->y * v2->x;
	dest->x = x;
	dest->y = y;
	dest->z = z;
}

INLINE void Vector3Lerp(Vector3 *dest, const Vector3 *v1, const Vector3 *v2, float inc)
{
	float x, y, z;
	x = v1->x + inc * (v2->x - v1->x);
	y = v1->y + inc * (v2->y - v1->y);
	z = v1->z + inc * (v2->z - v1->z);
	dest->x = x;
	dest->y = y;
	dest->z = z;
}

INLINE void Vector3Transform(Vector3 *v, Matrix4 *m)
{
	float x, y, z;
	x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + m->_41; // w = 1とする
	y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + m->_42;
	z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + m->_43;
	v->x = x; // 同次座標がどーのこーの
	v->y = y;
	v->z = z;	
}

INLINE void Vector3TransformCoordinate(Vector3 *v, Matrix4 *m)
{
	float x, y, z, w;
	x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + m->_41; // w = 1とする
	y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + m->_42;
	z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + m->_43;
	w = v->x * m->_14 + v->y * m->_24 + v->z * m->_34 + m->_44;
	v->x = x / w; // 同次座標がどーのこーの
	v->y = y / w;
	v->z = z / w;
}




// vector4 utility functions

INLINE void Vector4Add(Vector4 *dest, Vector4 *v1, Vector4 *v2)
{
	dest->x = v1->x + v2->x;
	dest->y = v1->y + v2->y;
	dest->z = v1->z + v2->z;
	dest->w = v1->w + v2->w;
}

INLINE void Vector4Subtract(Vector4 *dest, Vector4 *v1, Vector4 *v2)
{
	dest->x = v1->x - v2->x;
	dest->y = v1->y - v2->y;
	dest->z = v1->z - v2->z;
	dest->w = v1->w - v2->w;
}

INLINE void Vector4Scale(Vector4 *dest, const Vector4 *src, float mag)
{
	dest->x = src->x * mag;
	dest->y = src->y * mag;
	dest->z = src->z * mag;
	dest->w = src->w * mag;
}

INLINE float Vector4Length(Vector4 *v)
{
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}

INLINE void Vector4Normalize(Vector4 *v)
{
	float length = Vector4Length(v);
	v->x /= length;
	v->y /= length;
	v->z /= length;
	v->w /= length;
}

INLINE float Vector4Dot(Vector4 *v1, Vector4 *v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

INLINE void Vector4Cross(Vector4 *dest, Vector4 *v1, Vector4 *v2)
{ // imcomplete
	dest->x = v1->y * v2->z - v1->z * v2->y;
	dest->y = v1->z * v2->x - v1->x * v2->z;
	dest->z = v1->x * v2->y - v1->y * v2->x;
}

INLINE void Vector4Transform(Vector4 *v, Matrix4 *m)
{
	float x, y, z, w;
	x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + v->w * m->_41;
	y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + v->w * m->_42;
	z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + v->w * m->_43;
	w = v->x * m->_14 + v->y * m->_24 + v->z * m->_34 + v->w * m->_44;
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}




// matrix3 utility functions

INLINE float Matrix3Determinant(Matrix3 *m)
{
	return
	  m->_11 * (m->_22 * m->_33 - m->_23 * m->_32)
	- m->_12 * (m->_21 * m->_33 - m->_23 * m->_31)
	+ m->_13 * (m->_21 * m->_32 - m->_22 * m->_31);
}

// matrix4 utility functions

INLINE void Matrix4Identity(Matrix4 *m)
{
	m->_11 = 1.0f; m->_22 = 1.0f; m->_33 = 1.0f; m->_44 = 1.0f;
	m->_12 = 0.0f; m->_13 = 0.0f; m->_14 = 0.0f;
	m->_21 = 0.0f; m->_23 = 0.0f; m->_24 = 0.0f;
	m->_31 = 0.0f; m->_32 = 0.0f; m->_34 = 0.0f;
	m->_41 = 0.0f; m->_42 = 0.0f; m->_43 = 0.0f;
}

INLINE void Matrix4Transpose(Matrix4 *m)
{
	float temp = 0.0f;
	temp = m->_12; m->_12 = m->_21; m->_21 = temp;
	temp = m->_13; m->_13 = m->_31; m->_31 = temp;
	temp = m->_14; m->_14 = m->_41; m->_41 = temp;
	temp = m->_23; m->_23 = m->_32; m->_32 = temp;
	temp = m->_24; m->_24 = m->_42; m->_42 = temp;
	temp = m->_34; m->_34 = m->_43; m->_43 = temp;
}

INLINE void Matrix4Translation(Matrix4 *m, float x, float y, float z)
{
	Matrix4Identity(m);
	m->_41 = x;
	m->_42 = y;
	m->_43 = z;
}

INLINE void Matrix4RotationX(Matrix4 *m, float theta)
{
	Matrix4Identity(m);
	m->_22 = cos(theta);
	m->_23 = -sin(theta);
	m->_32 = sin(theta);
	m->_33 = cos(theta);
}

INLINE void Matrix4RotationY(Matrix4 *m, float theta)
{
	Matrix4Identity(m);
	m->_11 = cos(theta);
	m->_13 = sin(theta);
	m->_31 = -sin(theta);
	m->_33 = cos(theta);
}

INLINE void Matrix4RotationZ(Matrix4 *m, float theta)
{
	Matrix4Identity(m);
	m->_11 = cos(theta);
	m->_12 = -sin(theta);
	m->_21 = sin(theta);
	m->_22 = cos(theta);
}

INLINE void Matrix4RotationAxis(Matrix4 *m, Vector3 *axis, float theta)
{
	float c = cos(theta);
	float s = sin(theta);

	Matrix4Identity(m);
	m->_11 = axis->x * axis->x * (1.0f - c) + c;
	m->_12 = axis->x * axis->y * (1.0f - c) - axis->z * s;
	m->_13 = axis->x * axis->z * (1.0f - c) + axis->y * s;

	m->_21 = axis->y * axis->x * (1.0f - c) + axis->z * s;
	m->_22 = axis->y * axis->y * (1.0f - c) + c;
	m->_23 = axis->y * axis->z * (1.0f - c) - axis->x * s;

	m->_31 = axis->z * axis->x * (1.0f - c) - axis->y * s;
	m->_32 = axis->z * axis->y * (1.0f - c) + axis->x * s;
	m->_33 = axis->z * axis->z * (1.0f - c) + c;
}

INLINE void Matrix4ViewL(Matrix4 *m, const Vector3 *eye, const Vector3 *look, const Vector3 *up)
{
	Vector3 ax, ay, az;
	az = *look - *eye;
	Vector3Normalize(&az);
	Vector3Cross(&ax, up, &az);
	Vector3Normalize(&ax);
	Vector3Cross(&ay, &az, &ax);

	ax.toStream(std::cout);
	ay.toStream(std::cout);
	az.toStream(std::cout);

	Matrix4Identity(m);
	m->_11 = ax.x;
	m->_12 = ay.x;
	m->_13 = az.x;
	m->_21 = ax.y;
	m->_22 = ay.y;
	m->_23 = az.y;
	m->_31 = ax.z;
	m->_32 = ay.z;
	m->_33 = az.z;
	m->_41 = -Vector3Dot(eye, &ax);
	m->_42 = -Vector3Dot(eye, &ay);
	m->_43 = -Vector3Dot(eye, &az);
}

INLINE void Matrix4PerspectiveL(Matrix4 *m, float fov, float aspect, float znear, float zfar)
{
	Matrix4Identity(m);

	m->_11 = tanf(fov) / aspect;
	m->_22 = tanf(fov);
	m->_33 = zfar / (zfar - znear);
	m->_34 = 1.0f;
	m->_43 = -m->_33 * znear;
	m->_44 = 0.0f;
}

INLINE void Matrix4Screen(Matrix4 *m, float width, float height)
{
	Matrix4Identity(m);

	m->_11 = width / 2.0f;
	m->_22 = -height / 2.0f;
	m->_41 = width / 2.0f;
	m->_42 = height / 2.0f;
}

INLINE float Matrix4Determinant(Matrix4 *m)
{
	// 余因子展開
	return
	m->_11 * (
		  m->_22 * (m->_33 * m->_44 - m->_34 * m->_43)
		- m->_23 * (m->_32 * m->_44 - m->_34 * m->_42)
		+ m->_24 * (m->_32 * m->_43 - m->_33 * m->_42)
		) -
	m->_12 * (
  		  m->_21 * (m->_33 * m->_44 - m->_34 * m->_43)
		- m->_23 * (m->_31 * m->_44 - m->_34 * m->_41)
		+ m->_24 * (m->_31 * m->_43 - m->_33 * m->_41)
		) +
	m->_13 * (
		  m->_21 * (m->_32 * m->_44 - m->_34 * m->_42)
		- m->_22 * (m->_31 * m->_44 - m->_34 * m->_41)
		+ m->_24 * (m->_31 * m->_42 - m->_32 * m->_41)
		) -
	m->_14 * (
		  m->_21 * (m->_32 * m->_43 - m->_33 * m->_42)
		- m->_22 * (m->_31 * m->_43 - m->_33 * m->_41)
		+ m->_23 * (m->_31 * m->_42 - m->_32 * m->_41)
		);
}

// 余因子
INLINE float Matrix4Cofactor(Matrix4 *m, int row, int col)
{
	int r = 0, c = 0;
	Matrix3 dst;

	for(int i = 0; i < 4; i++)
	{
		if(i == col) continue;
		r = 0;
		for(int j = 0; j < 4; j++)
		{
			if(j == row) continue;
			dst.m[c][r] = m->m[i][j];
			r++;
		}
		c++;
	}

	float det = Matrix3Determinant(&dst);
	if((row + col) % 2) // 余因子の符号を決める (-1)^(row + col)
	{
		det *= -1;
	}

	return det;
}

INLINE void Matrix4Inverse(Matrix4 *dest, Matrix4 *src)
{
	// 超力技だけど、、、
	float det = Matrix4Determinant(src);

	if(det >= -0.0000001f && det <= 0.0000001f) return; // 正則ではない、誤差を考慮

	dest->_11 = Matrix4Cofactor(src, 0, 0) / det;
	dest->_12 = Matrix4Cofactor(src, 0, 1) / det;
	dest->_13 = Matrix4Cofactor(src, 0, 2) / det;
	dest->_14 = Matrix4Cofactor(src, 0, 3) / det;

	dest->_21 = Matrix4Cofactor(src, 1, 0) / det;
	dest->_22 = Matrix4Cofactor(src, 1, 1) / det;
	dest->_23 = Matrix4Cofactor(src, 1, 2) / det;
	dest->_24 = Matrix4Cofactor(src, 1, 3) / det;

	dest->_31 = Matrix4Cofactor(src, 2, 0) / det;
	dest->_32 = Matrix4Cofactor(src, 2, 1) / det;
	dest->_33 = Matrix4Cofactor(src, 2, 2) / det;
	dest->_34 = Matrix4Cofactor(src, 2, 3) / det;

	dest->_41 = Matrix4Cofactor(src, 3, 0) / det;
	dest->_42 = Matrix4Cofactor(src, 3, 1) / det;
	dest->_43 = Matrix4Cofactor(src, 3, 2) / det;
	dest->_44 = Matrix4Cofactor(src, 3, 3) / det;
}






#endif
