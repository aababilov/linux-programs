/* Copyright (c) 2005 by John M. Boyer, All Rights Reserved.
        Please see License.txt for use and redistribution license. */


#ifndef STACK_H
#define STACK_H

typedef struct
{
        int *S;
        int Top, Size;
} stack;

typedef stack * stackP;

stackP sp_New(int);
void sp_Free(stackP *);

int  sp_Copy(stackP, stackP);

#ifndef SPEED_MACROS

int  sp_ClearStack(stackP);

int  sp_IsEmpty(stackP);
int  sp_NonEmpty(stackP);

int  sp_Push(stackP, int);
int  sp_Push2(stackP, int, int);

#define sp_Pop(theStack, a) sp__Pop(theStack, &(a))
#define sp_Pop2(theStack, a, b) sp__Pop2(theStack, &(a), &(b))

int  sp__Pop(stackP, int *);
int  sp__Pop2(stackP, int *, int *);

#else

#define sp_ClearStack(theStack) theStack->Top=0

#define sp_IsEmpty(theStack) !theStack->Top
#define sp_NonEmpty(theStack) theStack->Top

#define sp_Push(theStack, a) theStack->S[theStack->Top++] = a
#define sp_Push2(theStack, a, b) sp_Push(theStack, a); sp_Push(theStack, b)

#define sp_Pop(theStack, a) a=theStack->S[--theStack->Top]
#define sp_Pop2(theStack, a, b) sp_Pop(theStack, b);sp_Pop(theStack, a)

#endif

#endif
