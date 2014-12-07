/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @brief A XML parser derived from michamimosas Dasyur
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */ 


#include <xml.h>
#include <string.h>
#include <printf.h>
#include <mm/heap.h>


xml_param *xml_param_create()
{
	xml_param *p = malloc(sizeof(xml_param));
	p->name = NULL;
	p->value = NULL;
	return p;
}


void xml_param_set(xml_param * p, const char *name, const char*value)
{
	size_t name_len = strlen(name);
	size_t value_len = strlen(value);
	
	char *mem = malloc(name_len + value_len + 2); // string lenghs plus terminating zeros
	strcpy(mem,name);
	p->name = mem;
	mem += name_len+1;
	strcpy(mem,value);
	p->value = mem;
}

xml_node *xml_node_create()
{
	xml_node *node = malloc(sizeof(xml_node));
	node->name = NULL;
	node->text = NULL;
	node->params = list_create();
	node->subnodes = list_create();
	node->parent = NULL;
	return node;
}

xml_node *xml_node_get_subnode_by_name(xml_node *node, const char *name)
{
	iterator_t it = iterator_create(node->subnodes);
	while(!list_is_last(&it))
	{
		xml_node *subnode = (xml_node *)list_get_current(&it);
		if(subnode->name != NULL)
		{
			if(strcmp(subnode->name,name) == 0)
				return subnode;
		}
		list_next(&it);
	}
	return NULL;
}

xml_node *xml_node_get_subnode_by_param(xml_node *node, const char *name, const char *value)
{
	iterator_t node_it = iterator_create(node->subnodes);
	while(!list_is_last(&node_it))
	{
		xml_node *subnode = list_get_current(&node_it);
		iterator_t param_it = iterator_create(subnode->params);
		
		while(!list_is_last(&param_it))
		{
			xml_param *param = (xml_param *) list_get_current(&param_it);
			if(param->name != NULL && param->value!= NULL)
			{
				if(strcmp(param->name, name) == 0 && strcmp(param->value,value) == 0)
					return subnode;
			}
			list_next(&param_it);
		}
		
		list_next(&node_it);
	}
	return NULL;
}


char *xml_node_parser(xml_node *node ,char *str)
{
	int m = 0, n = 0;
	int j = 0;
	size_t len = strlen(str);
	
	parser_status_t status = STATUS_TEXT;
	node->text = malloc(len+1);
	
	xml_node *subnode = NULL;
	xml_param *param = NULL;
	
	char compare_name[256]; // TODO: dynamic
	
	while(*str != '\0')
    {
        switch(*str)
        {
            case '<':
                if(status == STATUS_TEXT)
                {
                    str++;
                    if(*str == '/')
                    {
                        status = STATUS_TAG_END;
                        str++;
                    }
                    else
                    {
                        subnode = xml_node_create();
                        subnode->name = (char*) malloc(256); // TODO: dynamic
                        status = STATUS_TAG_NAME;
                    }
                }
                break;
            case '>':
                if(status == STATUS_TAG_END)
                {
                    compare_name[n] = '\0';
                    n = 0;
                    str++;

                    if(node->name != NULL)
                    {
                        if(strcmp(compare_name, node->name) == 0)
                        {
                            node->text[j] = '\0';
                            //printf("</%s>", node->name);
                            return str;
                        }
                        else
                        {
                            //printf("</CLOSE: %s, expected %s>", compare_name, node->name);
                        }
                    }
                    status = STATUS_TEXT;
                }
                else if(status != STATUS_TEXT && subnode != NULL)
                {
                    subnode->name[m] = '\0';
                    m = 0;
                    //printf("<%s>", subnode->name);

                    str = xml_node_parser(subnode, ++str);
                    list_push_front(node->subnodes,subnode);

                    subnode = NULL;
                    status = STATUS_TEXT;
                    continue;
                }
                break;
            case ' ':
                if(status == STATUS_TAG_NAME || status == STATUS_TAG_SPACE)
                {
                    str++;
                    if(*str != ' ')
                    {
                        status = STATUS_PARAM_NAME;
                        param = xml_param_create();
                        param->name = (char*) malloc(256); // TODO: DYNAMIC!!!
                        param->value = (char*) malloc(256); //  TODO: DYNAMICCC!!!!!!!!
                    }
                    else
                    {
                        status = STATUS_TAG_SPACE;
                    }
                }
                break;
            case '=':
                if(status == STATUS_PARAM_NAME)
                {
                    str++;
                    if(*str == '\"')
                    {
                        param->name[n] = '\0';
                        n = 0;

                        str++;
                        status = STATUS_PARAM_VALUE;
                    }
                }
                break;
            case '\"':
                if(status == STATUS_PARAM_VALUE)
                {
                    param->value[n] = '\0';
                    n = 0;
                    list_push_front(subnode->params, param);

                    status = STATUS_TAG_SPACE;
                    str++;
                }
                break;
        }

        switch(status)
        {
            case STATUS_TEXT:
                //printf("%c", *str);
                node->text[j++] = *str++;
                break;
            case STATUS_TAG_NAME:
                if(subnode != NULL)
                {
                    subnode->name[m++] = *str++;
                }
                else
                {
                    printf("something went wrong.\n");
                }
                break;
            case STATUS_PARAM_NAME:
                if(param != NULL)
                {
                    param->name[n++] = *str++;
                }
                break;
            case STATUS_PARAM_VALUE:
                if(param != NULL)
                {
                    param->value[n++] = *str++;
                }
                break;
            case STATUS_TAG_END:
                compare_name[n++] = *str++;
                break;
        }
    }

    return str;
	
}
