/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"


/* ************************************************************** */
#ifdef NOPROTO
void Reorder(tr)
TEMPREL *tr;
#else
void Reorder(TEMPREL *tr)
#endif
/*
   insures that paths in relation members are stored in decreasing order.
*/
{
  RELATION *r;
  RELMEM rm,*rm1,*rm2;
  int done,val,changed;
  unsigned bound,index,index1;

  r = &tr->rel;

  index = 0;
  done = (bound = r -> length) == 0;

  while(!done)
  {
    IF !((rm1 = r -> mem[index>>SHIFT] + (index&MASK))->coef)
    THEN
      rm = *rm1;
      index1 = index;

      while(index1 < r->length - 1)
      {
	r -> mem[(index1)>>SHIFT][(index1)&MASK] =
		r->mem[(index1+1)>>SHIFT][(index1+1)&MASK];
	++index1;
      }

      r -> mem[(index1)>>SHIFT][(index1)&MASK] = rm;
      --r->length;
      done = index >= --bound;
      continue;
    ENDIF

    done = (++index >= bound);
  }


  IF r -> length < 2 THEN return; ENDIF

  bound = r -> length - 1;

  while(bound > 0)
  {
    done = changed = index = 0;

    while(!done)
    {
       rm1 = &r -> mem[index>>SHIFT][index&MASK];
       rm2 = &r -> mem[(index+1)>>SHIFT][(index+1)&MASK];


       IF (val = rmLessThan(rm1,rm2)) == 1
       THEN
	 rm = *rm1;
	 *rm1 = *rm2;
	 *rm2 = rm;
	 changed = 1;
       ELSE
	 IF val == 0
	 THEN
	   rm1 -> coef = pAdd(rm1->coef,rm2 -> coef);
	   rm = *rm2;
	   index1 = index + 1;

	   while(index1 < r->length - 1)
	   {
	     r -> mem[(index1)>>SHIFT][(index1)&MASK] =
		     r->mem[(index1+1)>>SHIFT][(index1+1)&MASK];
	     ++index1;
	   }

	   r -> mem[(index1)>>SHIFT][(index1)&MASK] = rm;
	   --r->length;
	   done = index >= --bound;
	   continue;
	 ENDIF
       ENDIF

       done = (++index >= bound);
    }

    if(!changed) break;

    --bound;
	  } 

  return;
}

/* ************************************************************** */

#ifdef NOPROTO
void LeadOne(tr)
TEMPREL *tr;
#else
void LeadOne(TEMPREL *tr)
#endif
{
   unsigned int i,length;
   int coef;
   RELATION *r;

   r = &tr->rel;

   restart:

   if((length = r->length) == 0) return;

   IF (coef = r -> mem[0][0].coef) != 0
   THEN
     coef = pInverse(coef);
     r -> mem[0][0].coef = 1;
   ELSE
     Reorder(tr);
     goto restart;
   ENDIF

   for(i=1;i < length; ++i)
   {
      r -> mem[i>>SHIFT][i&MASK].coef =
	  pMult(coef,r -> mem[i>>SHIFT][i&MASK].coef);
   }

   return;
}

/* ************************************************************** */

#ifdef NOPROTO
static copyEdgelist(g1,g2, length)
gaget *g1, *g2;
unsigned length;
#else
static void copyEdgelist(gaget *g1,gaget *g2, unsigned length)
#endif
/*
  copies edgelist g1 into edgelist g2.
*/
{
  gaget *g1end;

  g1end = g1 + length;

  while(g1 < g1end) *g2++ = *g1++;

  return;
}


/* ************************************************************** */

#ifdef NOPROTO
int addRels(r1,r2,tr,coef)
RELATION *r1, *r2;
TEMPREL *tr;
int coef;
#else
int addRels(RELATION *r1,RELATION *r2,TEMPREL *tr,int coef)
#endif
/*
   returns 0 if no error else 1.
   coef * r1 is added to r2.  The result is in tr->rel.
*/
{
  RELATION *r3;
  RELMEM *RM1,*RM2,*RM3;
  int i,j,done,coef1;
  unsigned ind1,ind2,length1,length2,length3,index;
  char ch;


  IF r1 == NULL || !r1->length || r2 == NULL || !r2->length 
  THEN
    IF((r1==NULL||!r1->length) && (r2 == NULL||!r2->length))
    THEN
       tr->rel.length = 0; return(0); 
    ENDIF

    if(r1==NULL||!r1->length) return(copyTEMPREL(r2,tr));
    if(!coef){tr->rel.length = 0; return(0);}

    IF copyTEMPREL(r1,tr) THEN return(1); ENDIF
    if(coef == 1) return(0);
    index = 0;
    while(index < tr->rel.length)
    {
      coef1 = tr->rel.mem[index>>SHIFT][index&MASK].coef;
      tr->rel.mem[index>>SHIFT][index&MASK].coef = pMult(coef,coef1);
      ++index;
    }
    return(0);
  ENDIF

  IF (length3 = (length1 = r1->length) + (length2 = r2->length)) >
					   MAXRMLST * tr->RMgroups
  THEN
    IF expandTempRel(tr,length3) THEN return(1); ENDIF
  ENDIF

  (r3 = &tr -> rel) -> pmt = 1;
  length3 = r3 -> length = ind1 = ind2 = done = 0;

  RM1 = r1 -> mem[0];
  RM2 = r2 -> mem[0];

  /* RM2 and RM1 are compared.  If the paths are different, the
     one having the larger path is added to r3.  If they are
     the same, the non zero difference is added to r3. */

  done = ind1 >= length1 || ind2 >= length2;

  while (!done)
  {
    IF (i = rmLessThan(RM1,RM2)) == 0
    THEN
      IF (coef1 = pAdd(RM2 -> coef,pMult(coef,RM1->coef))) != 0
      THEN
	RM3 = r3 -> mem[length3>>SHIFT] + (length3&MASK);
	copyEdgelist(RM2->edgelist,RM3->edgelist,
				   (RM3->pathlength=RM2->pathlength));
	RM3 -> init = RM2 -> init;
	RM3 -> term = RM2 -> term;
	RM3 -> coef = coef1;
	++length3;
      ENDIF

      if(++ind1 < length1) RM1 = r1 -> mem[ind1>>SHIFT] + (ind1&MASK);
      if(++ind2 < length2) RM2 = r2 -> mem[ind2>>SHIFT] + (ind2&MASK);
    ELSE
      IF i == 2
      THEN
	RM3 = r3 -> mem[length3>>SHIFT] + (length3&MASK);
	copyEdgelist(RM1->edgelist,RM3->edgelist,
				   (RM3->pathlength=RM1->pathlength));
	RM3 -> init = RM1 -> init;
	RM3 -> term = RM1 -> term;
	RM3 -> coef = pMult(coef,RM1 -> coef);
	++length3;++ind1;
	if(ind1 < length1) RM1 = r1 -> mem[ind1>>SHIFT] + (ind1&MASK);
      ELSE                    /* i = 2 */
	RM3 = r3 -> mem[length3>>SHIFT] + (length3&MASK);
	copyEdgelist(RM2->edgelist,RM3->edgelist,
				   RM3->pathlength=RM2->pathlength);
	RM3 -> init = RM2 -> init;
	RM3 -> term = RM2 -> term;
	RM3 -> coef = RM2 -> coef;
	++length3; ++ind2;
	if(ind2 < length2) RM2 = r2 -> mem[ind2>>SHIFT] + (ind2&MASK);
      ENDIF
    ENDIF

    done = ind1 >= length1 || ind2 >= length2;
  }

/* the remainder of r1 is added to the result */

    while (ind1 < length1)
    {
	RM3 = r3 -> mem[length3>>SHIFT] + (length3&MASK);
	copyEdgelist(RM1->edgelist,RM3->edgelist,
				   (RM3->pathlength=RM1->pathlength));
	RM3 -> init = RM1 -> init;
	RM3 -> term = RM1 -> term;
	RM3 -> coef = pMult(coef,RM1 -> coef);
	++length3; ++ind1;
	if(ind1 < length1) RM1 = r1 -> mem[ind1>>SHIFT] + (ind1&MASK);
     }


/* the remainder of r2 is added to the result */

    while (ind2 < length2)
    {
      RM3 = r3 -> mem[length3>>SHIFT] + (length3&MASK);
      copyEdgelist(RM2->edgelist,RM3->edgelist,
				 RM3->pathlength=RM2->pathlength);
      RM3 -> init = RM2 -> init;
      RM3 -> term = RM2 -> term;
      RM3 -> coef = RM2 -> coef;
      ++length3; ++ind2;
      if(ind2 < length2) RM2 = r2 -> mem[ind2>>SHIFT] + (ind2&MASK);
    }

    r3 -> length = length3;
    return(0);
  }

/* ****************************************************************** */

#ifdef NOPROTO
static int check(list1)
gaget *list1;
#else
static int check(gaget *list1)
#endif

{
  gaget *gPtr;

  gPtr = list1 + *list1;

  while(++list1 < gPtr)
  {
    IF EdgeList[*list1].term != EdgeList[*(list1+1)].init
    THEN
      return(1);
    ENDIF
  }
  return(0);
}

/* ************************************************************** */

#ifdef NOPROTO
int PRPmult(R1,tr,pre1,post1)
RELATION *R1;
TEMPREL *tr;
gaget *pre1, *post1;
#else
int PRPmult(RELATION *R1,TEMPREL *tr,gaget *pre1,gaget *post1)
#endif
/*
   premultiplies R1 by the string in pre1 of length pre1[0]
   and postmultiples the result by the path in post1 of length
   post1[0].  The result is in tr -> rel.  &tr -> rel must not
   be the same as R1.  Paths with endpoints not matching pre1 or
   post1 are discarded. Returns 1 in case of error and 0 otherwise.
*/
  {
   int i;
   unsigned length,ind,index;
   gaget *gPtr1,*gPtr2,*gPtrBd;
   RELMEM *RM1, *RM2;
   RELATION *R2;

   R2 = &tr->rel;

   IF R1 == NULL
   THEN
     R2 -> length = 0;
     return(0);
   ENDIF

   if(homogeneous || NoFallBack)
   IF (int) R1->mem[0]->pathlength + pre1[0] + post1[0] > (int) maxlength
   THEN
     R2 -> length = 0;
     return(0);
   ENDIF

   IF check(pre1) THEN PRINTF(" illegal pre1 in PRPmult\n"); return(1);ENDIF

   IF check(post1)
   THEN
     PRINTF(" illegal post1 in PRPmult\n"); return(1);
   ENDIF

   R2  -> pmt = 1;

   if ((length = R1 -> length) > MAXRMLST * tr -> RMgroups)
     IF expandTempRel(tr,length) THEN return(1); ENDIF

   for(index = ind = 0; ind < length; ++ind)
   {
      RM1 = R1 -> mem[ind>>SHIFT] + (ind&MASK);

      if ((i= pre1[0] + post1[0] + (int) RM1->pathlength) > (int) maxlength)
	continue;

      if (pre1[0] && RM1->init != EdgeList[pre1[pre1[0]]].term) continue;
      if (post1[0] && RM1->term != EdgeList[post1[1]].init) continue;

      RM2 = R2 -> mem[index>>SHIFT] + (index&MASK);
      ++index;

      RM2 -> pathlength = (unsigned) i;
      RM2 -> coef = RM1 -> coef;

      gPtr2 = RM2 -> edgelist;

      IF pre1[0]
      THEN
	gPtrBd = (gPtr1 = pre1 + 1) + pre1[0];
	RM2 -> init = EdgeList[*gPtr1].init;
	while(gPtr1 < gPtrBd) *gPtr2++ = *gPtr1++;
      ELSE
	RM2 -> init = RM1 -> init;
      ENDIF

      gPtrBd = (gPtr1 = RM1 -> edgelist) + RM1 -> pathlength;

      while(gPtr1 < gPtrBd) *gPtr2++ = *gPtr1++;

      IF post1[0]
      THEN
	gPtrBd = (gPtr1 = post1 + 1) + post1[0];
	RM2 -> term = EdgeList[*(gPtrBd-1)].term;
	while(gPtr1 < gPtrBd) *gPtr2++ = *gPtr1++;
      ELSE
	RM2 -> term = RM1 -> term;
      ENDIF
   }

   R2 -> length = index;

   return(0);
  }
/* ****************************************************************** */

#ifdef NOPROTO
int substring(g1,g2,length1,length2)
gaget *g1,*g2;
int length1,length2;
#else
int substring(gaget *g1,gaget *g2,int length1,int length2)
#endif

/*
   returns -1 if g1 is not a substring of g2, otherwise
   returns the index of the starting point of g1 in g2.
   length1 must be less than length2.
*/
{
  int i1,i2;
  gaget *gPtr1,*gPtr2,*gPtrBd;

  for (i2 = 0; i2 <= length2 - length1; i2++)
  {
    gPtrBd =(gPtr1 = g1) + length1; gPtr2 = g2 + i2;

    while (gPtr1 < gPtrBd)
      if (*gPtr1++ != *gPtr2++) goto next;

    return(i2);

    next: doNothing;
  }

  return(-1);
}

/* ****************************************************************** */
#ifdef NOPROTO
int overlap(g1,g2,length1,length2,BigLength)
gaget *g1,*g2;
int length1,length2,BigLength;
#else
int overlap(gaget *g1,gaget *g2,int length1,int length2,int BigLength)
#endif

/*
   returns the number of proper overlaps of g1 with g2.  The maximum
   length of the overlapped path is BigLength.
   The global "list" contains the positions of the last edge of g1 in g2.
*/

{
  int i,iubd,ilbd,num = 0;
  gaget *gPtr1,*gPtr2,*gPtrBd;

  iubd = length1 < length2 ? length1:length2;
  ilbd = length1 + length2 - BigLength;
  ilbd = 1 > ilbd ? 1 : ilbd;

  for (i = ilbd; i < iubd;++i)
  {
    gPtr1 = g1 + length1 - i;
    gPtrBd = (gPtr2 = g2) + i;

    while (gPtr2 < gPtrBd) if(*gPtr1++ != *gPtr2++) goto contin;

    list[num++] = i-1;

    contin: doNothing;
  }

  return(num);
}
/* *************************************************************  */
#ifdef NOPROTO
static int prep(i,plength,g1,tr,result,pre1,post1)
int i,plength;
gaget *g1,*pre1,*post1;
TEMPREL *tr;
PATH *result;
#else
static int prep(int i,int plength,gaget *g1,TEMPREL *tr,PATH *result,
			gaget *pre1, gaget *post1)
#endif
/*
  The path in result is a substring of gr beginning at position i 
  in gaget.  plength is the length of g1.

  The relation in tr is the result of pre1 and post1 multiplying the
  relation whose tip is the path result by the appropriate parts of g1.

  Returns 0 if no error, else returns 1.
*/
{
  RELATION *r1;
  gaget *g2Ptr,*g1Ptr,*gPtrBd;

  r1 = result -> relptr;

  IF (pre1[0] = i) != 0
  THEN
    g1Ptr = g1; gPtrBd = g1 + i; g2Ptr = pre1 + 1;
    while(g1Ptr < gPtrBd) *g2Ptr++ = *g1Ptr++;
  ENDIF

  IF (post1[0] = plength - i - (int) result -> length) > 0
  THEN
    g1Ptr = g1 + plength - post1[0];
    gPtrBd = g1 + plength; g2Ptr = post1 + 1;
    while(g1Ptr < gPtrBd) *g2Ptr++ = *g1Ptr++;
  ENDIF

  IF PRPmult(r1,tr,pre1,post1) THEN return(1); ENDIF

  return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
static int sBackReduce(r,tr,trs,projres)
RELATION *r;
TEMPREL *tr,*trs;
int projres;
#else
static int sBackReduce(RELATION *r,TEMPREL *tr,TEMPREL *trs,int projres)
#endif
/*
  Back reduces the relation r and stores the result in tr
  if a reduction is possible.  trs is used for temporary storage.
  Returns 0 if there were no reductions, 1 in case of a storage problem,
  and 2 if a reduction has occured.
*/
{
  int i,plength;
  gaget *g1;
  PATH *result;
  RELMEM *rm;

  if(r == NULL || r -> length == 0) return(0);

  g1 = (rm = r -> mem[0]) -> edgelist;
  plength = (int) rm -> pathlength;

  if ((i = tipSearch(g1,plength,&result,projres)) == -1) return(0);

  IF prep(i,plength,g1,trs,result,pre,post) THEN return(1); ENDIF
/*
  printf("\n");
  fShowRelation(stdout,r,1);
  fShowRelation(stdout,&trs->rel,1);
*/
  IF addRels(&trs->rel,r,tr,
	 -pMult(r->mem[0]->coef,pInverse(trs->rel.mem[0]->coef)))
  THEN
    return(1);
  ENDIF
/*
  fShowRelation(stdout,&tr->rel,1);
*/
  return(2);
}
/* ************************************************************* */
#ifdef NOPROTO
int BackReduce(r,projres)
RELATION *r;
int projres;
#else
int BackReduce(RELATION *r,int projres)
#endif
/*
  It is assumed that tr1,tr2,tr3, and trtemp are not being used.
  root points at the tree of paths that are to be used in the reduction.
  if no reductions occur, then 0 is returned.
  Otherwise 2 is returned and the reduction of r is returned in tr1->rel.
*/
{
  int i=2,num = 0;
  RELATION *r1;

  r1 = r;

  while(i)
  {
    IF (i = sBackReduce(r1,tr3,tr2,projres)) == 1 THEN return(1); ENDIF;
    IF i == 2
    THEN
      num = 2;
      trtemp = tr1;
      tr1 = tr3;
      tr3 = trtemp;
      r1 = &tr1 -> rel;
    ENDIF
  }

  return(num);
}

/* ************************************************************* */
#ifdef NOPROTO
int reduce(r,index,projres)
RELATION *r;
unsigned index;
int projres;
#else
int reduce(RELATION *r,unsigned index,int projres)
#endif
/*
  The result of the reduction is returned in tr1 if any reductions are made.
  It is assumed that tr1, tr2, and tr3 are not in use.  Returns 1 if there
  are memory problems, 2 if some reductions are made, and 0 otherwise.
  If index is 0, the tip will be reduced. Otherwise the index th RELMEM
  is the first RELMEM that is reduced.

  If projres = 1, reduction by elements using elements from 
  PRrootPath are also used.
*/
{
   int plength,done,num = 0,i;
   RELATION *r1 = r;
   RELMEM *rm;
   PATH *result;
   gaget *g1;

   done = (r1 == NULL) || (index >= r1 -> length);

   while(!done)
   { 
      g1 = (rm = r1 -> mem[index>>SHIFT] + (index&MASK)) -> edgelist;
      plength = (int) rm -> pathlength;

      IF (i = tipSearch(g1,plength,&result,projres)) != -1 
      THEN
	num = 2;

	IF prep(i,plength,g1,tr3,result,pre,post) THEN return(1); ENDIF

	IF addRels(&tr3->rel,r1,tr2,
	       -pMult(rm->coef,pInverse(tr3->rel.mem[0]->coef)))
	THEN
	  return(1);
	ENDIF

	trtemp = tr2;
	tr2 = tr1;
	tr1 = trtemp;

	done = index >= (r1 = &tr1 -> rel) -> length;
	continue;
      ENDIF

      done = ++index >= r1 -> length;
   }

   return(num);
}
/* ************************************************************* */

#ifdef NOPROTO
static int ProcessR(r1,root,projres,pre1,post1)
RELATION *r1;
PATH **root;
int projres;
gaget *pre1,*post1;
#else
static int ProcessR(RELATION *r1,PATH **root,int projres,
			   gaget *pre1, gaget *post1)
#endif
/* 
  pre and post may not be in use.
*/
{
  RELATION *tempR;
  int i,num = 0;

  IF PRPmult(r1,tr0,pre1,post1) == 1 THEN return(1); ENDIF

  IF tr0 -> rel.length
  THEN
    IF (i = BackReduce(&tr0->rel,projres)) == 1 THEN return(1); ENDIF
    trtemp = i != 0 ? tr1 : tr0;

    if((tempR = &trtemp->rel) ->length)
    {
      num = 2;
      LeadOne(trtemp);
      checkTree(tempR,root);
      tempR->creationTime = rtime;
      tempR->pmt=1;

      IF storeRel(trtemp,root) THEN return(1); ENDIF
      IF enterTip(root) THEN return(1); ENDIF
    }
  ENDIF

  return(num);
}
/* *************************************************************  */
#ifdef NOPROTO
static int extendL(r1,i1,root,projres)
RELATION *r1;
int i1,projres;
PATH **root;
#else
static int extendL(RELATION *r1, int i1,PATH **root,int projres)
#endif
{
  struct MemAdjLst *mal1;

  int num=0,i;

  if(i1 == 0)
  {
    IF (i = ProcessR(r1,root,projres,extpre,extpost)) == 1 
    THEN 
      return(1); 
    ENDIF

    if(i) return(2);
    return(0);
  }

  if(projres) return(0);

  IF extpre[0] == i1
  THEN
    mal1 = rAdjLst[r1 -> mem[0] -> init];
  ELSE
    mal1 = rAdjLst[EdgeList[extpre[i1+1]].init];
  ENDIF


  while (mal1 != NULL)
  {
    extpre[i1] = mal1 -> EdgeNum;
    IF (i = extendL(r1,i1-1,root,projres)) == 1 THEN return(1); ENDIF
    if(i) num = 2;

    mal1 = mal1 -> Next;
  }

  return(num);
}

/* *************************************************************  */
#ifdef NOPROTO
static int extendR(r1,i1,root,projres)
RELATION *r1;
int i1,projres;
PATH **root;
#else
static int extendR(RELATION *r1, int i1,PATH **root,int projres)
#endif

{
  struct MemAdjLst *mal1;

  int num=0,i;

  extpre[0] = i1;
  IF (i = extendL(r1,i1,root,projres)) == 1 THEN return(1); ENDIF
  if(i) num = 2;

  IF i1 > 0     /* add all possible terminal edges */
  THEN
    IF extpost[0] == 0
    THEN
      mal1 = AdjLst[r1 -> mem[0] -> term];
    ELSE
      mal1 = AdjLst[EdgeList[extpost[extpost[0]]].term];
    ENDIF

    ++extpost[0];

    while (mal1 != NULL)
    {
      extpost[extpost[0]] = mal1 -> EdgeNum;
      IF (i = extendR(r1,i1-1,root,projres)) == 1 THEN return(1); ENDIF
      if(i) num = 2;

      mal1 = mal1 -> Next;
    }

    --extpost[0];
  ENDIF

return(num);
}

/* *************************************************************  */
#ifdef NOPROTO
int expand(r1,root,projres)
RELATION *r1;
PATH **root;
int projres;
#else
int expand(RELATION *r1,PATH **root,int projres)
#endif
/*
  If two paths in the relation have different lengths,
  new relations are formed by multiplying by all possible pre and
  post strings.  The resulting relations are entered.
  returns 1 if there are memory problems, 0 if no new relation
  is produced, and 2 otherwise.
*/
{
  RELMEM   *rm1;

  int MinExt,length1,i,j=0;
  unsigned index=1;

  length1 = (int) r1 -> mem[0] -> pathlength;
  extpost[0] = 0;

  while (index < r1 -> length)
  {
    rm1 = r1 -> mem[index>>SHIFT] + (index&MASK);

    IF length1 > (int) rm1 -> pathlength
    THEN
      MinExt = (int) maxlength + 1 - length1;

      IF (j = extendR(r1,MinExt,root,projres)) == 1
      THEN
	PRINTF(" Problems in expand.\n ");
	return(1);
      ENDIF

      if (j == 2) break;
    ENDIF

    ++index;
  }

  return(j);
}
/* *************************************************************  */
#ifdef NOPROTO
int copyTEMPREL(r1,trTwo)
RELATION *r1;
TEMPREL *trTwo;
#else
int copyTEMPREL(RELATION *r1, TEMPREL *trTwo)
#endif
/*
  copies the relation in trOne into the relation in trTwo.
*/
{
  RELATION *r2;
  RELMEM  *rm1,*rm2;
  gaget *g1,*g2,*g1bd;
  unsigned index,length,i1,i2;

  if( ((length = r1 ->length)>>SHIFT) + 1 > trTwo -> RMgroups)
  IF expandTempRel(trTwo,length) THEN return(1); ENDIF

  (r2 = &trTwo->rel)->length = length;
  r2 -> pmt = r1->pmt;
  r2 -> tipPtr = NULL;
  r2 -> next = NULL;
  r2 -> index = 0;
  r2 -> creationTime = 0;

  for(index = 0; index < length; ++index)
  {
    rm1 = r1 -> mem[(i1 = (index>>SHIFT))] + (i2 = (index&MASK));
    g2 = (rm2 = r2 -> mem[i1] + i2) -> edgelist;
    rm2 -> coef = rm1 -> coef;
    rm2 -> init = rm1 -> init;
    rm2 -> term = rm1 -> term;

    g1bd = (g1 = rm1 -> edgelist) + (rm2 -> pathlength = rm1 -> pathlength);
    while(g1 < g1bd) *g2++ = *g1++;

  }
  return(0);
}
/* ***************************************************************** */

#ifdef NOPROTO
int QRmult(R1,tr,term)
RELATION *R1;
TEMPREL *tr;
int term;
#else
int QRmult(RELATION *R1,TEMPREL *tr,int term)
#endif
/*
   premultiplies R1 by the string in pre1 of length pre1[0]
   and postmultiples the result by the path in post1 of length
   post1[0].  The result is in tr -> rel.  &tr -> rel must not
   be the same as R1.  Paths with endpoints not matching pre1 or
   post1 are discarded. Returns 1 in case of error and 0 otherwise.
*/
  {
   int i;
   unsigned length,ind,index;
   gaget *gPtr1,*gPtr2,*gPtrBd;
   RELMEM *RM1, *RM2;
   RELATION *R2;

   R2 = &tr->rel;

   IF R1 == NULL
   THEN
     R2 -> length = 0;
     return(0);
   ENDIF

   R2  -> pmt = 1;

   if ((length = R1 -> length) > MAXRMLST * tr -> RMgroups)
     IF expandTempRel(tr,length) THEN return(1); ENDIF

   for(index = ind = 0; ind < length; ++ind)
   {
      RM1 = R1 -> mem[ind>>SHIFT] + (ind&MASK);

      if (RM1->init != term) continue;

      RM2 = R2 -> mem[index>>SHIFT] + (index&MASK);
      ++index;

      RM2 -> pathlength = RM1 -> pathlength;
      RM2 -> coef = RM1 -> coef;
      RM2 -> init = RM1 -> init;
      RM2 -> term = RM1 -> term;

      gPtrBd = (gPtr1 = RM1 -> edgelist) + RM1 -> pathlength;
      gPtr2 = RM2 -> edgelist;

      while(gPtr1 < gPtrBd) *gPtr2++ = *gPtr1++;
   }

   R2 -> length = index;

   return(0);
  }
/* ****************************************************************** */
/* ***************************************************************** */

#ifdef NOPROTO
int multRelations(r1,r2,tr)
RELATION *r1,*r2;
TEMPREL **tr;
#else
int multRelations(RELATION *r1,RELATION *r2,TEMPREL **tr)
#endif
/*
  computes *r1 * *r2 modulo paths of length greater than maxlength
  and returns the result at (*tr)->rel.  In case of error 1 is returned.
  tr4, and tr5 may not be in use.  extpre and post1 may also not be in use.
*/
{
  RELATION *r3;
  RELMEM *rm1;
  gaget *g1,*g2,*g1bd;
  int i,j;
  unsigned index;

  (*tr)->rel.length = 0;

  IF r1 == NULL || r2 == NULL THEN return(0); ENDIF
  IF !r1->length || !r2 -> length THEN return(0); ENDIF

  if(homogeneous || NoFallBack)
  IF r1->mem[0]->pathlength + r2->mem[0]->pathlength >  maxlength
  THEN
    return(0); 
  ENDIF

  extpost[0] = 0;
  index = 0;

  while (index < r1 -> length)
  {
    extpre[0]= (int) (rm1= r1->mem[index>>SHIFT]+(index&MASK))->pathlength;

    IF rm1 -> pathlength
    THEN
      g1bd = (g1 = rm1 -> edgelist) + extpre[0];
      g2 = extpre + 1;

      while(g1 < g1bd) *g2++ = *g1++;

      IF PRPmult(r2,tr5,extpre,extpost) THEN return(1); ENDIF;
    ELSE
      IF QRmult(r2,tr5,rm1->term) THEN return(1); ENDIF;
    ENDIF

    IF addRels(&tr5->rel,&(*tr)->rel,tr4,rm1->coef) THEN return(1); ENDIF

    trtemp = *tr; 
    *tr = tr4; 
    tr4 = trtemp; 

    ++index;
  }

  return(0);
}
/* ***************************************************************** */
#ifdef NOPROTO
int rInverse(r)
RELATION *r;
#else
int rInverse(RELATION *r)
#endif
/*
  It is assumed that all of the paths of r begin and end at some
  vertex v1.  We find a relation r1 such that r * r1 = v1.
  The relation r1 is returned in tr0.
  It is also assumed that tr0,tr1,tr2, and tr3 are not in use.
*/
{
  RELATION *r1;
  RELMEM   *rm,*rm1;
  int coef,mult,InvMult,ind;
  unsigned index;

  tr0->rel.length = 0;

  IF r == NULL || !r->length
  THEN
    PRINTF("\n\n Null in rInverse, program aborted. \n");
    return(1);
  ENDIF

  index = 0;
  while(index < r -> length)
  {
    rm = r -> mem[index>>SHIFT] + (index&MASK);
    if(!rm->pathlength) break;
    ++index;
  }

  IF index != r->length
  THEN
    mult = rm -> coef;
    InvMult = pInverse(mult);
  ELSE
    PRINTF("\n\n No vertex in relation in rInverse, program aborted. \n");
    return(1);
  ENDIF

  IF !tr0->RMgroups THEN if(expandTempRel(tr0,1)) return(1); ENDIF
  (rm1 = tr0->rel.mem[0]) -> coef = 1;
  rm1 -> init = rm1 -> term = rm -> term;
  rm1 -> pathlength = 0;
  tr0->rel.length = 1;

  IF copyTEMPREL(r,tr1) THEN return(1); ENDIF
  r1 = &tr1->rel;

  index = 0;
  while(index < r1 -> length)
  {
    rm = r1 -> mem[index>>SHIFT] + (index&MASK);
    rm -> coef = pMult(InvMult,rm ->coef);
    ++index;
  }

  IF addRels(&tr0->rel,&tr1->rel,tr2,-1) THEN return(1); ENDIF

  IF !tr2 -> rel.length THEN goto out; ENDIF
  IF copyTEMPREL(&tr2->rel,tr1) THEN return(1); ENDIF

  coef = -1;

  while(tr1->rel.length != 0 )
  {
    IF addRels(&tr1->rel,&tr0->rel,tr3,coef) THEN return(1); ENDIF
    trtemp = tr0;
    tr0 = tr3;
    tr3 = trtemp;
    coef = - coef;

    IF multRelations(&tr1->rel,&tr2->rel,&tr3) THEN return(1); ENDIF
    trtemp = tr3; tr3 = tr4; tr4 = trtemp;
    trtemp = tr2; tr2 = tr5; tr5 = trtemp;
    IF (ind = reduce(&tr4->rel,0,0)) == 1 THEN return(1); ENDIF;

    trtemp = tr2; tr2 = tr5; tr5 = trtemp;
    IF ind != 2 THEN trtemp = tr4; tr4 = tr1; tr1 = trtemp; ENDIF
  }

   out:

   r1 = &tr0->rel;
   index = 0;
   while(index < r1 -> length)
   {
     rm = r1 -> mem[index>>SHIFT] + (index&MASK);
     rm -> coef = pMult(rm->coef,InvMult);
     ++index;
   }

  return(0);
}
/* *************************************************************  */
