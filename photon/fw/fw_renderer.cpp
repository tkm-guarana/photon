
#include "fw_renderer.h"
#include <algorithm>
#undef max
#undef min

namespace fw
{

namespace g3d
{



class SoftwareRenderer : public IRenderer
{
private:
	HBITMAP	back_buffer1;
	HBITMAP	back_buffer2;
	HBITMAP z_buffer;
	BOOL	attach_buffer;
	HDC		buffer_dc;
	HDC		z_buffer_dc;
	HWND	window_handle;
	unsigned long *pixel1;
	unsigned long *pixel2;
	unsigned long *z_pixel;

	fw::Vector3 light;

	// Affine Transform
	fw::Matrix4 world;
	fw::Matrix4 view;
	fw::Matrix4 proj;
	fw::Matrix4 screen;
	fw::Matrix4 affine;

	// stream
	fw::Vector3 *coordinate;
	fw::Vector3 *normal;
	fw::Vector3 *texture_uv;

	unsigned short index_buffer;
public:
	SoftwareRenderer(HWND hWnd);
	virtual ~SoftwareRenderer();
	virtual void clear(dword color);
	virtual void setTransform(const fw::Matrix4 *matrix, TransformType type);
	virtual void setStreamSource(const void *stream, StreamType type);
	virtual void draw(float v[], unsigned long count, int type);
	virtual void drawIndexed(float v[], unsigned short i[], unsigned long count, int type);
	virtual void present();

	void drawTriangleList(fw::Vector3 vertices[], unsigned short indices[]);
	void drawPointList(fw::Vector3 vertices[], dword num);

	DWORD shadeLambert(fw::Vector3 coord, fw::Vector3 normal);
};


SoftwareRenderer::SoftwareRenderer(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);

//	back_buffer1 = CreateCompatibleBitmap(hdc, 640, 480);
//	back_buffer2 = CreateCompatibleBitmap(hdc, 640, 480);

	BITMAPINFO bitmap_info;
	ZeroMemory(&bitmap_info, sizeof(BITMAPINFO));

	bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info.bmiHeader.biBitCount = 32;
	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biWidth = 640;
	bitmap_info.bmiHeader.biHeight = -480;

	back_buffer1 = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, (void**)&pixel1, NULL, 0);
	back_buffer2 = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, (void**)&pixel2, NULL, 0);
	z_buffer = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, (void**)&z_pixel, NULL, 0);
	buffer_dc = CreateCompatibleDC(hdc);
	z_buffer_dc = CreateCompatibleDC(hdc);
	SelectObject(buffer_dc, back_buffer1);
	SelectObject(z_buffer_dc, z_buffer);

	ReleaseDC(hWnd, hdc);

	window_handle = hWnd;
	attach_buffer = 0;

	fw::Matrix4PerspectiveL(&proj, 60.0f * 3.1415f / 180.0f, 640.0f / 480.0f, 1.0f, 100.0f);
	fw::Matrix4Screen(&screen, 640.0f, 480.0f);
}

SoftwareRenderer::~SoftwareRenderer()
{
	DeleteObject(z_buffer);
	DeleteObject(back_buffer2);
	DeleteObject(back_buffer1);
	ReleaseDC(window_handle, z_buffer_dc);
	ReleaseDC(window_handle, buffer_dc);
}

void SoftwareRenderer::clear(dword color)
{
	HBRUSH brush = CreateSolidBrush(color);
	HGDIOBJ old = SelectObject(buffer_dc, brush);
	PatBlt(buffer_dc, 0, 0, 640, 480, PATCOPY);
	SelectObject(buffer_dc, old);
	DeleteObject(brush);

	PatBlt(z_buffer_dc, 0, 0, 640, 480, BLACKNESS);
}

void SoftwareRenderer::setTransform(const fw::Matrix4 *matrix, TransformType type)
{
	switch(type)
	{
	case TT_World:
		world = *matrix;
		break;
	case TT_View:
		view = *matrix;
		break;
	case TT_Projection:
		proj = *matrix;
		break;
	case TT_Screen:
		screen = *matrix;
		break;
	};

	affine = world * view * proj;
}

void SoftwareRenderer::setStreamSource(const void *stream, StreamType type)
{
	switch(type)
	{
	case ST_Coord:
		coordinate = (fw::Vector3*)stream;
	case ST_Normal:
		normal = (fw::Vector3*)stream;
	case ST_TextureUV:
		texture_uv = (fw::Vector3*)stream;
	}
}

void SoftwareRenderer::draw(float v[], unsigned long count, int type)
{
	if(type == PT_PointList)
	{
		drawPointList((fw::Vector3*)v, count);
	}
}

void SoftwareRenderer::drawPointList(fw::Vector3 vertices[], dword num)
{
	unsigned long *buf = NULL;
	if(!attach_buffer)
	{
		buf = pixel1;
	}
	else
	{
		buf = pixel2;
	}

	for(dword i = 0; i < num; i++)
	{
		fw::Vector3 v = vertices[i];
		fw::Vector3TransformCoordinate(&v, &world);
		fw::Vector3TransformCoordinate(&v, &view);
		fw::Vector3TransformCoordinate(&v, &proj);
		fw::Vector3TransformCoordinate(&v, &screen);

		int xx = int(v.x);
		int yy = int(v.y);
		if(0 < xx && xx < 640 && 0 < yy && yy < 480)
		{
			int t = yy * 640 + xx;
		//	if(0 < t && t < 640*480)
		//	{
			buf[t] = (0xFF << 16);
		//	}
		}
	}
}

// 三角形描画
void SoftwareRenderer::drawTriangleList(fw::Vector3 vertices[], unsigned short indices[])
{
	unsigned long *buf = NULL;
	if(!attach_buffer)
	{
		buf = pixel1;
	}
	else
	{
		buf = pixel2;
	}

	int p[3];
	// y位置により、ソートする。三つのみなのでifだけで
	p[0] = indices[0]; // [0|x|x]
	if(vertices[p[0]].y > vertices[indices[1]].y)
	{
		p[1] = p[0];
		p[0] = indices[1]; // [1|0|x]
	}
	else
	{
		p[1] = indices[1]; // [0|1|x]
	}

	if(vertices[p[0]].y > vertices[indices[2]].y)
	{
		p[2] = p[1];
		p[1] = p[0];
		p[0] = indices[2]; // [2|x|x]
	}
	else
	{
		if(vertices[p[1]].y > vertices[indices[2]].y)
		{
			p[2] = p[1];
			p[1] = indices[2]; // [x|2|x]
		}
		else
		{
			p[2] = indices[2]; // [x|x|2]
		}
	}
	// スクリーン座標に変換
	fw::Vector3 p0(vertices[p[0]]);
	fw::Vector3 p1(vertices[p[1]]);
	fw::Vector3 p2(vertices[p[2]]);
/*
	fw::Vector3TransformCoordinate(&p0, &affine);
	fw::Vector3TransformCoordinate(&p1, &affine);
	fw::Vector3TransformCoordinate(&p2, &affine);
	fw::Vector3TransformCoordinate(&p0, &screen);
	fw::Vector3TransformCoordinate(&p1, &screen);
	fw::Vector3TransformCoordinate(&p2, &screen);	
*/

	// ラスタライズ
	if(abs(p1.y - p0.y) > 1.0f)
	{
		int y_begin = int(p0.y);
		int y_end = int(p1.y);
//		float z_begin = p0.z;
//		float z_end = p1.z;
		float dx0 = (p1.x - p0.x) / (p1.y - p0.y); // p0 to p1
		float dx1 = (p2.x - p0.x) / (p2.y - p0.y); // p0 to p2
		float dz0 = (p1.z - p0.z) / (p1.y - p0.y); // p0 to p1
		float dz1 = (p2.z - p0.z) / (p2.y - p0.y); // p0 to p2

		float x_border = dx1 * (p1.y - p0.y) + p0.x;
		if(p1.x > x_border)
		{
			std::swap(dx0, dx1); // exchange dx0 and dx1, dx0 = p0 to p2, dx1 = p0 to p1
			std::swap(dz0, dz1);
		}

		float x_begin = p0.x;
		float x_end = p0.x;
		float z_begin = p0.z;
		float z_end = p0.z;

//		float inc0 = p1.y - p0.y; // increase value
//		float inc1 = p2.y - p0.y;

//		fw::Vector3 lerp0, lerp1;

		for(int y = y_begin; y < y_end; y++)
		{
//			fw::Vector3Lerp(&lerp0, &p0, &p1, inc0);
//			fw::Vector3Lerp(&lerp1, &p0, &p2, inc1);
//			float inc = lerp1.x - lerp0.x;
			float dz = (z_end - z_begin) / (x_end - x_begin);
			float z = z_begin;
			for(int x = int(x_begin); x < int(x_end); x++)
			{
//				fw::Vector3 lerp_coord;
//				fw::Vector3Lerp(&lerp_coord, &lerp0, &lerp1, inc);
				unsigned char z_int = 255 * (1.0f - z);
				if(z_int > (z_pixel[y*640+x] & 0xFF))
				{
					buf[y*640+x] = (0xFF << 16);
					z_pixel[y*640+x] = (0xFF << 24) | (z_int << 16) | (z_int << 8) | (z_int << 0); // z buffer
				}
				z += dz;
			}
			x_begin += dx0;
			x_end += dx1;
			z_begin += dz0;
			z_end += dz1;
		}
	}

	if(abs(p2.y - p1.y) > 1.0f)
	{
		int y_begin = int(p1.y);
		int y_end = int(p2.y);
		float dx0 = (p2.x - p1.x) / (p2.y - p1.y); // p1 to p2, begin increase
		float dx1 = (p2.x - p0.x) / (p2.y - p0.y); // p0 to p2, end increase
		float dz0 = (p2.z - p1.z) / (p2.y - p1.y); // p1 to p2, begin increase
		float dz1 = (p2.z - p0.z) / (p2.y - p0.y); // p0 to p2, end increase

		float x_border = dx1 * (p1.y - p0.y) + p0.x;
		float x_begin = p1.x;
		float x_end = x_border;

		float z_border = dz1 * (p1.y - p0.y) + p0.z;
		float z_begin = p1.z;
		float z_end = z_border;

		if(p1.x > x_border)
		{
			std::swap(dx0, dx1); // exchange dx0 and dx1, dx0 = p0 to p2, dx1 = p1 to p2 
			std::swap(x_begin, x_end);
//		}
//		if(p1.z > z_border)
//		{
			std::swap(dz0, dz1);
			std::swap(z_begin, z_end);
		}

		for(int y = y_begin; y < y_end; y++)
		{
			float dz = (z_end - z_begin) / (x_end - x_begin);
			float z = z_begin;
			for(int x = int(x_begin); x < int(x_end); x++)
			{
				unsigned char z_int = 255 * (1.0f - z);
				if(z_int > (z_pixel[y*640+x] & 0xFF))
				{
					buf[y*640+x] = 0x00000000; //(0xFF << 16) | (y << 8) | (x << 0);
					z_pixel[y*640+x] = (0xFF << 24) | (z_int << 16) | (z_int << 8) | (z_int << 0); // z buffer
				}
				z += dz;
			}
			x_begin += dx0;
			x_end += dx1;
			z_begin += dz0;
			z_end += dz1;
		}
	}
}

DWORD SoftwareRenderer::shadeLambert(fw::Vector3 coord, fw::Vector3 normal)
{
	return 0x00000000;
}

void SoftwareRenderer::drawIndexed(float v[], unsigned short i[], unsigned long count, int type)
{
	static float angle = 0.0f;
	angle += 3.1415f / 180.0f / 4.0f;
	if(angle >= 2.0f * 3.1415f) angle = 0.0f;

	light = fw::Vector3(1.0f, 1.0f, 1.0f);
	fw::Vector3Normalize(&light);

//	fw::Vector3 eye(5.0f * cos(angle), 0.0f, 5.0f * sin(angle));
	fw::Vector3 eye(0.0f, 0.0f, 5.0f);
	fw::Vector3 look(0.0f, 0.0f, 0.0f);
	fw::Vector3 up(0.0f, 1.0f, 0.0f);

	fw::Matrix4ViewL(&view, &eye, &look, &up);

	fw::Matrix4 world;
	fw::Vector3 axis(1, 2, 3);
	fw::Vector3Normalize(&axis);
	fw::Matrix4RotationAxis(&world, &axis, angle);
	fw::Matrix4 vp = world * view * proj;

	setTransform(&world, TT_World);
	setTransform(&view, TT_View);
	setTransform(&proj, TT_Projection);
	setTransform(&screen, TT_Screen);
	for(unsigned long j = 0; j < count; j++)
	{
		if(type == fw::g3d::PT_TriangleList)
		{
			unsigned short n0 = i[j*3+0];// - 1;
			unsigned short n1 = i[j*3+1];// - 1;
			unsigned short n2 = i[j*3+2];// - 1;
			fw::Vector3 v1(v[n0*3+0], v[n0*3+1], v[n0*3+2]);
			fw::Vector3 v2(v[n1*3+0], v[n1*3+1], v[n1*3+2]);
			fw::Vector3 v3(v[n2*3+0], v[n2*3+1], v[n2*3+2]);


			fw::Vector3TransformCoordinate(&v1, &vp);
			fw::Vector3TransformCoordinate(&v2, &vp);
			fw::Vector3TransformCoordinate(&v3, &vp);
			fw::Vector3TransformCoordinate(&v1, &screen);
			fw::Vector3TransformCoordinate(&v2, &screen);
			fw::Vector3TransformCoordinate(&v3, &screen);

			float v[] = {v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z};
			unsigned short idx[] = {0, 1, 2};
			drawTriangleList((fw::Vector3*)v, idx);
		}
		else if(type == PT_PointList)
		{
			drawPointList((fw::Vector3*)v, count);
		}
//		MoveToEx(buffer_dc, v1.x, v1.y, NULL);
//		LineTo(buffer_dc, v2.x, v2.y);
//		LineTo(buffer_dc, v3.x, v3.y);
//		LineTo(buffer_dc, v1.x, v1.y);

	}
}

void SoftwareRenderer::present()
{
	PAINTSTRUCT ps;
	InvalidateRect(window_handle, NULL, FALSE);
	HDC hdc = BeginPaint(window_handle, &ps);
	
	BitBlt(hdc, 0, 0, 640, 480, buffer_dc, 0, 0, SRCCOPY);
//	BitBlt(hdc, 0, 0, 640, 480, z_buffer_dc, 0, 0, SRCCOPY);

	EndPaint(window_handle, &ps);
	
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

void CreateRenderer(IRenderer** renderer, HWND handle)
{
	*renderer = new SoftwareRenderer(handle);
}


}

}