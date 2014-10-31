
#ifndef _dimensional_tree_h_
#define _dimensional_tree_h_


typedef unsigned char	byte;
typedef unsigned short	word;
typedef unsigned long	dword;
typedef unsigned int	uint;


enum class Axis
{
	X = 0,
	Y,
	Z,
};

class DimensionalTree
{
private:
	typedef std::pair<uint32_t, uint32_t>	node;
	typedef std::vector<uint32_t>			u32v;

public:

	struct ThreadVars
	{
		uint32_t &child;
		u32v points;

		ThreadVars(uint32_t &_child, u32v _points) :
		child(_child), points(_points)
		{
			/* this->points. */
		}

		ThreadVars(const ThreadVars &vars) :
		child(vars.child), points(vars.points)
		{
		}
	};

	struct ThreadQue
	{
		ThreadVars vars;
		int depth = 0;

		ThreadQue(ThreadVars &_vars, int _depth) :
		vars(_vars), depth(_depth)
		{
		}

		ThreadQue(const ThreadQue &que) :
		vars(que.vars), depth(que.depth)
		{
		}
	};


private:

	// ノードのリスト
	std::vector<node>	nodes;

	
public:
	// フォトンの座標リスト
	std::vector<Photon> points;

private:
	std::vector<ThreadQue> thread_que;
	std::vector<ThreadQue> thread_que2;
	std::mutex mtx;

	uint32_t	root_index;
	float		find_sq_dist; // squared distance
	Vector3		find_center;
	Vector3 	find_normal;
	u32v		find_result;
	uint32_t	find_heap;

	struct HeapItem
	{
		float distance;
		uint32_t number;
		HeapItem(float d, uint32_t n) : distance(d), number(n) {}
	};
	// ソート用のクラスの定義
	// 距離によるソートを行なう
	struct HeapSorter : public std::binary_function<HeapItem, HeapItem, bool>
	{
		bool operator()(const HeapItem &lhs, const HeapItem &rhs) const
		{
			return lhs.distance < rhs.distance;
		}
	};
	typedef std::set<HeapItem, HeapSorter>	HeapList;

	uint32_t BuildRec(u32v &indices, uint32_t depth);
	uint32_t FindRec(HeapList &list, uint32_t node, Vector3 &center, float &dist, uint32_t max, uint32_t depth);

	void BuildWorker(ThreadVars &vars, std::vector<ThreadQue> &ques, uint32_t depth);

public:

	// dimension axis
	enum
	{
		endn = 0xFFFFFFFF // end node
	};

public:
	DimensionalTree() = default;
	virtual ~DimensionalTree() = default;

	void Add(const Photon &photon);
	void Build();
	void Build2();
	void RunThread();

	uint32_t size();
	Vector3 Radiance(const Vector3 &coord,
					 float distance,
					 uint32_t max_heap,
					 const Vector3 &normal);
};


#endif
