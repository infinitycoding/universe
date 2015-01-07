#ifndef MEMCHECK_H
#define MEMCHECK_H
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
 *  @author cedi <c_edi@gmx.de>
 */


#pragma once


static int g_nMallocCnt  = 0;
static int g_nReallocCnt = 0;
static int g_nCallocCnt  = 0;
static int g_nFreeCnt    = 0;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* Secure malloc */
#define malloc( size )                                                      \
        malloc( size );                                                     \
        printf( "malloc in Zeile %d, Datei %s \nBytes: %d ) \n"             \
               , __LINE__                                                   \
               , __FILE__                                                   \
               , size                                                       \
              );                                                            \
        g_nMallocCnt++;                                                     \


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* Secure realloc */
#define realloc( ptr, size )                                                \
        realloc( ptr, size );                                               \
        printf( "realloc in Zeile %d, Datei %s \nCount: %d\nBytes: %d ) \n" \
               , __LINE__                                                   \
               , __FILE__                                                   \
               , size                                                       \
              );                                                            \
        g_nReallocCnt++;                                                    \


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* Secure calloc */
#define calloc( num, size )                                                \
        calloc( num, size );                                               \
        printf( "calloc in Zeile %d, Datei %s \nCount: %d\nBytes: %d ) \n" \
               , __LINE__                                                  \
               , __FILE__                                                  \
               , size                                                      \
              );                                                           \
        g_nCallocCnt++;                                                    \


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* Secure free*/
#define free( x )                                                          \
        free( x );                                                         \
        printf( "free in Zeile %d, Datei %s\n"                             \
              , __LINE__                                                   \
              ,  __FILE__                                                  \
              );                                                           \
    g_nFreeCnt++;


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* Secure free*/
#define return EXIT_SUCCESS;                                               \
        printf( "Count malloc    : %d   \n", g_nMallocCnt );               \
        printf( "Count realloc   : %d   \n", g_nReallocCnt );              \
        printf( "Count calloc    : %d   \n", g_nCallocCnt );               \
        printf( "Anzahl free     : %d   \n", g_nFreeCnt );                 \
        printf( "-----------------------\n" );                             \
        printf( "Vergessene free: %d    \n"                                \
              , ( g_nMallocCnt + g_nReallocCnt + g_nCallocCnt )            \
                - g_nFreeCnt                                               \
              );                                                           \
        printf( "\n\n" );                                                  \
        return EXIT_SUCCESS;


#endif	/* MEMCHECK_H */

