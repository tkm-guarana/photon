
#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_


struct Intersection
{
	float distance;
};

class SceneObject
{
private:

public:
	SceneObject() = default;
	virtual ~SceneObject() = default;

	virtual bool Collide(const Vector3 &origin, const Vector3 &direction, Intersection &is) = 0;
	virtual void Behave(Photon &photon) = 0;
	virtual Vector3 GetNormalVector(const Vector3 &point) = 0;

	Material material;
};

extern std::unique_ptr<SceneObject> CreateSphere(const Vector3 &center, float radious);
extern std::unique_ptr<SceneObject> CreateTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2);


#endif /* _SCENEOBJECT_H_ */
