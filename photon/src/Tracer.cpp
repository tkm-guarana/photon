
#ifndef _tracer_h_
#define _tracer_h_


class Tracer
{
private:
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;

	Vector3 eye;
	Vector3 look;
	Vector3 up;

public:

	Tracer() = default;
	~Tracer() = default;
};


#endif /* _TRACER_H_ */
