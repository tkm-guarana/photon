

#ifndef _camera_h_
#define _camera_h_



class Camera
{
private:
	Vector3 eye;
	Vector3 look;
	Vector3 up;

public:
	Camera() = default;
	~Camera() = default;

};



#endif /* _CAMERA_H_ */
