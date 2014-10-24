
#ifndef _DIMENSIONALTREE_H_
#define _DIMENSIONALTREE_H_



// K-Dimensional Tree Node
class TreeNode
{
private:	
	std::unique_ptr<TreeNode> left;
	std::unique_ptr<TreeNode> right;
public:
	TreeNode(std::unique_ptr<TreeNode> &_left,
			 std::unique_ptr<TreeNode> &_right);
};


enum class Axis : int
{
	X = 0,
	Y = 1,
	Z = 2,
};


// K-Dimentional Tree
class DimensionalTree
{
private:
	std::unique_ptr<TreeNode> BuildRec(int depth);

public:
	DimensionalTree();
	~DimensionalTree();

	
	std::unique_ptr<TreeNode> Build();
};



#endif /* _DIMENSIONALTREE_H_ */
