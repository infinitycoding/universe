#ifndef ULTRASHELL_H
#define ULTRASHELL_H



#include <binaryTree.h>



#define MAX_LINE_LENGTH 50
#define MAX_TOKEN_LENGTH 20


int main(void);
int parserLine(const char *line, binary_tree *tree);
int getTokens(const char *instring, char ***tokens);
int countTokens(const char *instring);

#endif
