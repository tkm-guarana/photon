
#ifndef _photon_trancer_h_
#define _photon_trancer_h_

#include "../fw/fw_matrix.h"
#include "object.h"
#include "kd_tree.h"

namespace pm
{



class Tracer
{
private:
	fw::Matrix4 world_matrix;
	fw::Matrix4 view_matrix;
	fw::Matrix4 proj_matrix;
	fw::Matrix4 affine;

	fw::Vector3 eye_vec;
	fw::Vector3 look_vec;
	fw::Vector3 up_vec;

	fw::Vector3 temp_normal; // クラス同士の依存がややこしくなってきた、、

	ObjCont	&objects;
	KDTree	&tree;
private:
	struct thread_param
	{
		Tracer *inst;
		dword *pixel;
		fw::Vector3 eye;
		fw::Matrix4 affine;
		int sx, dx;
		int sy, dy;
	};
	float detect_nearest_object(const fw::Vector3 &base, const fw::Vector3 &dir);
	bool trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &rgb, uint depth);
public:
	Tracer(ObjCont &object_list, KDTree &kdtree) : objects(object_list), tree(kdtree) {}
	virtual ~Tracer() {}

	virtual void render(void *buffer, int width, int height);
	virtual float shade() { return 0.0f; }
	virtual void transform(const fw::Matrix4 &world, const fw::Matrix4 &proj);
	virtual void camera(const fw::Vector3 &eye, const fw::Vector3 &look, const fw::Vector3 &up);

	static void thread(void *param);
};



}

#endif