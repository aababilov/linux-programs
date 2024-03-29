/* Copyright (c) 2005 by John M. Boyer, All Rights Reserved.
        Please see License.txt for use and redistribution license. */

#include "appconst.h"
#include "stack.h"
#include <stdlib.h>

stackP sp_New(int Size)
{
stackP theStack;

     theStack = (stackP) malloc(sizeof(stack));
     
     if (theStack != NULL)
     {
         theStack->S = (int *) malloc(Size*sizeof(int));
         if (theStack->S == NULL)
         {
             free(theStack);
             theStack = NULL;
         }
     }
     
     if (theStack != NULL)
     { 
         theStack->Size = Size;
         sp_ClearStack(theStack);
     } 

     return theStack;
}

void sp_Free(stackP *pStack)
{
     if (pStack == NULL || *pStack == NULL) return;

     (*pStack)->Size = (*pStack)->Top = 0;

     if ((*pStack)->S != NULL)
          free((*pStack)->S);
     (*pStack)->S = NULL;
     free(*pStack);

     *pStack = NULL;
}

int  sp_Copy(stackP stackDst, stackP stackSrc)
{
stackP newStack = NULL;
int  I, *p;

     if (stackDst->Size == stackSrc->Size)
     {
         for (I=0; I < stackSrc->Top; I++)
              stackDst->S[I] = stackSrc->S[I];
     }

     else
     {
         newStack = sp_New(stackSrc->Size);
         if (newStack == NULL) return NOTOK;

         for (I=0; I < stackSrc->Top; I++)
              newStack->S[I] = stackSrc->S[I];

         p = stackDst->S;
         stackDst->S = newStack->S;
         newStack->S = p;
         newStack->Size = stackDst->Size;
         sp_Free(&newStack);
     }

     stackDst->Top = stackSrc->Top;
     stackDst->Size = stackSrc->Size;

     return OK;
}

#ifndef SPEED_MACROS

int  sp_ClearStack(stackP theStack)
{
     theStack->Top = 0;
     return OK;
}

int  sp_IsEmpty(stackP theStack)
{
     return !theStack->Top;
}

int  sp_NonEmpty(stackP theStack)
{
     return theStack->Top;
}

int  sp_Push(stackP theStack, int a)
{
//     if (theStack->Top >= theStack->Size)
//         return NOTOK;

     theStack->S[theStack->Top++] = a;
     return OK;
}

int  sp_Push2(stackP theStack, int a, int b)
{
//     if (theStack->Top + 1 >= theStack->Size)
//         return NOTOK;
     
     theStack->S[theStack->Top++] = a;
     theStack->S[theStack->Top++] = b;
     return OK;
}

int  sp__Pop(stackP theStack, int *pA)
{
//     if (theStack->Top <= 0)
//         return NOTOK;

     *pA = theStack->S[--theStack->Top];
     return OK;
}

int  sp__Pop2(stackP theStack, int *pA, int *pB)
{
//     if (theStack->Top <= 1)
//         return NOTOK;

     *pB = theStack->S[--theStack->Top];
     *pA = theStack->S[--theStack->Top];

     return OK;
}

#endif // SPEED_MACROS

