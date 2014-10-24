
#ifndef _photon_photon_h_
#define _photon_photon_h_

#include "matrix.h"
/* #include "type.h" */

namespace pm
{

	struct Photon
	{
		Vector3	position;	// 位置
		Vector3 direction;	// 方向、常に正規化されているものとする
		Vector3 spectrum;	// スペクトル成分(色)
		Vector3 incidence;	// 入射角
	};


}

#endif
