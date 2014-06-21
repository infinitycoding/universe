


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pfp.h>



char *comment_handle(char *str, struct parser_state *state);
char *section_handle(char *str, struct parser_state *state);
char *kernel_root_handle(char *str, struct parser_state *state);
char *pipe_handle(char *str, struct parser_state *state);
char *semicolon_handle(char *str, struct parser_state *state);
char *colon_handle(char *str, struct parser_state *state);
char *service_handle(char *str, struct parser_state *state);
char *open_square_brace_handle(char *str, struct parser_state *state);
char *comma_handle(char *str, struct parser_state *state);

char *identifiers[] = {"#", "section", "service", "->", "|", "[", "]", "~{", "}", ";", ":", "," ,"(", ")", NULL};
char *(*identifier_handles[])(char *str, struct parser_state *state) = {comment_handle, section_handle, service_handle, kernel_root_handle, pipe_handle, open_square_brace_handle, NULL, NULL, NULL, semicolon_handle, colon_handle, comma_handle};



int is_identifier(char *str)
{
    int i;
    size_t str_len = strlen(str);
    for(i = 0; identifiers[i] != NULL; i++)
    {
        size_t id_len = strlen(identifiers[i]);
        if(id_len > str_len)
            continue;
        if(strncmp(str,identifiers[i],id_len) == 0)
            break;
    }

    if(identifiers[i] == NULL)
    {
        return -1;
    }

    return i;
}

char *call_identifier_handle(int num, char *str, struct parser_state *state)
{
    str = str+strlen(identifiers[num]);

    if(identifier_handles[num] == NULL)
    {
        printf("Identifier %s\n", identifiers[num]);
        return str;
    }
    return identifier_handles[num](str,state);
}

char *comment_handle(char *str, struct parser_state *state)
{
    size_t i = 0;
    while((str[i] != '\n') && (i < strlen(str)))
        i++;
    printf("comment lengh: %d\n",i);
    return str+i;
}


char *section_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (DEFAULT_MODE|SECTION_MODE)))
    {
        printf("invalid identifier section\n");
        exit(1);
    }

    if(state->section)
    {
        if(!state->section_list)
        {
            state->section_list = list_create();
        }
        list_push_front(state->section_list, state->section);
    }


    state->node = NULL;
    state->section = malloc(sizeof(struct section));
    state->section->type = 0;
    state->section->name = NULL;
    state->section->subtree = NULL;
    state->mode = SECTION_NAME_MODE;
    printf("Section ");
    return str;
}

char *service_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & SECTION_MODE))
    {
        printf("invalid identifier service\n");
        exit(1);
    }

    list_push_front(state->mode_stack,(void *) state->mode);
    state->mode = SERVICE_NAME_MODE;

    struct pnode *new_node = malloc(sizeof(struct pnode));

    if(!state->section->subtree)
        state->section->subtree = list_create();
    list_push_front(state->section->subtree, new_node);

    state->node = new_node;
    new_node->type = SERVICE;
    new_node->file = NULL;
    printf("Service: %s");

    return str;
}

char *kernel_root_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (SECTION_MODE | SERVICE_MODE)) || !state->section)
    {
        printf("no section specified before first pipe (->)\n");
        exit(1);
    }

    printf("Kernel Root Nodes:\n");
    list_push_front(state->mode_stack,(void *) state->mode);


    struct pnode *new_node = malloc(sizeof(struct pnode));

    if(!state->node || state->mode == SECTION_MODE)
    {
        if(!state->section->subtree)
            state->section->subtree = list_create();
        list_push_front(state->section->subtree, new_node);
    }
    else
    {
        if(!state->node->subtree)
            state->node->subtree = list_create();
        list_push_front(state->node->subtree, new_node);
    }
    new_node->parent = NULL;
    state->mode = PIPE_NAME_MODE;
    state->node = new_node;
    new_node->type = KERNELROOT;

    return str;
}

char *pipe_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & PIPE_MODE))
    {
        printf("no section or driver tree specified before first pipe (|) %d\n",state->mode);
        exit(1);
    }

    printf("Pipe\n");
    struct pnode *new_node = malloc(sizeof(struct pnode));

    if(!state->node->subtree)
            state->node->subtree = list_create();
    list_push_front(state->node->subtree, new_node);
    state->node = new_node;
    new_node->type = NOTHING;

    return str;

}




char *open_square_brace_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (SERVICE_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \"[\"\n");
        exit(1);
    }

    list_push_front(state->mode_stack,(void *) state->mode);
    state->mode = CHILDREN_MODE;

    return str;
}



char *colon_handle(char *str, struct parser_state *state)
{
    if(! (state->mode &  (SERVICE_NAME_MODE | SECTION_NAME_MODE)))
    {
        printf("invalid identifier \":\"\n");
        exit(1);
    }

    if(state->mode & SERVICE_NAME_MODE)
        state->mode = SERVICE_MODE;
    else
        state->mode = SECTION_MODE;
    return str;

    
}

char *semicolon_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (SERVICE_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \";\"\n");
        exit(1);
    }

    if(state->mode & PIPE_MODE)
    {
        state->mode = (parser_mode_t) list_pop_front(state->mode_stack);
        if(state->mode & SERVICE_MODE)
            state->mode = (parser_mode_t) list_pop_front(state->mode_stack);
    }
    else
    {
        state->mode = (parser_mode_t) list_pop_front(state->mode_stack);
    }
    state->node = NULL;
    return str;
}


char *comma_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (CHILDREN_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \",\"\n");
        exit(1);
    }

    if(state->mode & PIPE_MODE )
    {
        state->mode = (parser_mode_t) list_pop_front(state->mode_stack);
        if(state->mode != CHILDREN_MODE)
        {
            printf("invalid identifier \",\"\n");
            exit(1);
        }
    }
    else
    {
        list_push_front(state->mode_stack,(void *) state->mode);
        state->mode = PIPE_MODE;
    }
    return str;
}