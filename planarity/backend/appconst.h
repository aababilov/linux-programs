#ifndef APPCONST_H
#define APPCONST_H

/* Copyright (c) 2005 by John M. Boyer, All Rights Reserved.
        Please see License.txt for use and redistribution license. */

//#define PROFILE
#ifdef PROFILE
#include "platformTime.h"
#endif

/* If the DEBUG macro is not defined, then low-level functions are replaced by faster macros */

#ifndef DEBUG
#define SPEED_MACROS
#endif

/* Return status values */

#define OK              0
#define NOTOK           -2
#define NONPLANAR       -3
#define NONOUTERPLANAR	-4

/* Array indices are used as pointers, and this means bad pointer */

#define NIL		-1
#define NIL_CHAR	0xFF

/* Defines fopen strings for reading and writing text files on PC and UNIX */

#ifdef WINDOWS
#define READTEXT        "rt"
#define WRITETEXT       "at"
#else
#define READTEXT        "r"
#define WRITETEXT       "a"
#endif

/* This macro controls whether DFS puts all child edges at the beginning of
    the adjacency list (link[0]) and all forward edges at the end of the
    adjacency list (link[1]).  This allows several subsequent methods to 
    find the desired edges more easily and to terminate without doing 
    as much unnecessary traversal of adjacency lists */

#define ORDER_EDGES

#endif
