#include <stdio.h>
#include <graph.h>
#include <string.h>

#define NUM_OF_ELEMENTS 400
#define NUM_OF_LEFT_ELEMENTS 100
#define NUM_OF_RIGHT_ELEMENTS 100
#define NUM_OF_UPPER_ELEMENTS 100
#define NUM_OF_LOWER_ELEMENTS 100


void graphTest()
{
	printf("Create Graph\n");
	graph *g = graphCreate();
	
	printf("Test setGraphAttribute and getGraphAttribute\n");
	graphSetAttribute(g, "color", "blue");
	graphSetAttribute(g, "blubb", "fish");
	graphSetAttribute(g, "grr", "wulf");
	graphSetAttribute(g, "color", "red");
	
	const char *color = graphGetAttribute(g, "color");
	if(strcmp(color, "red")!= 0)
		printf("Error: Attribute[color] = %s instead of red\n",color);
	else
		printf("Sucess!\n");
	
	printf("Create %d elements\n",NUM_OF_ELEMENTS);
	
	int e, errors = 0;
	element *newElements[NUM_OF_ELEMENTS];
	for(e = 0; e < NUM_OF_ELEMENTS; e++)
	{
		newElements[e] = elementCreate();
		if(newElements[e] == NULL)
			errors++;
	}
	
	if(errors != 0)
	{
		printf("failed to create %d/%d graph elements\nStopping test...",errors,NUM_OF_ELEMENTS);
		return;
	}
	else
		printf("Success!\n");
	
	printf("Create graphIndex\n");
	graphIndex *i = graphIndexCreate(g, LEFT_EDGE);
	if(i != NULL)
		printf("Success\n");
	else
		printf("Error!\n");
	
	printf("Insert %d right elements\n",NUM_OF_RIGHT_ELEMENTS);
	int baseIndex = 0;
	errors = 0;
	for(e = 0; e < NUM_OF_RIGHT_ELEMENTS; e++)
	{
		if(graphInsertRight(i, newElements[baseIndex+e]) == NULL)
			errors ++;
	}
	if(errors != 0)
		printf("failed to insert %d/%d elements right from index\n",errors,NUM_OF_RIGHT_ELEMENTS);
	else
		printf("Success!\n");
	
	graphIndexMovRight(i);
	
	baseIndex += NUM_OF_RIGHT_ELEMENTS;
	
	
	printf("Insert %d left elements\n",NUM_OF_LEFT_ELEMENTS);
	errors = 0;
	for(e = 0; e < NUM_OF_LEFT_ELEMENTS; e++)
	{
		if(graphInsertLeft(i, newElements[baseIndex+e]) == NULL)
			errors ++;
	}
	if(errors != 0)
		printf("failed to insert %d/%d elements left from index\n",errors,NUM_OF_LEFT_ELEMENTS);
	else
		printf("Success!\n");
	
	graphIndexMovRight(i);
	
	baseIndex += NUM_OF_LEFT_ELEMENTS;
	
	
	printf("Insert %d upper elements\n",NUM_OF_UPPER_ELEMENTS);
	errors = 0;
	for(e = 0; e < NUM_OF_UPPER_ELEMENTS; e++)
	{
		if(graphInsertAbove(i, newElements[baseIndex+e]) == NULL)
			errors ++;
	}
	if(errors != 0)
		printf("failed to insert %d/%d elements above index\n",errors,NUM_OF_UPPER_ELEMENTS);
	else
		printf("Success!\n");
	
	baseIndex += NUM_OF_UPPER_ELEMENTS;
	
	printf("Insert %d lower elements\n",NUM_OF_LOWER_ELEMENTS);
	errors = 0;
	for(e = 0; e < NUM_OF_LOWER_ELEMENTS; e++)
	{
		if(graphInsertBelow(i, newElements[baseIndex+e]) == NULL)
			errors ++;
	}
	if(errors != 0)
		printf("failed to insert %d/%d elements below index\n",errors,NUM_OF_LOWER_ELEMENTS);
	else
		printf("Success!\n");
	
	baseIndex += NUM_OF_LOWER_ELEMENTS;
}