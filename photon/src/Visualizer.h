

#ifndef _visualizer_h_
#define _visualizer_h_


class Visualizer
{
public:
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;

	Visualizer() = default;
	~Visualizer() = default;

	void Render(Canvas &canvas, Scene &scene);
	void RenderPoints(std::vector<Photon> photons, Canvas &canvas);
};


#endif /* _visualizer_h_ */
