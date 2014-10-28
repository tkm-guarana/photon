
#include <vector>
#include <set>
#include <memory>
#include <iostream>
#include <mutex>

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

/**
 * フォトンを放出する
 */
void Scene::Emitte(int quantity,			// フォトンの量
				   int reflection_limit)	// 最大反射回数
{
	Photon p;
	// 吸収フラグ
	bool absorption = false;
	Intersection is;

	// 暫定、本来は放出フォトン数を光源の数で割り、均等に配置する
	auto &light = this->lights[0];

	std::cout << "object size: " << objects.size() << std::endl;

	for (int j = 0; j < quantity; j++)
	{
		light->Emitte(p);
		absorption = false;

		for (int r = 0; r < reflection_limit; r++)
		{
			float distance = 100000000;
			int index = -1;
			for (size_t i = 0; i < objects.size(); i++)
			{
				if (objects[i]->Collide(p.position, p.direction, is))
				{
					if (is.distance < distance)
					{
						distance = is.distance;
						index = i;
					}
				}
			}

			if (index >= 0)
			{
				objects[index]->Behave(p);
				if (p.behavior == PhotonBehavior::Absorption)
				{
					// KDimensionalTreeに追加
					this->tree.Add(p);
					absorption = true;
				}
				else if (p.behavior != PhotonBehavior::DiffuseReflection &&
						 p.behavior != PhotonBehavior::SpecularReflection)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (!absorption)
			this->tree.Add(p);
	}

	this->tree.Build2();
}

bool Scene::Trace(Vector3 &base,
				  Vector3 &direction,
				  Vector3 &rgb,
				  uint32_t depth)
{
	// 再帰の深さが3以上なら終了
	if (depth > 3)
	{
		rgb << 0.0f, 0.0f, 0.0f;
		return false;
	}

	float distance = 100000.0f;
	int n = -1;
	Intersection is;

	// レイと交差する一番近いオブジェクトを選択
	for (int i = 0; i < this->objects.size(); i++)
	{
		if (objects[i]->Collide(base, direction, is))
		{
			if (is.distance < distance)
			{
				distance = is.distance;
				n = i;
			}
		}
	}

	// 交差するオブジェクトがなければ終了
	if (n < 0)
	{
		rgb << 0.0f, 0.0f, 0.0f;
		return false;
	}
	
	Vector3 diffuse_color;
	Material &material = objects[n]->material;
	float diffuse = Average(material.diffuse);
	
	// Diffuse Reflection
	if (diffuse > 0.0f && distance > 0)
	{
		// std::cout << "diffuse" << std::endl;
		Vector3 intersection = base + direction * distance;
		Vector3 normal = objects[n]->GetNormalVector(intersection);
		// Radiance Estimation
		diffuse_color = tree.Radiance(intersection,
									  0.5f,
									  500,
									  normal);
	}
	else
		std::cout << "oops" << std::endl;

	rgb = diffuse_color;

	return true;
}
