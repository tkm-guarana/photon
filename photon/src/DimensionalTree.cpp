

#include <cstdint>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <tuple>
#include <memory>
#include <thread>
#include <mutex>

#include <assert.h>


#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/Geometry"

#include "Geometry.h"
#include "Photon.h"
#include "Material.h"

#include "DimensionalTree.h"



// Dimensional Tree
// ----------------------------
void DimensionalTree::Add(const Photon &photon)
{
	points.push_back(photon);
}

/**
 * 現在の状態で空間分割木を構築する
 */
void DimensionalTree::Build()
{
	uint32_t count = points.size();
	// ノードの一覧を生成
	nodes.resize(count);
	// ソート用の整数リスト
	u32v points(count);

	for (uint32_t i = 0; i < count; i++)
		points[i] = i;

	root_index = this->BuildRec(points, 0);
}

void DimensionalTree::Build2()
{
	uint32_t count = points.size();
	// ノードの一覧を生成
	nodes.resize(count);
	// ソート用の整数リスト
	u32v p(count);
	for (uint32_t i = 0; i < count; i++)
		p[i] = i;

	ThreadVars vars(root_index, p);
	ThreadQue que(vars, 0);
	this->thread_que.push_back(que);

	while (this->thread_que.size() > 0)
	{
		this->RunThread();
	}
}

void DimensionalTree::RunThread()
{
	std::vector<std::thread> threads;
	std::vector<ThreadQue> ques;
	// auto q2 = std::unique_ptr<std::vector<ThreadQue>>(new std::vector<ThreadQue>());

	std::cout << "run thread" << std::endl;

	for (auto &que : this->thread_que)
	{
		std::cout << "raise thread, depth: " << que.depth << std::endl;
		threads.push_back(std::thread(&DimensionalTree::BuildWorker, this, std::ref(que.vars), std::ref(ques), que.depth));
	}
	// スレッド終了待ち
	for (auto &thread : threads)
	{
		thread.join();
	}
	// 入れ替え
	this->thread_que.clear();
	for (auto &q : ques)
	{
		this->thread_que.push_back(q);
	}
}

#if 0
void DimensionalTree::Sort(Axis axis)
{
	switch(axis)
	{
	case Axis::X:
		std::sort(points.begin(), points.end(), photon_pred<Vector3LessX>());
		break;
	case Axis::Y:
		std::sort(points.begin(), points.end(), photon_pred<Vector3LessY>());
		break;
	case Axis::Z:
		std::sort(points.begin(), points.end(), photon_pred<Vector3LessZ>());
		break;
	}
}
#endif

uint32_t DimensionalTree::find_range(Vector3 coord, float distance, uint32_t max_heap)
{
	find_center = coord;
	find_sq_dist = distance*distance;
	find_result.clear();

	return 0;//FindRec(root_index, 0);
}

uint32_t DimensionalTree::size()
{
	return (uint32_t)points.size();
}

// 放射輝度推定 radiance estimation
// 最適化してないゴリ押し探索
Vector3 DimensionalTree::Radiance(const Vector3 &coord,
								  float distance,
								  uint32_t max_heap,
								  const Vector3 &normal)
{
	HeapList list;
	Vector3 center = coord;
	float dist = distance*distance;

	FindRec(list, root_index, center, dist, max_heap, 0);

	uint32_t max = (uint32_t)list.size();
	max = std::min(max, max_heap);

	uint32_t n = 0;
	Vector3 flux{ 0.0f, 0.0f, 0.0f };
	HeapList::iterator itr = list.begin();
	for (uint32_t i = 0; i < max; i++)
	{
		n = itr->number;
		// flux += points[n].spectrum * material->brdf(normal, points[n].incidence, Vector3());
		flux += points[n].spectrum;
		itr++;
	}

	float sr = 1.0f;
	if(max < max_heap) // 要求した数に満たない
	{
		sr = distance*distance;
	}
	else
	{
		Vector3 diff = coord - points[n].position;
		sr = diff.squaredNorm();
	}

	flux /= (3.1415f * sr * max_heap);

	return flux;
}


void DimensionalTree::BuildWorker(ThreadVars &vars, std::vector<ThreadQue> &ques, uint32_t depth)
{
	// 残りの要素がない
	if (vars.points.empty())
	{
		vars.child = endn;
		return;
	}

	// 葉に到達した
	if (vars.points.size() < 2) // reach a leaf.
	{
		uint32_t suffix = vars.points.front();
		this->nodes[suffix].first = endn;
		this->nodes[suffix].second = endn;
		vars.child = suffix;
		return;
	}

	// 指定軸でソートする
	int axis = depth % 3;
	std::sort(vars.points.begin(), vars.points.end(),
			  [this, axis](uint32_t lhs, uint32_t rhs)
			  {
				  return (points[lhs].position(axis) < points[rhs].position(axis));
			  });

	// 中間部分のインデックス
	uint32_t median = (uint32_t)vars.points.size() / 2 + 1;
	
	// 分割する
	u32v before_median(vars.points.begin(), vars.points.begin() + median);
	u32v after_median(vars.points.begin() + median, vars.points.end());

	uint32_t suffix = before_median.back();
	before_median.pop_back(); // before list include median

	ThreadVars vars1(this->nodes[suffix].first, before_median);
	ThreadVars vars2(this->nodes[suffix].second, after_median);

	ThreadQue que1(vars1, depth+1);
	ThreadQue que2(vars2, depth+1);

	this->mtx.lock();

	// スレッドキューを追加
	ques.push_back(que1);
	ques.push_back(que2);

	this->mtx.unlock();
	
	vars.child = suffix;
}


/**
 * 空間分割木を構築する
 */
uint32_t DimensionalTree::BuildRec(u32v &indices, uint32_t depth)
{
	// 残りの要素がない
	if (indices.empty())
	{
		return endn;
	}

	// 葉に到達した
	if (indices.size() < 2) // reach a leaf.
	{
		uint32_t suffix = indices.front();
		nodes[suffix].first = endn;
		nodes[suffix].second = endn;
		return suffix;
	}

	// 指定軸でソートする
	int axis = depth % 3;
	std::sort(indices.begin(), indices.end(),
			  [this, axis](uint32_t lhs, uint32_t rhs)
			  {
				  return (points[lhs].position(axis) < points[rhs].position(axis));
			  });

	// 中間部分のインデックス
	uint32_t median = (uint32_t)indices.size() / 2 + 1;
	
	// 分割する
	u32v before_median(indices.begin(), indices.begin() + median);
	u32v after_median(indices.begin() + median, indices.end());

	uint32_t suffix = before_median.back();
	before_median.pop_back(); // before list include median

	this->nodes[suffix].first = this->BuildRec(before_median, depth + 1);
	this->nodes[suffix].second = this->BuildRec(after_median, depth + 1);

#ifdef _DEBUG
	{
		bool b = !(nodes[suffix].first == 0 && nodes[suffix].second == 0);
		assert(b);
	}
#endif
	return suffix;
}

/**
 * FindRec
 * 再帰探索
 */
uint32_t DimensionalTree::FindRec(HeapList &list,	// 最近傍フォトン格納コンテナ
								  uint32_t node,	// 探索対象ノード
								  Vector3 &center,	// 探索座標
								  float &dist,		// 探索距離
								  uint32_t max,		// 最大収集数
								  uint32_t depth)	// 再帰の深さ
{
	if (node == endn)
		return 0; // invalid call

	// 現在のノードに保存されているフォトンと、
	// 探索地点の距離をとる
	int k = depth % 3;
	float d = center(k) - points[node].position(k);

	// 軸の正負によって処理を分岐
	if (d < 0) // point in left side
	{
		// 子ノードを起点に探索する
		FindRec(list, nodes[node].first, center, dist, max, depth + 1);
		// これがなんかわからないが、距離の条件にひっかかった場合
		// のみ探索するっぽい
		if (d*d < dist)
		{
			FindRec(list, nodes[node].second, center, dist, max, depth + 1);
		}
	}
	else // other side
	{
		FindRec(list, nodes[node].second, center, dist, max, depth + 1);
		if (d*d < dist)
		{
			FindRec(list, nodes[node].first, center, dist, max, depth + 1);
		}
	}

	{
		// 直線距離の二乗
		Vector3 diff = center - points[node].position;
		d = diff.squaredNorm();
	}
	// このフォトンが探索起点より一定範囲内にある
	if(d < dist) // point in range
	{
		list.insert(HeapItem(d, node));
		// 最大収集個数を越えていたら
		if(list.size() > max)
		{
			// 最後の要素(一番遠いフォトン)を取り除く
			list.erase(list.end()--, list.end()); // 増過ぎていたら消す
			HeapList::iterator itr = list.end();
			itr--;
			// distを一番遠いフォトンの距離として更新
			dist = itr->distance; // 最大距離を更新(参照なので引き継がれる)
		}
	}

	return 0;
}
