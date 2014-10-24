
#ifndef _LIGHT_H_
#define _LIGHT_H_


class Light
{
private:

public:
	Light() = default;
	virtual ~Light() = default;

	virtual void Emitte(Photon &photon) = 0;
};

extern std::unique_ptr<Light> CreatePointLight(const Vector3 &position);
extern std::unique_ptr<Light> CreateSquareLight(const Vector3 &position, float width, float height);

#endif /* _LIGHT_H_ */
