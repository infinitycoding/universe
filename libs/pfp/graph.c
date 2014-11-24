#include <graph.h>
#include <string.h>
#include <stdlib.h>

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the mercury c-library.
 
     The mercury c-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The mercury c-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the mercury c-library.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @copyright GNU Lesser Public License
 */


attribute *createAtributeList()
{
	attribute *newAttrib = calloc(1,sizeof(attribute));
	return newAttrib;
}

int destroyList(attribute *list)
{
	if(list == NULL)
		return 0;
	
	int i = 0;
	
	while(list != NULL)
	{
		if(list->key != NULL)
			free(list->key);
		
		if(list->value != NULL)
			free(list->value);
		
		attribute *next = list->next;
		free(list);
		list = next;
		
		i++;
	}
	
	return i;
}


const char *getListAttribute(attribute *list, const char *key)
{
	if(key == NULL || list == NULL)
		return NULL;

	while(list != NULL)
	{
		if(list->key != NULL)
		{
			if(strcmp(list->key,key) == 0)
				return list->value;
		}
		list = list->next;
	}
	
	// return NULL if element could not be found
	return NULL;
}

int setListAttribute(attribute *list, const char *key,const char *val)
{
	if(key == NULL || val== NULL || list == NULL)
		return 0;

	size_t keyLen = strlen(key)+1; //+1 = terminating zero
	size_t valLen = strlen(val)+1; //+1 = terminating zero
	
	while(list != NULL)
	{
		if(list->key != NULL)
		{
			if(strcmp(list->key,key) == 0)
			{
				free(list->value);
				list->value = malloc(valLen);
				strcpy(list->value,val);
				return 1;
			}
		}
		
		if(list->next == NULL)
			break;
		else
			list = list->next;
	}
	
	attribute *newAttrib = malloc(sizeof(attribute));
	
	newAttrib->key = malloc(keyLen);
	strcpy(newAttrib->key,key);
	
	newAttrib->value = malloc(valLen);
	strcpy(newAttrib->value,val);
	
	newAttrib->prev = list;
	newAttrib->next = NULL;
	
	list->next = newAttrib;
	return 1;
}



graph *createGraph()
{
	graph *g = malloc(sizeof(graph));
	g->attributes = createAtributeList();
	g->rootLayer = createElement();
	g->rootLayer->dummy = 1;
	
	element *leftEdge = createElement();
	element *rightEdge = createElement();
	element *upperEdge = createElement();
	element *lowerEdge = createElement();
	
	g->rootLayer->left = leftEdge;
	leftEdge->right = g->rootLayer;
	
	g->rootLayer->right = rightEdge;
	rightEdge->left = g->rootLayer;
	
	g->rootLayer->up = upperEdge;
	upperEdge->down = g->rootLayer;
	
	g->rootLayer->down = lowerEdge;
	lowerEdge->up = g->rootLayer;
	
	return g;
}


graphIndex *graphIndexCreate(graph *g, seekEdge s)
{
	graphIndex *i = malloc(sizeof(graphIndex));
	i->grph = g;
	i->currentElement = g->rootLayer;
	graphIndexSetEdge(i, s);
	return i;
}

graphIndex *graphIndexSetPosition(graphIndex *i, element *e)
{
	if(e == NULL || i == NULL)
		return NULL;
	//todo: error handling
	
	i->currentElement = e;
	return i;
}

graphIndex *graphIndexSetEdge(graphIndex *i, seekEdge s)
{
	switch(s)
	{
		case LEFT_EDGE:
			while(!isLeftEdge(i))
				graphIndexMovLeft(i);
		break;
		
		case RIGHT_EDGE:
			while(!isLeftEdge(i))
				graphIndexMovLeft(i);
		break;
		
		case UPPER_EDGE:
			while(!isUpperEdge(i))
				graphIndexMovUp(i);
		break;
		
		case LOWER_EDGE:
			while(!isLowerEdge(i))
				graphIndexMovDown(i);
		break;
		
		default:
			return NULL;
	}
	
	return i;
}

void graphIndexDestroy(graphIndex *i)
{
	if(i != NULL)
		free(i);
}

element *createElement()
{
	element *e = calloc(1,sizeof(element));
	e->attributes = createAtributeList();
	return e;
}


element *indexGetCurrent(graphIndex *i)
{
	if(i->currentElement->dummy)
		return NULL;
	
	return i->currentElement;
}


const char *getGraphAttribute(graph *g, const char *attr)
{
	// todo: do error handling
	return getListAttribute(g->attributes, attr);
}


graph *setGraphAttribute(graph *g, const char *attr, const char *val)
{
	if(setListAttribute(g->attributes, attr, val))
		return g;
	// todo error handling
	return NULL;
}


const char *getElementAttribute(element *e, const char *attr)
{
	// todo: do error handling
	return getListAttribute(e->attributes, attr);
}

element *setElementAttribute(element *e, const char *attr, const char *val)
{
	if(setListAttribute(e->attributes, attr, val))
		return e;
	// todo error handling
	return NULL;
}

graphIndex *graphIndexMovUp(graphIndex *i)
{
	if(i->currentElement->up != NULL)
	{
		i->currentElement = i->currentElement->up;
		if(i->currentElement->dummy)
			i->currentElement = i->currentElement->up;
		return i;
	}
	// do errorhandling
	return NULL;
}

graphIndex *graphIndexMovDown(graphIndex *i)
{
	if(i->currentElement->down != NULL)
	{
		i->currentElement = i->currentElement->down;
		if(i->currentElement->dummy)
			i->currentElement = i->currentElement->down;
		return i;
	}
	// do errorhandling
	return NULL;
}

graphIndex *graphIndexMovLeft(graphIndex *i)
{
	if(i->currentElement->left != NULL)
	{
		i->currentElement = i->currentElement->left;
		if(i->currentElement->dummy)
			i->currentElement = i->currentElement->left;
		return i;
	}
	// do errorhandling
	return NULL;
}

graphIndex *graphIndexMovRight(graphIndex *i)
{
	if(i->currentElement->right != NULL)
	{
		i->currentElement = i->currentElement->right;
		if(i->currentElement->dummy)
			i->currentElement = i->currentElement->right;
		return i;
	}
	// do errorhandling
	return NULL;
}


int isLeftEdge(graphIndex *i)
{
	if(i->currentElement->left == NULL)
		return 1;
	return 0;
}


int isRightEdge(graphIndex *i)
{
	if(i->currentElement->right == NULL)
		return 1;
	return 0;
}

int isUpperEdge(graphIndex *i)
{
	if(i->currentElement->up == NULL)
		return 1;
	return 0;
}

int isLowerEdge(graphIndex *i)
{
	if(i->currentElement->down == NULL)
		return 1;
	return 0;
}

graphIndex *graphInsertLeft(graphIndex* i, element *e)
{
	if(isLeftEdge(i) || e == NULL)
		return NULL;
	
	e->right = i->currentElement;
	e->left = i->currentElement->left;
	e->up = createElement();
	e->up->down = e;
	e->down = createElement();
	e->down->up = e;
	
	i->currentElement->left = e;
	e->left->right = e;
	
	return i;
}

graphIndex *graphInsertRight(graphIndex* i, element *e)
{
	if(isRightEdge(i) || e == NULL)
		return NULL;
	
	e->left = i->currentElement;
	e->right = i->currentElement->right;
	e->up = createElement();
	e->up->down = e;
	e->down = createElement();
	e->down->up = e;
	
	i->currentElement->right = e;
	e->right->left = e;
	
	return i;	
	
}

graphIndex *graphInsertAbove(graphIndex* i, element *e)
{
	if(isUpperEdge(i) || e == NULL)
		return NULL;
	
	e->down = i->currentElement;
	e->up = i->currentElement->up;
	e->left = createElement();
	e->left->right = e;
	e->right = createElement();
	e->right->left = e;
	
	i->currentElement->up = e;
	e->up->down = e;
	
	return i;
}

graphIndex *graphInsertBelow(graphIndex* i, element *e)
{
	if(isUpperEdge(i) || e == NULL)
		return NULL;
	
	e->up = i->currentElement;
	e->down = i->currentElement->down;
	e->left = createElement();
	e->left->right = e;
	e->right = createElement();
	e->right->left = e;
	
	i->currentElement->down = e;
	e->down->up = e;
	
	return i;
}




