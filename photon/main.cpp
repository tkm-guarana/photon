
#if 0
#include <crtdbg.h>
#include <windows.h>
#include <assert.h>
#endif

#include <vector>
#include <algorithm>
#include <cstdint>

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


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window;

	window = SDL_CreateWindow("SDL Application",
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  800, 600,
							  SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_RGBA8888,
											 SDL_TEXTUREACCESS_STREAMING,
											 800,
											 600);

	uint8_t *pixels = NULL;
	int pitch = 0;

	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
	
	printf("locking succeeded. pitch = %d\n", pitch);

	
	for (int i = 0; i < 600; i++)
	{
		uint8_t *p = pixels + pitch * i;
		for (int j = 0; j < 800; j++)
		{
			p[j*4+0] = 255;
			p[j*4+1] = 255;
			p[j*4+2] = i % 255;
			p[j*4+3] = j % 255;
		}
	}

	SDL_UnlockTexture(texture);

	std::unique_ptr<DimensionalTree> tree(new DimensionalTree());
	auto root = tree->Build();


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
