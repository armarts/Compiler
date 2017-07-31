#include "tree.h"

Tree* makeNode(NodeType nodeType, Tree *left, Tree *right)
{
    Tree *tmp = (Tree*) calloc(1, sizeof(Tree));
    tmp->left = left;
    tmp->right = right;

    tmp->nodeType = nodeType;
    return tmp;
}

Tree* makeLeaf(NodeType nodeType, int value)
{
    Tree *tmp = (Tree*) calloc(1, sizeof(Tree));
    tmp->left = tmp->right = NULL;
    
	tmp->nodeType = nodeType;
    tmp->value = value;
}
