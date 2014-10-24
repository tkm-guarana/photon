

#include <assert.h>
#include <windows.h>

#include "tracer.h"

#undef min
#undef max

namespace pm
{

// スレッド関数
void Tracer::thread(void *param)
{
	thread_param *p = (thread_param*)param;
#if 0 // アンチエイリアスをかけない？
	for(int y = p->sy; y < p->sy + p->dy; y++)
	{
		for(int x = p->sx; x < p->sx + p->dx; x++)
		{
			fw::Vector4 end(float(x), float(y), 0, 1.0f);
			fw::Vector4Transform(&end, &p->affine);
			fw::Vector4Scale(&end, &end, 1.0f / end.w);

			fw::Vector3 dir((end - p->eye).v); // 視線ベクトル
			fw::Vector3Normalize(&dir);

			fw::Vector3 color(0.1f, 0.1f, 0.1f);
			float t = p->inst->detect_nearest_object(p->eye, dir);
			if(t > 0)
			{
				p->inst->trace(p->eye, dir, color, 0);
			}
			unsigned char r = unsigned char(std::min(color.x, 1.0f) * 255);
			unsigned char g = unsigned char(std::min(color.y, 1.0f) * 255);
			unsigned char b = unsigned char(std::min(color.z, 1.0f) * 255);
			p->pixel[y*640+x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
#else
	float ax[] = {-0.25f, 0.0f, 0.25f, 0.0f};
	float ay[] = {0.0f, -0.25f, 0.0f, 0.25f};

	for(int y = p->sy; y < p->sy + p->dy; y++)
	{
		for(int x = p->sx; x < p->sx + p->dx; x++)
		{
			fw::Vector3 aa_color(0.0f, 0.0f, 0.0f);
			for(int i = 0; i < 4; i++)
			{
				float xx = float(x) + ax[i];
				float yy = float(y) + ay[i];
				fw::Vector4 end(xx, yy, 0, 1.0f);
				fw::Vector4Transform(&end, &p->affine);
				fw::Vector4Scale(&end, &end, 1.0f / end.w);

				fw::Vector3 dir((end - p->eye).v); // 視線ベクトル
				fw::Vector3Normalize(&dir);

				fw::Vector3 color(0.1f, 0.1f, 0.1f);
				float t = p->inst->detect_nearest_object(p->eye, dir);
				if(t > 0)
				{
					p->inst->trace(p->eye, dir, color, 0);
				}
				aa_color += color;
			}
			aa_color /= 4.0f;
			unsigned char r = unsigned char(std::min(aa_color.x, 1.0f) * 255);
			unsigned char g = unsigned char(std::min(aa_color.y, 1.0f) * 255);
			unsigned char b = unsigned char(std::min(aa_color.z, 1.0f) * 255);
			p->pixel[y*640+x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
#endif
}
/*
void Tracer::render(void *buffer, int width, int height)
{
	assert(buffer);

	HANDLE	thread[2];

	dword *pixel = (dword*)buffer;

//	fw::Matrix4 view, inverse;
//	fw::Matrix4Screen(&inverse, (float)width, (float)height);
//	view = affine * inverse; // スクリーンまで含めた変換行列
//	fw::Matrix4Inverse(&inverse, &view); // 変換行列
//	fw::Matrix4Inverse(&view, &view_matrix);

	fw::Matrix4 wi, vi, pi, si, scr;
	fw::Matrix4Inverse(&wi, &world_matrix);
	fw::Matrix4Inverse(&vi, &view_matrix);
	fw::Matrix4Inverse(&pi, &proj_matrix);
	fw::Matrix4Screen(&scr, (float)width, (float)height);
	fw::Matrix4Inverse(&si, &scr); 

	fw::Vector4 begin(0.0f, 0.0f, 0.0f, 1.0f);

//	fw::Vector4Transform(&begin, &view);
	fw::Vector4Transform(&begin, &vi);
	fw::Vector4Transform(&begin, &wi);
	fw::Vector4Scale(&begin, &begin, 1.0f / begin.w);
	fw::Vector3 base(begin.x, begin.y, begin.z);

	fw::Matrix4 aff;
	aff = si * pi * vi * wi;

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			fw::Vector4 end(float(x), float(y), 0, 1.0f);
//			fw::Vector4Transform(&end, &inverse);
//			fw::Vector4Transform(&end, &si);
//			fw::Vector4Transform(&end, &pi);
//			fw::Vector4Transform(&end, &vi);
//			fw::Vector4Transform(&end, &wi);
			fw::Vector4Transform(&end, &aff);
			fw::Vector4Scale(&end, &end, 1.0f / end.w);

			fw::Vector3 dir((end - begin).v); // 視線ベクトル
			fw::Vector3Normalize(&dir);


			fw::Vector3 color(0.1f, 0.1f, 0.1f);
			float t = detect_nearest_object(base, dir);
			if(t > 0)
			{
				//tree.find_range(base, 1.0f, 50);
//				fw::Vector3 cp = base + dir * t;
//				color = tree.radiance(cp, 0.5f, 100, temp_normal);
//				color.x = 0.3f;
//				color.y = 0.2f;
//				color.z = 0.7f;
				
				trace(base, dir, color, 0);
//				color /= 10;
			}
			unsigned char r = unsigned char(std::min(color.x, 1.0f) * 255);
			unsigned char g = unsigned char(std::min(color.y, 1.0f) * 255);
			unsigned char b = unsigned char(std::min(color.z, 1.0f) * 255);
			pixel[y*640+x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}
}
*/

#define ThreadMax	2

void Tracer::render(void *buffer, int width, int height)
{
	assert(buffer);

	HANDLE	thread[ThreadMax];

	fw::Matrix4 wi, vi, pi, si, scr;
	fw::Matrix4Inverse(&wi, &world_matrix);
	fw::Matrix4Inverse(&vi, &view_matrix);
	fw::Matrix4Inverse(&pi, &proj_matrix);
	fw::Matrix4Screen(&scr, (float)width, (float)height);
	fw::Matrix4Inverse(&si, &scr); 

	fw::Matrix4 aff;
	aff = si * pi * vi * wi;

	fw::Vector4 begin(0.0f, 0.0f, 0.0f, 1.0f);

	fw::Vector4Transform(&begin, &vi);
	fw::Vector4Transform(&begin, &wi);
	fw::Vector4Scale(&begin, &begin, 1.0f / begin.w);
	fw::Vector3 base(begin.x, begin.y, begin.z);

	assert(width % 4 == 0);
	assert(height % 4 == 0);

	int div_x = width / 4;
	int div_y = height / 4;

	thread_param param[ThreadMax];
	for(int i = 0; i < ThreadMax; i++)
	{
		param[i].inst = this;
		param[i].eye = base;
		param[i].pixel = (dword*)buffer;
		param[i].affine = aff;
		param[i].dx = div_x;
		param[i].dy = div_y;
	}

	std::vector<POINT>	ques;
	// キュー作成
	for(int y = 0; y < 4; y++)
	{
		for(int x = 0; x < 4; x++)
		{
			POINT sp;
			sp.x = x * div_x;
			sp.y = y * div_y;
			ques.push_back(sp);
		}
	}

	std::vector<DWORD>	timelist;

	for(size_t i = 0; i < ques.size();)
	{
		DWORD st = timeGetTime();

		for(int j = 0; j < ThreadMax; j++)
		{
			param[j].sx = (int)ques[i].x;
			param[j].sy = (int)ques[i].y;
			thread[j] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Tracer::thread, (void*)&param[j], 0, NULL);
			i++;
		}

		WaitForMultipleObjects(ThreadMax, thread, TRUE, INFINITE);
		DWORD et = timeGetTime();
		et -= st;
		timelist.push_back(et);
//		MessageBox(NULL, "advanced", "thread", MB_OK);
	}

	char filename[128];
	sprintf(filename, "thread_%d_time.txt", ThreadMax);
	FILE *fp = fopen(filename, "w");

	DWORD total = 0;
	for(size_t i = 0; i < timelist.size(); i++)
	{
		fprintf(fp, "Block %d, time %d [ms]\n", i, timelist[i]);
		total += timelist[i];
	}

	DWORD ave = total / 16;
	fprintf(fp, "total time %d [ms]\n", total);
	fprintf(fp, "average time %d [ms]\n", ave);

	fclose(fp);
}


void Tracer::transform(const fw::Matrix4 &world, const fw::Matrix4 &proj)
{
	world_matrix = world;
//	view_matrix = view;
	proj_matrix = proj;

	affine = world;
	affine *= view_matrix;
	affine *= proj;
}

void Tracer::camera(const fw::Vector3 &eye, const fw::Vector3 &look, const fw::Vector3 &up)
{
	eye_vec = eye;
	look_vec = look;
	up_vec = up;

	fw::Matrix4ViewL(&view_matrix, &eye, &look, &up);

	affine = world_matrix;
	affine *= view_matrix;
	affine *= proj_matrix;
}

float Tracer::detect_nearest_object(const fw::Vector3 &base, const fw::Vector3 &dir)
{
	float dist = 100000.0f;
	int n = -1;
	for(size_t i = 0; i < objects.size(); i++)
	{
		float t = objects[i]->distance(base, dir);
		if(t > 0 && t < dist)
		{
			dist = t;
			n = (int)i;
		}
	}

	if(n < 0)
	{
		dist = -1.0f;
		temp_normal = fw::Vector3();
	}
	else
	{
		fw::Vector3 intersect = base + dir * dist;
		temp_normal = objects[n]->normal(intersect);
	}

	return dist;
}

bool Tracer::trace(fw::Vector3 &base, fw::Vector3 &dir, fw::Vector3 &rgb, uint depth)
{
	if(depth > 3)
	{	// 此処で終わり
		rgb = fw::Vector3(0.0f, 0.0f, 0.0f);
		return false;
	}
	float dist = 100000.0f;
	int n = -1;	// 超ゴリ押し線形検索
	for(size_t i = 0; i < objects.size(); i++)
	{
		float t = objects[i]->distance(base, dir);
		if(t > 0 && t < dist)
		{
			dist = t;
			n = (int)i;
		}
	}

	if(n < 0)
	{	// 何れにも当たらない
		rgb = fw::Vector3(0.0f, 0.0f, 0.0f);
		dist = 0.0f;
		return false;
	}

	fw::Vector3 diffuse_rgb;
	fw::Vector3 specular_rgb;
	fw::Vector3 refraction_rgb;
	Material *mtrl = objects[n]->material();
	float diffuse_reflectance = average(mtrl->diffuse);
	float specular_reflectance = average(mtrl->specular);
	float refractive_index = average(mtrl->refraction);

	{
		fw::Vector3 intersect = base;
		fw::Vector3 reflect = dir;
		if(diffuse_reflectance > 0.0f) // 拡散反射面
		{
			if(dist > 0)
			{
				intersect += reflect * dist;
				diffuse_rgb = tree.radiance(intersect, 0.5f, 500, objects[n]->normal(intersect), mtrl);
				//diffuse_rgb = tree.radiance(intersect, 0.5f, 50, objects[n]->normal(intersect), mtrl);
			}
		}
	}

	{
		fw::Vector3 intersect = base;
		fw::Vector3 reflect = dir;
		if(specular_reflectance > 0.0f) // 鏡面反射面
		{
			if(dist > 0)
			{
				//intersect += reflect * dist + objects[n]->normal * 0.00001f;
				objects[n]->trace(intersect, reflect, fw::Vector3()); // 移動する
				intersect += objects[n]->normal(intersect) * 0.00001;
				trace(intersect, reflect, specular_rgb, depth+1);
			}
		}
	}

	{
		fw::Vector3 intersect = base;
		fw::Vector3 ref = dir;
		if(refractive_index > 0.0f) // 透過
		{
			if(dist > 0)
			{
				objects[n]->refract(intersect, ref); // 移動する
				//intersect -= objects[n]->normal(intersect) * 0.00001;
				trace(intersect, ref, refraction_rgb, depth+1);
			}
		}
	}

	rgb = diffuse_rgb + specular_rgb + refraction_rgb;

//	rgb = diffuse_rgb * diffuse_reflectance + specular_rgb * specular_reflectance;

	return true;
}

}