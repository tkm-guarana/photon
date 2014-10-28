

#ifndef _canvas_h_
#define _canvas_h_


class Canvas
{
private:
	virtual void SetColorRec(int x, int y, Vector3 color) = 0;
	
public:
	Canvas() = default;
	virtual ~Canvas() = default;

	void Clear() {  }
	void SetColor(int x, int y, Vector3 color)
	{
		SetColorRec(x, y, color);
	}

	virtual Matrix4 GetScreenMatrix() = 0;
	virtual Size GetScreenSize() = 0;
};


std::unique_ptr<Canvas> CreateSDLCanvas(SDL_Texture *tex);


#endif /* _canvas_h_ */
