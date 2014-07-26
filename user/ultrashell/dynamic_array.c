#include <dynamic_array.h>
#include <string.h>
#include <stdlib.h>


dynamic_array *create_array(int blcksize)
{
    if(blcksize <= 0)
        return NULL;

    dynamic_array *new_array = malloc(sizeof(dynamic_array));

	if(new_array == NULL)
		return NULL;

    new_array->blocksize = blcksize;
	new_array->usr_errors = 0;
	new_array->array_errors = 0;
    new_array->first = malloc(sizeof(dynamic_array_block));

	if(new_array->first == NULL)
	{
		free(new_array);
		return NULL;
	}
			
    new_array->first->value = malloc(sizeof(char) * new_array->blocksize);

	if(new_array->first->value == NULL)
	{
		free(new_array->first);
		free(new_array);
		return NULL;
	}

    new_array->first->next = NULL;

    return new_array;
}


dynamic_array *delete_array(dynamic_array *array)
{
	if(array == NULL)
		return NULL;

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
	if(array == NULL)
		return 0;

	if(array->blocksize <= 0)
	{
		array->array_errors++;
		return 0;
	}

    int block = (nr / array->blocksize);
    int element = (nr % array->blocksize);

    dynamic_array_block *current = array->first;

    for(; block > 0; block--)
    {
		if(current == NULL)
		{
			array->usr_errors++;
			return 0;
		}

        current = current->next;
    }

	if(current->value == NULL)
	{
		array->array_errors++;
		return 0;
	}

    return current->value[element];
}


char set(dynamic_array *array, int nr, char val)
{
	if(array == NULL)
		return 0;

	if(array->blocksize <= 0)
	{
		array->array_errors++;
		return 0;
	}

    int block = (nr / array->blocksize);
    int element = (nr % array->blocksize);

    dynamic_array_block *current = array->first;

    for(; block > 0; block--)
    {
        if(current->next == NULL)
        {
            current->next = (dynamic_array_block *)malloc(sizeof(dynamic_array_block));

			if(current->next == NULL)
				return 0;

            current->next->next = NULL;
            current->next->value = (char *)malloc(sizeof(char) * array->blocksize);
        
			if(current->next->value == NULL)
			{
				free(current->next);
				return 0;
        	}
		}

        current = current->next;
    }

	if(current->value == NULL)
	{
		array->array_errors++;
		return 0;
	}

    return (current->value[element] = val);
}


char *as_string(dynamic_array *array)
{
	if(array == NULL)
		return NULL;

    int blocknr;
    dynamic_array_block *current = array->first;

    for(blocknr = 0; current != NULL; blocknr++)
        current = current->next;

    if(blocknr <= 0)
	{
		array->array_errors++;
        return 0;
	}

    char *string = (char *)malloc(array->blocksize * blocknr * sizeof(char));

	if(string == NULL)
		return NULL;

    for(current = array->first, blocknr = 0; current != NULL; current = current->next, blocknr++)    
        strncpy(&string[array->blocksize * blocknr], current->value, array->blocksize * sizeof(char));

    return string;
}


dynamic_array_status get_status(dynamic_array *array)
{
	if(array == NULL)
		return status_unexistent;
	else if(array->array_errors > 0)
		return status_high_damage;
	else if(array->usr_errors > 0)
		return status_low_damage;
	else
		return status_normal;
}
