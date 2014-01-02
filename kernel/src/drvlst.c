#include <stdint.h>
#include <string.h>


/**
 *@param 0 the table token
 *@param 1  the driver list
 **/
char **get_tabel_section(char * token, char *table)
{
    int token_len = strlen(token);
    char read_token[token_len+1];

    int i = 0, j,k;

    int num_entries = 0;
    int entries_start;
    char **entries;

    // search for the token
    memcpy(read_token, table + i, token_len);
    read_token[token_len] = '\0';
    while(!strcmp(read_token, token) && table[i] != '\0') {
      memcpy(read_token, table + i, token_len);
      read_token[token_len] = '\0';
    }

    // search '{'
    while( table[i] != '{' ) {
        i++;
    }
    i++;

    entries_start = i;

    // count entries until '}'
    while( table[i] != '}' && table[i] != '\0') {
        while(table[i] == ' ' || table[i] == '\n') {
            i++;
        }
	num_entries++;
        while(table[i] != ' ' && table[i] != '\n') {
            i++;
        }
        i++;
    }
    printf("%d entries\n", num_entries);
    entries = malloc(num_entries * sizeof(uintptr_t));
    int start[num_entries];
    int len[num_entries];
    memset(start, 0, num_entries * sizeof(int));
    memset(len, 0, num_entries * sizeof(int));

    i = entries_start;
    for(j = 0; j < num_entries; j++) {
        while( table[i] == ' ' || table[i] == '\n') i++;
        while( table[i] != ' ' && table[i] != '\n') {
            if(start[j] == 0) start[j] = i;
            len[j]++;
            i++;
        }
    }
    
    // copy
    for(j = 0; j < num_entries; j++) {
        i = start[j];
        entries[j] = malloc(len[j]+1);
        for(k = 0; k < len[j]; k++) {
            entries[j][k] = table[i];
            i++;
        }
        entries[k] = '\0';
    }

    return entries;
}
