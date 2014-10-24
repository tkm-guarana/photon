
#ifndef _photon_kd_tree_
#define _photon_kd_tree_

#include "matrix.h"
#include "photon.h"
#include "material.h"

typedef unsigned char	byte;
typedef unsigned short	word;
typedef unsigned long	dword;
typedef unsigned int	uint;

namespace pm
{


	class KDTree
	{
	public:
		// dimension axis
		enum axis
		{
			ax = 0,
			ay,
			az
		};
		enum
		{
			endn = 0xFFFFFFFF // end node
		};
		class PhotonContainer;
	public:
		KDTree() {}
		virtual ~KDTree() {}

		virtual void add(const Photon &photon) = 0;
		virtual void build() = 0;
		virtual void sort(axis using_axis) = 0;
		virtual uint find_range(fw::Vector3 coord, float distance, uint heap_max) = 0;
		virtual uint size() = 0;
		virtual Photon* begin() = 0;
		virtual Photon* end() = 0;
		virtual fw::Vector3 radiance(const fw::Vector3 &coord, float distance, uint max_heap, const fw::Vector3 &normal, Material *material) = 0;

		// 試験用
		virtual fw::Vector3 getPosition(uint num) = 0;
	};

// 最近傍フォトンを格納するコンテナ
	class KDTree::PhotonContainer
	{
	public:
		PhotonContainer() {}
		virtual ~PhotonContainer() {}

		virtual uint size() = 0;
		virtual const Photon& operator[](uint n) = 0;
	};

	KDTree* CreateKDTree();


}



#endif
