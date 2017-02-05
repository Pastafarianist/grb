/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"

/* ********************************************************************** */
/* ********************************************************************** */
#ifdef NOPROTO
int insertPath(pp,root)
PATH *pp, **root;
#else
int insertPath(PATH *pp,PATH **root)
#endif
/*
  inserts path in tree.  If 1 returned, the path divides one of the 
  paths in the tree.  If 2 returned, a path in the tree divides the path.
  Otherwise 0 is returned.
*/
{
  PATH *tem,**tempP;
  int i,notDone = 1;

  IF *root == NULL THEN *root = pp; return(0); ENDIF

  tempP = root;

  while(notDone)
  {
    i = comparePaths(pp,*tempP);

    if(i == 0)
    IF pp -> length > (*tempP) -> length
    THEN
      return(2); 
    ELSE
      return(1);
    ENDIF

    IF i == 1 
    THEN
      IF (*tempP) -> lchild != NULL
      THEN
	tempP = &(*tempP) -> lchild;
      ELSE
	(*tempP) -> lchild = pp;
	return(0); 
      ENDIF
    ELSE
      IF (*tempP) -> rchild != NULL
      THEN
	tempP = &(*tempP) -> rchild;
      ELSE
	(*tempP) -> rchild = pp;
	return(0); 
      ENDIF
    ENDIF

  }

  return(0);
}

/* ********************************************************************** */

#ifdef NOPROTO
static int install(tem,pp)
PATH *tem, *pp;
#else
static int install(PATH *tem,PATH *pp)
#endif
/*
   installs subtree with root tem in subtree with root pp.
*/
{
   int i;

   IF tem == NULL THEN return(0); ENDIF

   IF tem -> rchild != NULL 
   THEN 
     IF (i = install(tem -> rchild,pp)) != 0 THEN return(i); ENDIF
     tem -> rchild = NULL;
   ENDIF

   IF tem -> lchild != NULL 
   THEN 
     IF (i = install(tem -> lchild,pp)) != 0 THEN return(i); ENDIF
     tem -> lchild = NULL;
   ENDIF

   return(insertPath(tem,&pp));
}

/* ********************************************************************** */

#ifdef NOPROTO
int deletePath(pp,top)
PATH *pp, **top;
#else
int deletePath(PATH *pp, PATH **top)
#endif
/*
  removes the path pp from the tree of paths pointed to by *top
  and inserts all nodes in the subtrees of pointed to by pp in
  the tree pointed to by *top.

  returns 0 if no problems arise.
  returns 1 if tree does not obey required laws.
  returns 2 if pp not found in tree.
*/
{
  PATH **tempP;
  int i,done = 0;

/* find path in tree. */
   tempP = top;

   while(!done && *tempP != NULL)
   {
     IF *tempP != pp
     THEN
       IF (i = comparePaths(pp,*tempP)) != 0
       THEN
	 IF i == 1 
	 THEN
	   tempP = &(*tempP) -> lchild;
	 ELSE
	   tempP = &(*tempP) -> rchild;
	 ENDIF
       ELSE
	 IF pp -> length == (*tempP) -> length
	 THEN
	   break;
	 ELSE
	   return(1);
	 ENDIF
       ENDIF
     ELSE
       done = 1;
     ENDIF
   }

   IF *tempP == NULL THEN return(2); ENDIF

   IF pp -> lchild != NULL
   THEN
     *tempP = pp -> lchild;
     return(install(pp -> rchild,*tempP));
   ELSE
     *tempP = pp -> rchild;
   ENDIF

   return(0);
}

/* ********************************************************************** */
#ifdef NOPROTO
int seekSubstring(ss,lengthS,root,pp)
gaget *ss;
unsigned lengthS;
PATH *root, **pp;
#else
int seekSubstring(gaget *ss,unsigned lengthS,PATH *root,PATH **pp)
#endif
/*
   seeks a path having edges which are an initial substring of ss
   in the tree at root.
   Returns 0 if such a path is found and 1 otherwise.
   If a path is found its address is returned as *pp.
*/
{
   int i;

   while(root != NULL)
   {
      IF (i = compareSubstring(ss,lengthS,root)) != 0 
      THEN
	IF i==1 THEN root = root->lchild; ELSE root = root->rchild; ENDIF
      ELSE
	IF lengthS >= root -> length
	THEN
	  *pp = root;
	  return(1);
	ELSE
	  return(0);
	ENDIF
      ENDIF
   }

  return(0);
}

/* ********************************************************************** */
#ifdef NOPROTO
int tipSearch(g1,length1,result,projres)
gaget *g1;
int length1,projres;
PATH **result;
#else
int tipSearch(gaget *g1,int length1,PATH **result,int projres)
#endif
/*
  determines if one of the tips in the tree of tips pointed at
  by *result divides g1.  If so, the position of the first edge in
  the dividing path in p1 for this division is returned and a pointer to
  the dividing path is returned in result.  Otherwise -1 is returned.
*/
{
   int i = 0;
   gaget *gPtrBd;

   if(projres)
   if( seekSubstring(g1,(unsigned) (length1),
					PRrootPath,result)) return(0);

   gPtrBd = g1 + length1;

   while(g1 < gPtrBd)
   {
     if(seekSubstring(g1++,(unsigned)(length1 - i),
				 rootPath,result))return(i);
     ++i;
   }
   
   return(-1);
}

/* ********************************************************************** */
#ifdef NOPROTO
void checkTree(r1,root)
RELATION *r1;
PATH **root;
#else
void checkTree(RELATION *r1,PATH **root)
#endif
{
   RELATION *r2;
   RELMEM *rm2;
   gaget *g1,*g2;
   unsigned length1;

   g1 = r1 -> mem[0] -> edgelist;
   length1 = r1 -> mem[0] -> pathlength;


   r2 = firstR;

   while(r2 != NULL)
   {
      IF r2 -> pmt == 1
      THEN
	rm2 = r2 -> mem[0];

	IF substring(g1,rm2 -> edgelist,(int) length1,
				   (int) rm2 -> pathlength) != -1
	THEN
	  IF deletePath(r2 -> tipPtr, root)
	  THEN
	    PRINTF(" Problems in deletePath.\n");
	    return;
	  ENDIF

	  r2 -> pmt = rMark;
	  r2 -> tipPtr = NULL;
	ENDIF
      ENDIF

      r2 = r2 -> next;
   }

   return;
}
/* ********************************************************************** */

