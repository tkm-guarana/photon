


#include <vector>
#include <set>
#include <memory>
#include <iostream>

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


class SDLTextureCanvas : public Canvas
{
private:
	SDL_Texture *texture;
	uint8_t *pixels;
	int pitch;

	virtual void SetColorRec(int x, int y, Vector3 color);

public:
	SDLTextureCanvas(SDL_Texture *_texture);
	virtual ~SDLTextureCanvas();
};

SDLTextureCanvas::SDLTextureCanvas(SDL_Texture *_texture) :
	texture(_texture), pixels(NULL), pitch(0)
{
	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
}

SDLTextureCanvas::~SDLTextureCanvas()
{
	SDL_UnlockTexture(texture);
}


void SDLTextureCanvas::SetColorRec(int x, int y, Vector3 color)
{
	uint8_t *p = pixels + pitch * y;
	p[x*4+0] = 255; // a
	p[x*4+1] = uint8_t(std::min(color(2), 1.0f) * 255); // r
	p[x*4+2] = uint8_t(std::min(color(1), 1.0f) * 255); // g
	p[x*4+3] = uint8_t(std::min(color(0), 1.0f) * 255); // b
}

std::unique_ptr<Canvas> CreateSDLCanvas(SDL_Texture *tex)
{
	return std::unique_ptr<Canvas>(new SDLTextureCanvas(tex));
}
