#include <dynamic_array.h>
#include <string.h>
#include <stdlib.h>


dynamic_array *create_array(int blcksize)
{
    if(blcksize <= 0)
        return NULL;

    dynamic_array *new_array = malloc(sizeof(dynamic_array));
    new_array->blocksize = blcksize;
    new_array->first = malloc(sizeof(dynamic_array_block));
    new_array->first->value = malloc(sizeof(char) * new_array->blocksize);
    new_array->first->next = NULL;

    return new_array;
}


dynamic_array *delete_array(dynamic_array *array)
{
    dynamic_array_block *current = array->first;
    dynamic_array_block *before = NULL;

    while(current != NULL)
    {
        before = current;
        current = current->next;
        free(before->value);
        free(before);
    }

    return NULL;
}


char get(dynamic_array *array, int nr)
{
    int block = (nr / array->blocksize);
    int element = (nr % array->blocksize);

    dynamic_array_block *current = array->first;

    for(; block > 0; block--)
    {
        current = current->next;

        if(current == NULL)
            return 0;
    }

    return current->value[element];
}


char set(dynamic_array *array, int nr, char val)
{
    int block = (nr / array->blocksize);
    int element = (nr % array->blocksize);

    dynamic_array_block *current = array->first;

    for(; block > 0; block--)
    {
        if(current->next == NULL)
        {
            current->next = (dynamic_array_block *)malloc(sizeof(dynamic_array_block));
            current->next->next = NULL;
            current->next->value = (char *)malloc(sizeof(char) * array->blocksize);
        
            #ifdef DYN_ARRAY_DEBUG
                printf("created new block in dynamic array\n");
            #endif
        }

        current = current->next;
    }

    return (current->value[element] = val);
}


char *as_string(dynamic_array *array)
{
    #ifdef DYN_ARRAY_DEBUG
        printf("converting array to string\n");
    #endif

    int blocknr;
    dynamic_array_block *current = array->first;

    for(blocknr = 0; current != NULL; blocknr++)
        current = current->next;

    #ifdef DYN_ARRAY_DEBUG
        printf("blocknumber in array = %d\n", blocknr);
    #endif

    if(blocknr <= 0)
        return 0;

    char *string = (char *)malloc(array->blocksize * blocknr * sizeof(char));

    for(current = array->first, blocknr = 0; current != NULL; current = current->next, blocknr++)
    {    
        #ifdef DYN_ARRAY_DEBUG
            printf("copying block nr %d\n", blocknr);
        #endif
    
        strncpy(&string[array->blocksize * blocknr], current->value, array->blocksize * sizeof(char));
    }

    #ifdef DYN_ARRAY_DEBUG
        printf("converted array to string\n");
    #endif

    return string;
}