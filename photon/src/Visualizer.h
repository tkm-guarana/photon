

#ifndef _visualizer_h_
#define _visualizer_h_


class Visualizer
{
public:

	Canvas &canvas;
	Scene &scene;

	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;

	Visualizer(Canvas &_canvas, Scene &_scene);
	~Visualizer() = default;

	void Render();
	void RenderPoints();
	void RenderLine(const Vector3 &begin, const Vector3 &end, const Vector3 &color);

	struct ThreadParam
	{
		int x;
		int y;
		Vector3 origin;
		Matrix4 affine;
	};

	void RenderWorker(ThreadParam &param);
};


#endif /* _visualizer_h_ */
