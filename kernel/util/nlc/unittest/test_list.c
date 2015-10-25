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
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 **/

#include <list.h>
#include <printf.h>
#include <unittest/test_list.h>





#define CREATE_TEST_NUMBER 100
#define FILL_TEST_NUMBER 10
#define SIZE_TEST_NUMBER 100



/**
 * @brief tests the create/destroy functions of the list module
 * @return 0 if success
 * @return 1 if failture
 */
int test_list_create_destroy()
{
    list_t *firstList = NULL;

    if((firstList = list_create()) == NULL)
    {
        printf("create/delete one list: failture\n");
        return 1;
    }

    list_destroy(firstList);

    list_t *nextLists[CREATE_TEST_NUMBER];
    int i;

    for(i = 0; i < CREATE_TEST_NUMBER; i++)
    {
        if((nextLists[i] = list_create()) == NULL)
        {
            printf("create/delete %d lists: failture\n", CREATE_TEST_NUMBER);
            return 1;
        }
    }

    for(i = 0; i < CREATE_TEST_NUMBER; i++)
        list_destroy(nextLists[i]);

    return 0;
}


/**
 * @brief tests the push/pop functions of the list module
 * @return 0 if success
 * @return 1 if failture
 */
int test_list_push_pop()
{
    list_t *testList = list_create();

    int a = 12345;
    int b = 98765;
    int c = 54321;
    int d = 56789;
    int *resa, *resb, *resc, *resd;

    list_push_front(testList, &b);
    list_push_front(testList, &a);
    list_push_back(testList, &c);
    list_push_back(testList, &d);

    resa = list_pop_front(testList);
    resb = list_pop_front(testList);
    resd = list_pop_back(testList);
    resc = list_pop_back(testList);

    if(*resa != a || *resb != b || *resc != c || *resd != d)
    {
        printf("list push and pop front and back: failture\n");
        return 1;
    }

    list_destroy(testList);

    return 0;
}


/**
 * @brief tests the is_empty function of the list module
 * @return 0 if success
 * @return 1 if failture
 */
int test_list_is_empty()
{
    list_t *testList = list_create();

    if(!list_is_empty(testList))
    {
        printf("list is empty: failture\n");
        return 1;
    }

    int i;

    for(i = 0; i < FILL_TEST_NUMBER; i++)
        list_push_front(testList, testList);

    for(i = 0; i < FILL_TEST_NUMBER; i++)
        list_pop_front(testList);

    if(!list_is_empty(testList))
    {
        printf("list is empty: failture\n");
        return 1;
    }

    list_destroy(testList);

    return 0;
}


/**
* @brief tests the length function of the list module
* @return 0 if success
* @return 1 if failture
*/
int test_list_length()
{
    list_t *testList = list_create();

    int i;

    for(i = 0; i < SIZE_TEST_NUMBER; i++)
        list_push_front(testList, testList);

    if(list_length(testList) != SIZE_TEST_NUMBER)
    {
        printf("list length: failture\n");
        return 1;
    }

    list_destroy(testList);

    return 0;
}


/**
 * @brief tests the list module
 * @return 0 if success
 * @return 1 if failture
 */
int test_list()
{
    if(test_list_create_destroy())
        return 1;

    if(test_list_push_pop())
        return 1;

    if(test_list_is_empty())
        return 1;

    if(test_list_length())
        return 1;

    printf("list test: all successful\n");

    return 0;
}