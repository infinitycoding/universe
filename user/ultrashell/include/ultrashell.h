#ifndef ULTRASHELL_H
#define ULTRASHELL_H



#define MAX_TOKEN_LENGTH 20


int main(void);
int parserLine(const char *line);
int getTokens(const char *instring, char ***tokens);
int countTokens(const char *instring);

#endif
