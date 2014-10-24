
#ifndef _fw_renderer_h_
#define _fw_renderer_h_

#include <windows.h>
#include "fw_matrix.h"
#include "fw_type.h"

namespace fw // FrameWork
{
namespace g3d // Graphics 3D
{

enum TransformType
{
	TT_World = 1,
	TT_View,
	TT_Projection,
	TT_Screen,
	TT_Unkwon = 0,
};

enum StreamType
{
	ST_Coord = 1,
	ST_Normal,
	ST_TextureUV,
	ST_Color,
};

enum PrimitiveType
{
	PT_TriangleList = 0,
	PT_LineList,
	PT_PointList
};

class IRenderer
{
public:
	virtual void clear(dword color) = 0;
	virtual void setTransform(const fw::Matrix4 *matrix, TransformType type) = 0;
	virtual void setStreamSource(const void *stream, StreamType type) = 0;
	virtual void draw(float v[], unsigned long count, int type) = 0;
	virtual void drawIndexed(float v[], unsigned short i[], unsigned long count, int type) = 0;
	virtual void present() = 0;
};

void CreateRenderer(IRenderer** renderer, HWND handle);

}

}

#endif