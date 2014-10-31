

#ifndef _camera_h_
#define _camera_h_



class Camera
{
private:
	Vector3 eye;
	Vector3 look;
	Vector3 up;

	float fov;
	float znear;
	float zfar;

	Matrix4 view;
	Matrix4 projection;

public:
	Camera(Vector3 _eye, Vector3 _look, Vector3 _up);
	~Camera() = default;
	
};



#endif /* _CAMERA_H_ */
