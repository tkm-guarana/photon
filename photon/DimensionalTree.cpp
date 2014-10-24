


#include <memory>

#include "DimensionalTree.h"


TreeNode::TreeNode(std::unique_ptr<TreeNode> &_left,
				   std::unique_ptr<TreeNode> &_right)
{
	// nothing to do
	this->left = std::move(_left);
	this->right = std::move(_right);
}

DimensionalTree::DimensionalTree()
{

}

DimensionalTree::~DimensionalTree()
{

}



std::unique_ptr<TreeNode> DimensionalTree::Build()
{
	return this->BuildRec(0);
}

std::unique_ptr<TreeNode> DimensionalTree::BuildRec(int depth = 0)
{
	if (depth > 10)
		return std::unique_ptr<TreeNode>(nullptr);

	Axis k = (Axis)(depth % 3);

	switch (k)
	{
	case Axis.X:
		break;
	}

	printf("depth = %d\n", depth);

	auto left = this->BuildRec(depth + 1);
	auto right = this->BuildRec(depth + 1);
	std::unique_ptr<TreeNode> node(new TreeNode(left, right));

	return node;
}
