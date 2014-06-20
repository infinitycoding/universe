#include <tokens.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pfp.h>





char *identifiers[] = {"#", "section", "service", "->", "|", "[", "]", "~{", "}", ";", ":", "," ,"(", ")", NULL};
char *(*identifier_handles[])(char *str, struct parser_state *state) = {comment_handle, section_handle, service_handle, kernel_root_handle, pipe_handle, open_square_brace_handle, NULL, NULL, NULL, semicolon_handle, colon_handle};

bool (*expression_checks[])(char *str) = {is_name, is_whitespace, NULL};
char *(*expression_handles[])(char *str, struct parser_state *state) = {name_handle, whitespace_handle, NULL};

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

/**
 * @brief checks if a character is a whitespace
 * @param character the character
 * @return true or false
 */
bool is_whitespace(char *str)
{
    return ((*str == ' ' || *str == '\t' || *str == '\n') ? true : false);
}


char *skip_whitespaces(char *str)
{
    for(; is_whitespace(str); str++);
    return str;
}


bool is_comment(char *in)
{
	if(*in == '#')
		return true;
	return false;
}

size_t comment_lengh(char *in)
{
	size_t i = 0;
	while((in[i] != '\n') && (i < strlen(in)))
		i++;
	return i;
}

char *comment_handle(char *str, struct parser_state *state)
{
	printf("comment lengh: %d\n",comment_lengh(str));
	return str+comment_lengh(str);
}

bool is_name(char *in)
{
	char c = *in;
	if(isalnum(c) || c == '_' || c == '.')
		return true;
	return false;
}

size_t name_lengh(char *in)
{
	int i = 0;
	while(is_name(&in[i]) && i < strlen(in))
		i++;
	
	return i;
}

char *whitespace_handle(char *str, struct parser_state *state)
{
    return skip_whitespaces(str);
}

char *name_handle(char *str, struct parser_state *state)
{
    if(!is_name(str))
        return str;

    size_t name_len = name_lengh(str);
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

