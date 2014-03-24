#ifndef ULTRASHELL_H
#define ULTRASHELL_H



#include <binaryTree.h>



#define MAX_LINE_LENGTH 50
#define MAX_TOKEN_LENGTH 20


int main(void);
binary_tree *initBinaryTree();
int parserLine(binary_tree *tree, const char *line);
int getTokens(const char *instring, char ***tokens);
int countTokens(const char *instring);

#endif
