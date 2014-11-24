#ifndef _lbps_graph_h_
#define _lbps_graph_h_

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
/***
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @copyright GNU Lesser Public License
 */

typedef enum
{
	LEFT_EDGE = 0,
	RIGHT_EDGE = 1,
	UPPER_EDGE = 2,
	LOWER_EDGE = 3
}seekEdge;

struct attribute
{
	struct attribute *prev;
	struct attribute *next;
	char *key;
	char *value;
};

typedef struct attribute attribute;

struct element
{
	struct element *left;
	struct element *right;
	struct element *up;
	struct element *down;
	attribute *attributes;
	int dummy;
};

typedef struct element element;

typedef struct 
{
	element *rootLayer;
	attribute *attributes;
}graph;



typedef struct 
{
	graph *grph;
	element *currentElement;
}graphIndex;


graph *createGraph();
int destroyGraph(graph *g);

graphIndex *graphIndexCreate(graph *g, seekEdge s);
graphIndex *graphIndexSetPosition(graphIndex *i, element *e);
graphIndex *graphIndexSetEdge(graphIndex *i, seekEdge s);
void graphIndexDestroy(graphIndex *i);


element *createElement();

element *indexGetCurrent(graphIndex *i);

const char *getElementAttribute(element *e, const char *attr);
element *setElementAttribute(element *e, const char *attr, const char *val);

const char *getGraphAttribute(graph *g, const char *attr);
graph *setGraphAttribute(graph *g, const char *attr, const char *val);



graphIndex *graphIndexMovUp(graphIndex *i);
graphIndex *graphIndexMovDown(graphIndex *i);
graphIndex *graphIndexMovLeft(graphIndex *i);
graphIndex *graphIndexMovRight(graphIndex *i);

int isLeftEdge(graphIndex *i);
int isRightEdge(graphIndex *i);
int isUpperEdge(graphIndex *i);
int isLowerEdge(graphIndex *i);

graphIndex *graphInsertLeft(graphIndex* i,element *e);
graphIndex *graphInsertRight(graphIndex* i,element *e);
graphIndex *graphInsertAbove(graphIndex* i,element *e);
graphIndex *graphInsertBelow(graphIndex* i,element *e);


void pushLeft(graphIndex *i, element *e);
void pushRight(graphIndex *i, element *e);
void pushUp(graphIndex *i,element *l);
void pushDown(graphIndex *i,element *l);

element *popLeft(graphIndex *i);
element *popRight(graphIndex *i);
element *popUp(graphIndex *i);
element *popDown(graphIndex *i);




int writeGraph(graph *g, const char *file);
graph *readGraph(const char *file);

#endif