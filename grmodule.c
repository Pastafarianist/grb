/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"
#include "time.h"

/* ***************************** grrelops.c *************************** */
#ifndef NOPROTO
int expand(RELATION *r1,PATH **root,int projres);
#endif

/* ************************************************************* */

#ifdef NOPROTO
static int UO(r1,r2,projres)
RELATION *r1, *r2;
int projres;
#else
static int UO(RELATION *r1,RELATION *r2,int projres)
#endif
{
   int num = 0,i = 0;
   RELATION *tempR;
   RELMEM *rm1,*rm2;
   gaget *g1,*g2,*gPtrBd;

   int length1,length2,j,k,index,coef;

   length1 = (int) (rm1 = *r1 -> mem) -> pathlength;
   g1 = rm1 -> edgelist;

   length2 = (int) (rm2 = *r2 -> mem) -> pathlength;
   g2 = rm2 -> edgelist;

   index = overlap(g1,g2,length1,length2,(int) maxlength);

   while (i < index)
   {
     if(r1 -> pmt != 1 || r2 -> pmt != 1) break;

     pre[0] = 0;

     gPtrBd = (g1 = post + 1) + (post[0] = length2  - (j = list[i] + 1));
     g2 = rm2 -> edgelist + j;

     while(g1 < gPtrBd) *g1++ = *g2++;

     IF PRPmult(r1,tr1,pre,post)
     THEN
       return(1);
     ENDIF;

     post[0] = 0;

     gPtrBd = (g1 = pre + 1) + (pre[0] =  length1 - j);
     g2 = rm1 -> edgelist;

     while(g1 < gPtrBd) *g1++ = *g2++;

     IF PRPmult(r2,tr2,pre,post) THEN return(1); ENDIF;

     IF addRels(&tr1->rel,&tr2->rel,tr0,-1) THEN return(1); ENDIF;

     IF (j = BackReduce(&tr0->rel,projres)) == 1 THEN return(1); ENDIF

     trtemp = (j == 2) ? tr1 : tr0;

     IF trtemp->rel.length
     THEN
       IF newRelation(projres?&PRrootPath:&rootPath,trtemp)
       THEN 
	 return(1);
       ENDIF

       num = 2;
     ENDIF

     ++i;
   }

   return(num);
}

/* ************************************************************* */

#ifdef NOPROTO
static int useOverlap(r1,r2,projres)
RELATION *r1, *r2;
int projres;
#else
static int useOverlap(RELATION *r1,RELATION *r2,int projres)
#endif
/*
  computes all overlaps of r1 and r2, reduces these, and enters non-zero
  relations in relation list.  returns 0 if no non null reduced relations
  are found, 1 if memory problems arise, 2 if new relations are found.
*/
{
   int i,num = 0;

   IF (i = UO(r1,r2,projres)) == 1 THEN return(1); ENDIF
   IF i == 2 THEN num = 2; ENDIF

   if((!projres) && r2->index != r1 -> index)
   {
     IF (i = UO(r2,r1,projres)) == 1 THEN return(1); ENDIF
     IF i == 2 THEN num = 2; ENDIF
   }

   return(num);
}

/* ************************************************************* */

#ifdef NOPROTO
static int initializeList(root, projres)
PATH **root;
int projres;
#else
static int initializeList(PATH **root,int projres)
#endif
/*
  returns 1 if there are problems with memory, otherwise 0.
*/
{
   RELATION *r1,*r2,**starter;
   int  i, equal = 0;

   starter = projres ? &generatorList : &firstR;

   if(firstR==NULL) return(0);

   r2 = firstR;

   while(r2 != NULL)
   {
      r2 -> usedAt = &r2;

      IF r2 -> mem[0] -> pathlength < maxlength
      THEN
	r1 = *starter;

	while(r1 != NULL)
	{
	  IF (!projres && r1 -> index > r2 -> index) || r2 -> pmt != 1 
	  THEN 
	    r2 -> usedAt = NULL; 
	    break; 
	  ENDIF

	  IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

	  if(r1 -> mem[0] -> pathlength < maxlength)
	  {
	    if(!(r1 == r2)) r1 -> usedAt = &r1;
	    else equal = 1;

	    IF(i = useOverlap(r2,r1,projres))==1 THEN return(1); ENDIF

	    if(equal) {equal = 0; r1 = r2;}
	    else r1 -> usedAt = NULL;
	  }
	  r1 = r1 -> next;
	}
      ENDIF

      r1 = firstR;

      while(r1 != NULL)
      {
	 if(r1 -> pmt == 4) r1 -> pmt = 3;
	 r1 = r1 -> next;
      }

      r1 = firstR;
      while(r1 != NULL)
      {
	 IF r1 -> pmt == 3
	 THEN
	   if(r1 != r2) r1 -> usedAt = &r1;
	   else equal = 1;

	   IF (i = BackReduce(r1,projres)) == 1 THEN return(1); ENDIF
	   r1 -> pmt = 0;

	   if(i == 2 && tr1->rel.length)
	   IF newRelation(root,tr1) THEN return(1); ENDIF
	   if(!equal) r1 -> usedAt = NULL; else { equal = 0; r1 = r2; }
	 ENDIF

	 r1 = r1 -> next;
      }

      if(!homogeneous&&!NoFallBack&&r2 -> pmt == 1)
      IF (i = expand(r2,root,projres)) == 1 THEN return(1); ENDIF

      r2 -> usedAt = NULL;
      r2 = r2 -> next;
   }  /* end while not done */

   return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
static int constructBasis(root,projres)
PATH **root;
int projres;
#else
static int constructBasis(PATH **root,int projres)
#endif
/*
  constructs Groebner basis.  returns 1 if there are problems with
  memory, otherwise 0.
*/
{
   RELATION *r1,*r2,**starter;
   int  equal = 0,done = 0,i;
   unsigned long indexBound;

   if(firstR==NULL) return(0);

   starter = projres ? &generatorList : &firstR;

   repeater:

   while(!done)
   {
      indexBound = lastR -> index;
      done = 1;
      ++rtime;

      r1 = firstR;

      while(r1 != NULL)
      {
	 if(r1 -> pmt == 4) r1 -> pmt = 3;
	 r1 = r1 -> next;
      }

      r1 = firstR;
      while(r1 != NULL)
      {
	 IF r1 -> pmt == 3
	 THEN
	   r1 -> usedAt = &r1;
	   IF (i = BackReduce(r1,projres)) == 1 THEN return(1); ENDIF
	   r1 -> pmt = 0;

	   IF i == 2 && (&tr1->rel) ->length
	   THEN
	     done = 0;
	     IF newRelation(root,tr1) THEN return(1); ENDIF
	   ENDIF

	   r1 -> usedAt = NULL;
	 ENDIF

	 r1 = r1 -> next;
      }

      r1 = firstR;

      while(r1 != NULL)
      {
	IF r1 -> index > indexBound THEN break; ENDIF
	IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

	r1 -> usedAt = &r1;
	r2 = *starter;

	if(r1 -> mem[0] -> pathlength < maxlength)
	while(r2 != NULL)
	{
	  if(!projres && r2->index > r1->index) break;
	  IF r2 -> pmt != 1 THEN r2 = r2 -> next; continue; ENDIF

	  if (r2 != r1) r2 -> usedAt = &r2;
	  else equal = 1;

	  IF (i = useOverlap(r1,r2,projres))==1 THEN return(1); ENDIF

	  IF i THEN done = 0; ENDIF

	  if(equal) { equal = 0; r2 = r1; }
	  else r2 -> usedAt = NULL;

	  r2 = r2 -> next;
	}

	r1 -> usedAt = NULL;
	r1 = r1 -> next;
      }


      if(!done)
      {
	PRINTF(" This ideal gives a negative answer to a question of\n");
	PRINTF(" Ed Green.  Please send your data to him. \n");
      }
   }  /* end while not done */

    if(!homogeneous && !NoFallBack)
    {
      r1 = firstR;
      while(r1 != NULL)
      {
	IF r1 -> pmt == 1
	THEN
	  r1 -> usedAt = &r1;
	  IF (i = expand(r1,root,projres)) == 1 THEN return(1); ENDIF
	  if(i) {done = 0;}
	  r1 -> usedAt = NULL;
	ENDIF

	r1 = r1 -> next;
      }

      if(!done) 
      { 
	PRINTF(" This ideal gives a negative answer to a question of\n");
	PRINTF(" Ed Green.  Please send your data to him. \n");
	goto repeater;
      }
    }

   return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
static int sharpenBasis(root, projres)
PATH **root;
int projres;
#else
static int sharpenBasis(PATH **root,int projres)
#endif
{
   RELATION *r1;
   unsigned long indexBound;
   int i;

   if(firstR==NULL) return(0);

   indexBound = lastR -> index;
   r1 = firstR;

   while(r1 != NULL)
   {
     if(r1->index > indexBound) break;

     IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

     IF (i = reduce(r1,1,projres)) == 1 THEN return(1); ENDIF

     IF (i == 2)
     THEN
       r1 ->usedAt = &r1;

       IF deletePath(r1 ->tipPtr,root) THEN return(1); ENDIF
       r1 -> tipPtr = NULL;
       r1 -> pmt = 0;
       tr1->rel.creationTime = r1 -> creationTime;

       LeadOne(tr1);
       IF storeRel(tr1,root) THEN return(1); ENDIF
       IF enterTip(root) THEN return(1); ENDIF
       lastR -> pmt = 1;
       r1 ->usedAt = NULL;
     ENDIF

     r1 = r1 -> next;
   }

   return(0);
}
/* ***************************** grquogen.c *************************** */
#ifndef NOPROTO
int QuotientBasis(PATH **root,int projres);
#endif
/* ************************************************************ */
#ifdef NOPROTO
int getBasis(projres)
int projres;
#else
int getBasis(int projres)
#endif
{
   int i;
   clock_t start,end;
   double Time,totaltime;
   PATH **root;
   RELATION *r1;

   root = projres ? &PRrootPath : &rootPath;

   start = clock();
   IF initializeList(root,projres) THEN return(1); ENDIF
   end = clock();
   totaltime = Time = (end - start)/CLK_TCK;

   if(!projres)
   {
     PRINTF("\n The time in initializeList was %f.\n",(float) Time);
     PRINTF(" The total number of relations examined is %ld.\n",
	       lastR != NULL ? lastR -> index:0);
     PRINTF(" The list of relations has been initialized.\n");
   }

   start = clock();
   IF constructBasis(root,projres) THEN return(1); ENDIF
   end = clock();
   totaltime += (Time = (end - start)/CLK_TCK);

   if(!projres)
   {
     PRINTF("\n The time in constructBasis was %f.\n",(float) Time);
     PRINTF(" The total number of relations examined is %ld.\n",
	       lastR != NULL ? lastR -> index:0);
     PRINTF(" A basis whose tips are minimal has been constructed.\n");
   }

   start = clock();
   IF sharpenBasis(root,projres) THEN return(1); ENDIF
   end = clock();
   totaltime += (Time = (end - start)/CLK_TCK);

   if(!projres)
   {
     PRINTF("\n The time in sharpenBasis was %f.\n",(float) Time);
     PRINTF(" The total number of relations examined is %ld.\n",
	       lastR != NULL ? lastR -> index:0);
     PRINTF(" A sharp Groebner basis has been constructed.\n");

     PRINTF("\n Total time for computation is %f.\n",totaltime);
   }

   if(!projres)
   if(QuotientBasis(root,projres))
      {PRINTF("\n Problems in QuotientBasis.\n");return(1); }

   IF CollectGarbage(root) THEN return(1); ENDIF
   IF !projres THEN setRestart(); ENDIF

   r1 = firstR;
   rIndex = 0;

   while(r1 != NULL)
   {
     if(r1->pmt) r1 -> index = ++rIndex;
     r1 = r1 -> next;
   }

   return(0);

}
