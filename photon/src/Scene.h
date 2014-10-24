
#ifndef _SCENE_H_
#define _SCENE_H_

class Scene
{
private:
	
public:
	Scene() = default;
	~Scene() = default;

	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<SceneObject>> objects;
	DimensionalTree tree;

	void Emitte(int quantity, int reflection_limit);
	bool Trace(Vector3 &base, Vector3 &direction, Vector3 &rgb, uint32_t depth);
};

#endif /* _SCENE_H_ */
