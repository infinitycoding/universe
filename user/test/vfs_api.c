/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of Universe.
 
     Universe is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     Universe is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with Universe. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <vfs_api.h>

/**
 * @brief A incomplete vfs api unit test
 * @return true if success, else failure
 */
bool test_vfs_api(void)
{
    bool success = true;
    char testfile[] = "testfile";
    printf("VFS-Runtime-API-Test:\n");

    printf("create file: ");
    bool res = test_create_file(testfile);
    success &= res;
    if(res)
        printf("success\n");
    else
        printf("failed\n");
    

    printf("read/write file: ");
    res = test_read_write_file(testfile);
    success &= res;
    if(res)
        printf("success\n");
    else
        printf("failed\n");


    printf("append file: ");
    res = test_apped_file(testfile);
    success &= res;
    if(res)
        printf("success\n");
    else
        printf("failed\n");


    return success;
}


/**
 * @brief Tests file creation
 * @return true if success, else failure
 */
bool test_create_file(const char *name)
{
    int f = 0;
    f = open(name,O_WRONLY,0);
    close(f);

    if(f)
        return true;
    return false;
}


/**
 * @brief Tests file reading and writing
 * @return true if success, else failure
 */
bool test_read_write_file(const char *name)
{
    char c[5] = {0,0,0,0,0};

    int f = 0;
    f = open(name, O_WRONLY, 0);
    if(!f)
    {
        printf("failed to open test file for writing! return code: %d",f);
        return false;
    }
    int n = write(f, "+++++", 5);
    if(n != 5)
    {
        printf("could not write all charaters. 5 expected, %d written", n);
        close(f);
        return false;
    }
    close(f);
    

    f = open("test",O_RDONLY,0);
    if(!f)
    {
        printf("failed to open test file for reading! return code: %d",f);
        return false;
    }
    n = read(f,c,5);
    if(n != 5)
    {
        printf("could not read all charaters. 5 expected, %d got", n);
        close(f);
        return false;
    }
    close(f);

    if(strncmp("+++++",c,5) == 0)
        return true;
    return false;
}


/**
 * @brief Tests file appending
 * @return true if success, else failure
 */
bool test_apped_file(const char *name)
{
    char c[10] = {0,0,0,0,0,0,0,0,0,0};

    int f = 0;
    f = open("test",O_APPEND|O_WRONLY,0);
    if(!f)
    {
        printf("failed to open test file for appending! return code: %d",f);
        return false;
    }
    int n = write(f, "+++++", 5);
    if(n != 5)
    {
        printf("could not append all charaters. 5 expected, %d written", n);
        close(f);
        return false;
    }
    close(f);


    f = open("test",O_RDONLY,0);
    if(!f)
    {
        printf("failed to open test file for reading! return code: %d",f);
        return false;
    }
    n = read(f,c,10);
    if(n != 10)
    {
        printf("could not read all charaters. 10 expected, %d got", n);
        close(f);
        return false;
    }
    close(f);

    if(strncmp("++++++++++",c,15) == 0)
        return true;
    return false;
}


