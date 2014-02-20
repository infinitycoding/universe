#ifndef BINARYTREE_H_INCLUDED
#define BINARYTREE_H_INCLUDED



enum branchType
{
	moreBranching, branchEnd
};


struct condition	// not sure if i really want to make it like this
{
	int number;
	char value;
};


struct function
{
	char *command;
	int (*f)(int, char **);
};


struct branching
{
	struct condition *c;
	struct branch *left;
	struct branch *right;
};


struct branch
{
	enum branchType type;
	union branchElement *element;
};


union branchElement
{
	struct branching *b;
	struct function *f;
};



#endif
