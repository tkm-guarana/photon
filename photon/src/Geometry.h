
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

typedef Eigen::Matrix<float, 3, 1> Color;
typedef Eigen::Matrix<float, 3, 1> Vector3;
typedef Eigen::Matrix<float, 4, 1> Vector4;
typedef Eigen::Matrix<float, 4, 4> Matrix4;

void Matrix4ViewL(Matrix4 &mat,
				  const Vector3 &eye,
				  const Vector3 &look,
				  const Vector3 &up);

void Matrix4PerspectiveL(Matrix4 &mat,
						 float fov,
						 float aspect,
						 float znear,
						 float zfar);

void Matrix4Screen(Matrix4 &mat,
				   float width,
				   float height);

void Matrix4OrthoBasis(Matrix4 &basis,
					   const Vector3 &normal);

void Vector3TransformCoordinate(Vector3 &v,
								Matrix4 &m);

inline float Average(Vector3 &v)
{
	return (v(0) + v(1) + v(2)) / 3.0f;
}

#endif /* _GEOMETRY_H_ */
