
#if 0
#include <crtdbg.h>
#include <windows.h>
#include <assert.h>
#endif

#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>

// #include "fw/fw_window.h"
// #include "fw/fw_matrix.h"
// #include "fw/fw_renderer.h"
// #include "photon/light.h"
// #include "photon/utility.h"
// #include "photon/kd_tree.h"
// #include "photon/object.h"
// #include "photon/tracer.h"
// #include "photon/material.h"

#include <SDL2/SDL.h>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Geometry"

#include "Geometry.h"
#include "Photon.h"
#include "Material.h"
#include "SceneObject.h"
#include "Light.h"
#include "DimensionalTree.h"
#include "Scene.h"
#include "Canvas.h"
#include "Visualizer.h"

// #include "DimensionalTree.h"

//#define POINT_RENDER 1

#if 0
class MainWindow : public fw::AppWindow
{
private:
	pm::Light* light;
	pm::Light* light2;
	pm::KDTree* tree;
	pm::Tracer* tracer;
	fw::g3d::IRenderer *ren;
	pm::ObjCont objects;

	std::vector<pm::Material*>	mtrl_list;

	HBITMAP	back_buffer1;
	HBITMAP	back_buffer2;
	BOOL	attach_buffer;
	HDC		buffer_dc;
	unsigned long *pixel1;
	unsigned long *pixel2;

	static MainWindow *_inst;

	template<class T> struct PointerDeleter { void operator()(T* pointer) { delete pointer; } };
public:
	MainWindow(HINSTANCE inst) : AppWindow(inst, MainWindow::WinProc), light(NULL), light2(NULL), tree(NULL), ren(NULL), objects(0)
	{
		// 光源生成
		light = pm::CreateSquareLight(fw::Vector3(0.0f, 6.0f, 1.0f), 2.0f, 2.0f);
		light2 = pm::CreatePointLight(fw::Vector3(2.0f, 1.0f, 0.0f));

		// マテリアル生成
		pm::Material *m1 = new pm::Lambert;
		m1->diffuse = fw::Vector3(0.7f, 0.2f, 0.3f);
		m1->specular = fw::Vector3();
		mtrl_list.push_back(m1);
		pm::Material *m2 = new pm::Lambert;
		m2->diffuse = fw::Vector3(0.2f, 0.1f, 0.7f);
		m2->specular = fw::Vector3();
		mtrl_list.push_back(m2);
		pm::Material *m3 = new pm::Lambert;
		m3->diffuse = fw::Vector3(0.0f, 0.0f, 0.0f);
		m3->specular = fw::Vector3(1.0f, 1.0f, 1.0f);
		mtrl_list.push_back(m3);
		pm::Material *m4 = new pm::Lambert;
		m4->diffuse = fw::Vector3(1.0f, 1.0f, 1.0f);
		m4->specular = fw::Vector3();
		mtrl_list.push_back(m4);
		pm::Material *m5 = new pm::Lambert;
		m5->diffuse = fw::Vector3(0.7f, 0.7f, 0.3f);
		m5->specular = fw::Vector3();
		mtrl_list.push_back(m5);

		pm::Material *m6 = new pm::Phong;
		m6->diffuse = fw::Vector3(0.2f, 0.3f, 0.2f);
		m6->specular = fw::Vector3(0.3f, 0.7f, 0.1f);
		mtrl_list.push_back(m6);

		pm::Material *m7 = new pm::Lambert;
		m7->diffuse = fw::Vector3(0.0f, 0.0f, 0.0f);
		m7->specular = fw::Vector3(0.0f, 0.0f, 0.0f);
		m7->refraction = fw::Vector3(0.9f, 0.8f, 0.9f);
		mtrl_list.push_back(m7);

		// オブジェクト生成
		objects.push_back(pm::CreateSphere(fw::Vector3(-1.5f, 1.0f, 1.5f), 0.8f, m3));
		objects.push_back(pm::CreateSphere(fw::Vector3(-1.0f, 3.6f, -2.0f), 0.3f, m2));
		objects.push_back(pm::CreateSphere(fw::Vector3(2.0f, 2.3f, 0.0f), 0.3f, m4));
		objects.push_back(pm::CreateSphere(fw::Vector3(0.0f, 0.7f, 1.1f), 0.5f, m5));
		objects.push_back(pm::CreateSphere(fw::Vector3(1.5f, 0.8f, -1.0f), 0.8f, m7));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 0.0f, 3.0f), fw::Vector3(3.0f, 0.0f, -6.0f), fw::Vector3(3.0f, 0.0f, 3.0f), m1));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 0.0f, 3.0f), fw::Vector3(-3.0f, 0.0f, -6.0f), fw::Vector3(3.0f, 0.0f, -6.0f), m1));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, 3.0f), fw::Vector3(-3.0f, 0.0f, 3.0f), fw::Vector3(3.0f, 0.0f, 3.0f), m2));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, 3.0f), fw::Vector3(3.0f, 0.0f, 3.0f), fw::Vector3(3.0f, 6.0f, 3.0f), m2));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(-3.0f, 0.0f, -6.0f), fw::Vector3(-3.0f, 0.0f, 3.0f), m3));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(-3.0f, 0.0f, 3.0f), fw::Vector3(-3.0f, 6.0f, 3.0f), m3));
		objects.push_back(pm::CreateTriangle(fw::Vector3(3.0f, 6.0f, 3.0f), fw::Vector3(3.0f, 0.0f, 3.0f), fw::Vector3(3.0f, 0.0f, -6.0f), m2));
		objects.push_back(pm::CreateTriangle(fw::Vector3(3.0f, 6.0f, 3.0f), fw::Vector3(3.0f, 0.0f, -6.0f), fw::Vector3(3.0f, 6.0f, -6.0f), m2));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(-3.0f, 6.0f, 3.0f), fw::Vector3(3.0f, 6.0f, 3.0f), m1));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(3.0f, 6.0f, 3.0f), fw::Vector3(3.0f, 6.0f, -6.0f), m1));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(3.0f, 0.0f, -6.0f), fw::Vector3(-3.0f, 0.0f, -6.0f), m2));
		objects.push_back(pm::CreateTriangle(fw::Vector3(-3.0f, 6.0f, -6.0f), fw::Vector3(3.0f, 6.0f, -6.0f), fw::Vector3(3.0f, 0.0f, -6.0f), m2));

		tree = pm::CreateKDTree();
		pm::Photon photon;

		float dist = -1.0f;
		float res = 0.0f;
		size_t n = 0;
		bool absorbe = false;
		for(int j = 0; j < 100000; j++)
		{
			light->emitte(photon);
			absorbe = false;

			for(int r = 0; r < 7; r++)
			{
				dist = -1.0f;
				for(size_t i = 0; i < objects.size(); i++)
				{ 
					res = objects[i]->distance(photon.position, photon.direction); // 距離を測って、最短なら更新
					if(res > 0.0f)
					{
						if(dist > 0.0f)
						{
							if(res < dist)
							{
								dist = res;
								n = i;
							}
						}
						else
						{
							dist = res;
							n = i;
						}
					}
				}

				if(dist > 0)
				{
					byte res = objects[n]->photon(photon);
					if(res & pm::Absorption) // 吸収
					{
						tree->add(photon);
					}
					else
					{
						if(r == 6) tree->add(photon);
					}

					if(!(res & (pm::Reflection | pm::Refraction))) // 反射、屈折しない
					{
						break;
					}

//					if(res & pm::Refraction)
//					{
//						tree->add(photon);
//					}
//					else break;
				}
				else
				{
					break;
				}
			}
		}

		tree->build();

		tracer = new pm::Tracer(objects, *tree);
		
		char buf[256];
		sprintf(buf, "フォトン数 : %d", tree->size());
		MessageBox(NULL, buf, "PhotonMapping", MB_OK);

		_inst = this;
	}
	virtual ~MainWindow()
	{
		std::for_each(mtrl_list.begin(), mtrl_list.end(), PointerDeleter<pm::Material>());
		std::for_each(objects.begin(), objects.end(), PointerDeleter<pm::Object>());

		delete tracer;
		if(ren) delete ren;
		delete tree;
		delete light2;
		delete light;
	}

	virtual int init()
	{
		assert(!AppWindow::init());

		fw::g3d::CreateRenderer(&ren, getHWND());

		{
			HDC hdc = GetDC(getHWND());

			BITMAPINFO bitmap_info;
			ZeroMemory(&bitmap_info, sizeof(BITMAPINFO));

			bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmap_info.bmiHeader.biBitCount = 32;
			bitmap_info.bmiHeader.biPlanes = 1;
			bitmap_info.bmiHeader.biWidth = 640;
			bitmap_info.bmiHeader.biHeight = -480;

			back_buffer1 = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, (void**)&pixel1, NULL, 0);
			back_buffer2 = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, (void**)&pixel2, NULL, 0);
			buffer_dc = CreateCompatibleDC(hdc);
			SelectObject(buffer_dc, back_buffer1);

			ReleaseDC(getHWND(), hdc);
		}

		return 0;
	}

	virtual int loop()
	{
		return 0;
	}

	void redraw()
	{
		static float angle = 0.0f;
		angle += 3.1415f / 180.0f;
		if(angle >= 2.0f * 3.1415f) angle = 0.0f;

		fw::Matrix4 world, view, proj;
		fw::Vector3 eye(0.0f, 1.0f, -5.0f);
		fw::Vector3 look(0.0f, 1.0f, 0.0f);
		fw::Vector3 up(0.0f, 1.0f, 0.0f);

		world.identity();
//		fw::Vector3 axis(1, 2, 3);
//		fw::Vector3Normalize(&axis);
//		fw::Matrix4RotationAxis(&world, &axis, angle);

		fw::Matrix4ViewL(&view, &eye, &look, &up);
		fw::Matrix4PerspectiveL(&proj, 60.0f * 3.1415f / 180.0f, 640.0f / 480.0f, 1.0f, 100.0f);

#if defined(POINT_RENDER)
		std::vector<fw::Vector3> vl;
		uint size = tree->size();
		for(uint i = 0; i < size; i++)
		{
			vl.push_back(tree->getPosition(i));
		}
		ren->setTransform(&world, fw::g3d::TT_World);
		ren->setTransform(&view, fw::g3d::TT_View);
#else
		tracer->transform(world, proj);
		tracer->camera(eye, look, up);
#endif

#if !defined(POINT_RENDER)
		{
			unsigned long *p = (!attach_buffer) ? pixel1 : pixel2;
			tracer->render(p, 640, 480);
		}

		{
			PAINTSTRUCT ps;
			InvalidateRect(getHWND(), NULL, FALSE);
			HDC hdc = BeginPaint(getHWND(), &ps);
			
			BitBlt(hdc, 0, 0, 640, 480, buffer_dc, 0, 0, SRCCOPY);

			EndPaint(getHWND(), &ps);
		}

		if(!attach_buffer)
		{
			SelectObject(buffer_dc, back_buffer2);
			attach_buffer = 1;
		}
		else
		{
			SelectObject(buffer_dc, back_buffer1);
			attach_buffer = 0;
		}
#else		
//		ren->setTransform(&world, fw::g3d::TT_World);
//		ren->setTransform(&view,  fw::g3d::TT_View);


		ren->clear(0x00000000);

		if(vl.size() != 0) ren->draw((float*)&vl.front().x, size, fw::g3d::PT_PointList);

		ren->present();
#endif	
	}

	void paint()
	{
		if(!attach_buffer)
		{
			SelectObject(buffer_dc, back_buffer2);
			attach_buffer = 1;
		}
		else
		{
			SelectObject(buffer_dc, back_buffer1);
			attach_buffer = 0;
		}

		{
			PAINTSTRUCT ps;
			InvalidateRect(getHWND(), NULL, FALSE);
			HDC hdc = BeginPaint(getHWND(), &ps);
			
			BitBlt(hdc, 0, 0, 640, 480, buffer_dc, 0, 0, SRCCOPY);

			EndPaint(getHWND(), &ps);
		}

		if(!attach_buffer)
		{
			SelectObject(buffer_dc, back_buffer2);
			attach_buffer = 1;
		}
		else
		{
			SelectObject(buffer_dc, back_buffer1);
			attach_buffer = 0;
		}
	}
	MainWindow* instance() { return _inst; }


	static LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch(Msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE) PostQuitMessage(0);
			_inst->redraw();
			break;
		case WM_PAINT:
			_inst->paint();
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
		
		return 0L;
	}
};

MainWindow* MainWindow::_inst = 0;
#endif


#if 0

class Canvas
{
private:
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	int width;
	int height;
public:
	Canvas(SDL_Renderer *renderer, int width, int height);
	~Canvas();

	void SetPixelColor(int x, int y);
};

Canvas::Canvas(SDL_Renderer *_renderer, int _width, int _height) :
	renderer(_renderer), texture(NULL), width(_width), height(_height)
{
	this->texture = SDL_CreateTexture(renderer,
									  SDL_PIXELFORMAT_RGB24,
									  SDL_TEXTUREACCESS_STREAMING,
									  this->width, this->height);
}

Canvas::~Canvas()
{
	if (this->texture)
		SDL_DestroyTexture(this->texture);
}

void Canvas::SetPixelColor(int x, int y)
{
	uint8_t *pixels = NULL;
	int pitch = 0;
	SDL_LockTexture(this->texture, NULL, (void **)&pixels, &pitch);	

	uint8_t *p = pixels + pitch * y + x;
	p[0] = 0;
	p[1] = 255;
	p[2] = 0;

	SDL_UnlockTexture(this->texture);
}

#endif

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window;

	window = SDL_CreateWindow("SDL Application",
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  SCREEN_WIDTH, SCREEN_HEIGHT,
							  SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_RGBA8888,
											 SDL_TEXTUREACCESS_STREAMING,
											 SCREEN_WIDTH,
											 SCREEN_HEIGHT);
	uint8_t *pixels = NULL;
	int pitch = 0;


	auto canvas = CreateSDLCanvas(texture);

	// SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
	
	printf("locking succeeded. pitch = %d\n", pitch);
	
	Vector3 white(0, 0, 0);
	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		// uint8_t *p = pixels + pitch * i;
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			// p[j*4+0] = 255;
			// p[j*4+1] = 255;
			// p[j*4+2] = i % 255;
			// p[j*4+3] = j % 255;
			canvas->SetColor(j, i, white);
		}
	}

	// SDL_UnlockTexture(texture);

	Scene scene;

	Vector3 c;
	c << -1.5f, 1.0f, 1.5f;
	auto sphere = CreateSphere(c, 2.0f);

	DimensionalTree tree;
	Vector3 pl;
	// pl << 2.0f, 1.0f, 0.0f;
	// auto light = CreatePointLight(pl);

	pl << 0.0f, 6.0f, 1.0f;

	scene.lights.push_back(CreateSquareLight(pl, 2.0f, 2.0f));
	scene.objects.push_back(CreateSphere(Vector3(-1.5f, 1.0f, 1.5f), 0.8f));
	scene.objects.push_back(CreateSphere(Vector3(-1.0f, 0.6f, -2.0f), 0.3f));
	// scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 0.0f,  3.0f),
	// 									   Vector3(-3.0f, 0.0f, -6.5f),
	// 									   Vector3( 3.0f, 0.0f,  3.0f)));
	// scene.objects.push_back(CreateTriangle(Vector3(-3.0f, 6.0f, -6.0f),
	// 									   Vector3( 3.0f, 6.0f, -6.0f),
	// 									   Vector3( 3.0f, 0.0f, -6.0f)));
	
	scene.Emitte(100000, 3);

	std::cout << "emitting photon was finished" << std::endl;

	Vector3 eye;
	Vector3 look;
	Vector3 up;
	Matrix4 view;
	Matrix4 projection;
	Matrix4 screen;

	// カメラ座標系
	eye << 0.0f, 0.0f, -5.0f;
	look << 0.0f, 0.0f, 0.0f;
	up << 0.0f, 1.0f, 0.0f;
	Matrix4ViewL(view, eye, look, up); // ok

	Matrix4PerspectiveL(projection, 60.0f * 3.1415f / 180.0f,
						(float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT),
						1.0f, 100.0f);

	Matrix4Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	Matrix4 inverse_view;
	Matrix4 inverse_projection;
	Matrix4 inverse_screen;
	inverse_view = view.inverse();
	inverse_projection = projection.inverse();
	inverse_screen = screen.inverse();

	Matrix4 inverse_affine = inverse_screen * inverse_projection * inverse_view;
	Matrix4 transpose_affine = inverse_affine.transpose();
	// std::cout << inverse_affine << std::endl;

	// レイ発射点, 同次座標系をあれするのでwをつかう
	Vector4 source;
	source << 0.0f, 0.0f, 0.0f, 1.0f;

	Vector4 s2;
	Matrix4 transpose_view;
	transpose_view = inverse_view.transpose();
	s2 = transpose_view * source;
	s2 = s2 / s2(3);
	// std::cout << s2 << std::endl;
	Vector3 origin;
	origin << s2(0), s2(1), s2(2);
	// std::cout << origin << std::endl;

	Intersection is;

#if 0
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		uint8_t *p = pixels + pitch * y;

		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			Vector4 destination;
			destination << float(x), float(y), 0.0f, 1.0f;
			// destination = inverse_affine * destination;
			// Transformをの演算(旧matrix.hのMatrix4Transformに相当)す
			// る処理を行なうには転地してから、乗算を行なえばよい
			destination = transpose_affine * destination;
			destination = destination / destination(3);
			Vector3 d2;
			d2 << destination(0), destination(1), destination(2);
			Vector3 direction;
			direction = d2 - origin;
			direction.normalize();

			Vector3 color(0, 0, 0);
			scene.Trace(origin, direction, color, 0);

			canvas->SetColor(x, y, color);
		}
	}
#else
	Visualizer visualizer;

	visualizer.world = Matrix4::Identity();
	visualizer.view = view;
	visualizer.projection = projection;

	visualizer.RenderPoints(scene.tree.points, *canvas);
#endif

	canvas = nullptr;

	SDL_Event event;
	bool loop = true;

	while (loop)
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				loop = false;
				break;
			}
		}	

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		// SDL_RenderDrawPoint(renderer, 600, 400);
		// SDL_RenderDrawLine(renderer, 0, 0, 400, 300);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
    
    return 0;
}
