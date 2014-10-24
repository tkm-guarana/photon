
#ifndef _photon_light_h_
#define _photon_light_h_

#include "photon.h"

namespace pm
{

// インターフェイス
class Light
{
private:
public:
	Light() {}
	virtual ~Light() {}

	virtual void emitte(Photon &photon) = 0;
};

Light* CreatePointLight(fw::Vector3 &position);
Light* CreateSquareLight(fw::Vector3 &position, float width, float height);




}

#endif