

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pfp.h>



bool is_name(char *in);
bool is_whitespace(char *str);

char *whitespace_handle(char *str, struct parser_state *state);
char *name_handle(char *str, struct parser_state *state);


bool (*expression_checks[])(char *str) = {is_name, is_whitespace, NULL};
char *(*expression_handles[])(char *str, struct parser_state *state) = {name_handle, whitespace_handle, NULL};


int is_expression(char *str)
{
    int i;
    for(i = 0; expression_checks[i] != NULL; i++)
    {
        if(expression_checks[i](str))
            break;
    }

    if(expression_checks[i] == NULL)
    {
        return -1;
    }

    return i;
}

char *call_expression_handle(int num, char *str, struct parser_state *state)
{
    if(expression_handles[num] == NULL)
    {
        printf("Expression type %d\n", num);
        return str;
    }

    return expression_handles[num](str, state);
}

bool is_whitespace(char *str)
{
    return ((*str == ' ' || *str == '\t' || *str == '\n') ? true : false);
}

char *whitespace_handle(char *str, struct parser_state *state)
{
    for(;is_whitespace(str); str++);
    return str;
}


bool is_name(char *in)
{
    char c = *in;
    if(isalnum(c) || c == '_' || c == '.')
        return true;
    return false;
}


char *name_handle(char *str, struct parser_state *state)
{
    if(!is_name(str))
        return str;

    size_t name_len = 0;
    while(is_name(&str[name_len]) && name_len < strlen(str))
        name_len++;
    
    struct pnode *node;

    switch(state->mode)
    {
        case DEFAULT_MODE:
            printf("unknown identifier\n");
            exit(1);
        break;

        case SERVICE_MODE:
        case SECTION_MODE:
            state->node = malloc(sizeof(struct pnode));
            state->node->type = DRIVER;
            state->node->file = malloc(name_len+1);
            memcpy(state->node->file,str,name_len);
            state->node->file[name_len] = 0; // terminator
            state->node->fallback = 0;
            state->node->subtree = NULL;
            if(!state->section->subtree)
                state->section->subtree = list_create();
            list_push_front(state->section->subtree, state->node);
            printf("Pipline\n%s\n",state->node->file);
            state->mode = PIPE_MODE;
        break;

        case SECTION_NAME_MODE:
            state->section->name = malloc(name_len+1);
            memcpy(state->section->name, str, name_len);
            state->section->name[name_len] = 0;
            printf("%s\n",state->section->name);
        break;

        case SERVICE_NAME_MODE:
            state->node->file = malloc(name_len+1);
            memcpy(state->node->file,str,name_len);
            state->node->file[name_len] = 0; // terminator
            printf("%s\n", state->node->file);
        break;


        case CHILDREN_MODE:
            list_push_front(state->mode_stack,(void *) state->mode);
        case PIPE_MODE:
            state->node->file = malloc(name_len+1);
            memcpy(state->node->file,str,name_len);
            state->node->file[name_len] = 0; // terminator
            state->mode = PIPE_MODE;
            printf("%s\n", state->node->file);
        break;




        default:
            printf("unknown expression\n");
            exit(1);
        break;

    };
    return str+name_len;
}