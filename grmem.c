/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "alloc.h"
#include "grs.h"
#include "grglobal.h"

typedef struct rwork
{
  unsigned int used;
  RELATION rels[MAXREL];
  struct rwork *next;
} RWORK;

typedef struct pwork
{ 
  unsigned int used; 
  PATH paths[MAXPATH];
  struct pwork *next;
} PWORK;

typedef struct rmwork
{
  unsigned int used;
  RELMEM relmems[MAXRELMEM];
  struct rmwork *next;
} RMWORK;

typedef struct gwork
{
  unsigned int used;
  gaget gagets[MAXGAGET];
  struct gwork *next;
} GWORK;

typedef struct rmpwork
{
  unsigned int used;
  RELMEM *rmp[MAXRMPTR];
  struct rmpwork *next;
} RMPWORK;

typedef struct lwork
{ 
  unsigned int used; 
  unsigned long longs[MAXLONG];
  struct lwork *next;
} LWORK;

typedef struct alwork
{
  unsigned int used;
  MEMADJLST mems[MAXADJLST];
  struct alwork *next;
} ALWORK;

typedef struct ovwork
{ 
  unsigned int used; 
  OVERLAPS overlaps[MAXOVERLAP];
  struct ovwork *next;
} OVWORK;


typedef struct relrecord
{
  RWORK *Rcurrent, *Rstart,*Rrestart;
  RMWORK *RMcurrent, *RMstart,*RMrestart;
  RMPWORK *RMPcurrent,*RMPstart,*RMPrestart; 
  GWORK *Gcurrent, *Gstart,*Grestart;
  unsigned int ReRused,ReRMused,ReRMPused,ReGused;

} RELRECORD;


typedef struct overlaprecord
{
  OVWORK *OVcurrent,*OVstart;
  LWORK  *Lcurrent,*Lstart;
} OVERLAPRECORD;

typedef struct alrecord
{
  ALWORK *ALcurrent,*ALstart;
} ALRECORD;

typedef struct pathrecord
{
  PWORK *Pcurrent, *Pstart,*Prestart;
  GWORK *Gcurrent, *Gstart,*Grestart;
  unsigned int RePused,ReGused;
} PATHRECORD;

static RELRECORD rRec;
static PATHRECORD pRec;
static OVERLAPRECORD oRec;
static ALRECORD aRec;
/* *************************************************************  */
#ifdef NOPROTO
static int MakeArray2(pp,dim1,dim2)
short int ***pp;
unsigned dim1,dim2;
#else
static int MakeArray2(short int ***pp,unsigned dim1,unsigned dim2)
#endif
						    
{
   short **p1,*p2,**p1Bd;

   p1 = *pp = (short **) malloc(dim1 * sizeof(short *));
   p2 = (short *) malloc(dim1 * dim2 * sizeof(short));

   IF p1 == NULL || p2 == NULL THEN return(1); ENDIF

   p1Bd = p1 + dim1;

   while(p1 < p1Bd)
   {
     *p1++ = p2;
     p2 += dim2;
   }

   return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
static int MkRowArr(pp,dim1,dim2)
double ***pp;
unsigned dim1,dim2;
#else
static int MkRowArr(double ***pp,unsigned dim1,unsigned dim2)
#endif

{
   double **p1,*p2,**p1Bd;

   p1 = *pp = (double **) malloc(dim1 * sizeof(double *));
   p2 = (double *) malloc(dim1 * dim2 * sizeof(double));
   rows = (double **) malloc(dim1 * sizeof(double *));
   IF p1 == NULL || p2 == NULL || rows == NULL THEN return(1); ENDIF

   p1Bd = p1 + dim1;

   while(p1 < p1Bd)
   {
     *p1++ = p2;
     p2 += dim2;
   }

   return(0);
}

/* *************************************************************  */

#ifdef NOPROTO
static int MakeArray3(pp, dim1, dim2, dim3)
unsigned int ****pp;
unsigned dim1,dim2,dim3;
#else
static int MakeArray3(unsigned int ****pp,unsigned dim1,unsigned dim2,
						    unsigned dim3)
#endif
{
   unsigned int ***p1,**p2,*p3,***p1Bd,**p2Bd;

   p1 = *pp = (unsigned int ***) malloc(dim1 * sizeof(unsigned int **));
   p2 = (unsigned int **) malloc(dim1 * dim2 * sizeof(unsigned int *));
   p3 = (unsigned int *)  malloc(dim1*dim2*dim3 * sizeof(unsigned int));
   IF p1 == NULL || p2 == NULL || p3 == NULL THEN return(1); ENDIF

   p1Bd = p1 + dim1;

   while(p1 < p1Bd)
   {
     *p1++ = p2;
     p2Bd = p2 + dim2;

     while(p2 < p2Bd)
     {
       *p2++ = p3;
       p3 += dim3;
     }
   }

   return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
void setRestart()
#else
void setRestart(void)
#endif
{
  rRec.Rrestart = rRec.Rcurrent;
  rRec.RMrestart = rRec.RMcurrent;
  rRec.RMPrestart = rRec.RMPcurrent;
  rRec.Grestart = rRec.Gcurrent;
  pRec.Grestart = pRec.Gcurrent;
  pRec.Prestart = pRec.Pcurrent;

  rRec.ReRused = rRec.Rcurrent->used;
  rRec.ReRMused = rRec.RMcurrent->used;
  rRec.ReRMPused = rRec.RMPcurrent->used;
  rRec.ReGused = rRec.Gcurrent->used;
  pRec.ReGused = pRec.Gcurrent->used;
  pRec.RePused = pRec.Pcurrent->used;
  return;
}
/* *************************************************************  */
#ifdef NOPROTO
void resetRestart()
#else
void resetRestart(void)
#endif
{
  rRec.Rrestart = rRec.Rcurrent = rRec.Rstart;
  rRec.RMrestart = rRec.RMcurrent = rRec.RMstart;
  rRec.RMPrestart = rRec.RMPcurrent = rRec.RMPstart;
  rRec.Grestart = rRec.Gcurrent = rRec.Gstart;
  pRec.Grestart = pRec.Gcurrent = pRec.Gstart;
  pRec.Prestart = pRec.Pcurrent = pRec.Pstart;

  rRec.ReRused = rRec.Rcurrent->used = 0;
  rRec.ReRMused = rRec.RMcurrent->used = 0;
  rRec.ReRMPused = rRec.RMPcurrent->used = 0;
  rRec.ReGused = rRec.Gcurrent->used = 0;
  pRec.ReGused = pRec.Gcurrent->used = 0;
  pRec.RePused = pRec.Pcurrent->used = 0;
  return;
}
/* *************************************************************  */
#ifdef NOPROTO
static int overhang()
#else
static int overhang(void)
#endif
{
  PLIST **tempPL;
  int i,j;

  tempPL = (PLIST **) malloc((MAXLEVEL+1) * (maxlength+2)*sizeof(PLIST *));

  if(tempPL == NULL) return(1);

  for(i = 0; i < MAXLEVEL + 1; ++ i)
  {
    Overhang[i] = tempPL;
    tempPL += maxlength + 2;

    for(j = 0; j < (int) maxlength + 2; ++j) Overhang[i][j] = NULL;
  }

  return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
int InitMem()
#else
int InitMem(void)
#endif
{
  int i,j,k;
  gaget *gage;
  RELMEM *rmPtr,**rmPtrPtr;

  freeALmem = NULL;
  plistList = NULL;
  lastR = firstR = NULL;
  PRrootPath = rootPath = NULL;
  aRec.ALcurrent = aRec.ALstart = NULL;

  matrix1 = (RMATRIX *) malloc(sizeof(RMATRIX));
  matrix2 = (RMATRIX *) malloc(sizeof(RMATRIX));
  tmatrix = (RMATRIX *) malloc(sizeof(RMATRIX));

  IF overhang() || tmatrix == NULL
  THEN
    PRINTF(" Out of space in InitMem.  Program aborted. \n");
    return(1);
  ENDIF

  for(i=0; i<MAXMATRIXSIZE; ++i)
  {
    matrix1 -> entry[i] = NULL;
    matrix2 -> entry[i] = NULL;
    tmatrix -> entry[i] = NULL;
  }

  matrix1 -> numRows =   matrix2 -> numRows =  tmatrix -> numRows = 0;
  matrix1 -> numCols =   matrix2 -> numCols =  tmatrix -> numCols = 0;

  IF MkRowArr(&ppmatrix,(unsigned) numberVertexes, (unsigned) numberVertexes)
  THEN
    return(1);
  ENDIF

  IF MakeArray3(&tab,(unsigned) numberVertexes+1,
		     (unsigned) numberVertexes+1,maxlength + 1)
  THEN 
    return(1);
  ENDIF   

  IF MakeArray3(&table,(unsigned) numberVertexes+1,
		       (unsigned) numberVertexes+1,MAXLEVEL + 1)
  THEN 
    return(1);
  ENDIF   

  IF MakeArray2(&matrix,(unsigned)numberVertexes+1,
				   (unsigned)numberVertexes+1)
  THEN
    return(1);
  ENDIF

  IF MakeArray2(&pmatrix,(unsigned)numberVertexes+1,
				 (unsigned)numberVertexes+1)
  THEN
    return(1);
  ENDIF


  rpAdjLst = (MEMADJLST **) malloc((unsigned) (numberVertexes + 1)
					   * sizeof(MEMADJLST *));
  rAdjLst = (MEMADJLST **) malloc((unsigned) (numberVertexes + 1)
					   * sizeof(MEMADJLST *));
  AdjLst = (MEMADJLST **) malloc((unsigned) (numberVertexes + 1)
					   * sizeof(MEMADJLST *));
  IF AdjLst == NULL THEN PRINTF(" out of space \n"); return(1); ENDIF

  rRec.Rcurrent = rRec.Rstart = (RWORK *) malloc(sizeof(RWORK));
  rRec.RMcurrent = rRec.RMstart = (RMWORK *) malloc(sizeof(RMWORK));
  rRec.RMPcurrent = rRec.RMPstart = (RMPWORK *) malloc(sizeof(RMPWORK));
  rRec.Gcurrent = rRec.Gstart = (GWORK *) malloc(sizeof(GWORK));

  IF rRec.RMcurrent == NULL || rRec.Rcurrent == NULL  || 
     rRec.RMPcurrent == NULL || rRec.Gcurrent == NULL 
  THEN 
    PRINTF(" out of space in initMem \n"); 
    return(1); 
  ENDIF

  pRec.Gcurrent = pRec.Gstart = (GWORK *) malloc(sizeof(GWORK));
  pRec.Pcurrent = pRec.Pstart = (PWORK *) malloc(sizeof(PWORK));

  IF pRec.Gcurrent == NULL || pRec.Gcurrent == NULL
  THEN
    PRINTF(" out of space in initMem \n"); 
    return(1); 
  ENDIF

  rRec.Rcurrent->used = rRec.RMcurrent->used = rRec.RMPcurrent->used =
  rRec.Gcurrent->used = pRec.Pcurrent->used = pRec.Gcurrent->used = 0;

  rRec.Rcurrent-> next = NULL;
  rRec.RMcurrent->next = NULL;
  rRec.RMPcurrent->next = NULL;
  rRec.Gcurrent->next = NULL;
  pRec.Pcurrent->next = NULL;
  pRec.Gcurrent->next = NULL;

  setRestart();

  oRec.Lcurrent = oRec.Lstart = (LWORK *) malloc(sizeof(LWORK));
  oRec.OVcurrent = oRec.OVstart = (OVWORK *) malloc(sizeof(OVWORK));

  IF oRec.OVcurrent == NULL || oRec.Lcurrent == NULL
  THEN
    PRINTF(" out of space in initMem \n"); 
    return(1); 
  ENDIF

  oRec.Lcurrent->used = 0;
  oRec.OVcurrent-> used = 0;
  oRec.Lcurrent-> next = NULL;
  oRec.OVcurrent-> next = NULL;

  (tr0 = tempRel) -> RMgroups = 0;
  (tr1 = tempRel + 1) -> RMgroups = 0;    
  (tr2 = tempRel + 2) -> RMgroups = 0;
  (tr3 = tempRel + 3) -> RMgroups = 0;
  (tr4 = tempRel + 4) -> RMgroups = 0;
  (tr5 = tempRel + 5) -> RMgroups = 0;
  tr0->rel.usedAt = tr1->rel.usedAt = tr2->rel.usedAt = tr3->rel.usedAt =
  tr4->rel.usedAt = tr5->rel.usedAt = NULL;

  list = (gaget *) malloc((unsigned) 5 * (maxlength+2) * sizeof(gaget));
  pre = list + maxlength + 2;
  post = pre + maxlength + 2;
  extpre = post + maxlength + 2;
  extpost = extpre + maxlength + 2;

  IF list  == NULL
  THEN 
    PRINTF(" out of space in initMem \n"); 
    return(1); 
  ENDIF

  rtime = 0;
  rIndex = 0;

  return(0);
}

/* **************************************************************** */
#ifdef NOPROTO
static int storeRel1(r2,root)
RELATION *r2;
PATH **root;
#else
static int storeRel1(RELATION *r2,PATH **root)
#endif
/*
   puts relation stored in r2 into permananent storage.
   Updates pointer at r2 -> usedAt whenever this is non NULL.
*/
{
   RELATION *r1,**addrPtr;
   RELMEM *rmPtr,*rmPtr1,*rmPtr2;
   gaget  *gPtr,*gPtr1,*gPtr2;
   int i,j,k,numleft,restarted=0;
   unsigned index,numlet;

/*
  Try to insure that enough memory is available.  If not collect garbage.
  For very long relations, we may fail to collect garbage in time, but
  we would expect our program to exhaust memory anyway.
*/     

   restart:

   IF (rRec.Rcurrent -> used) >= MAXREL
   THEN
     IF rRec.Rcurrent->next == NULL
     THEN
       IF NULL == (rRec.Rcurrent -> next = (RWORK *) malloc(sizeof(RWORK)))
       THEN
	 IF !restarted 
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in storeTempRel. \n");
	 return(1);
       ENDIF

       rRec.Rcurrent-> next -> next = NULL;
     ENDIF

     rRec.Rcurrent = rRec.Rcurrent -> next;
     rRec.Rcurrent -> used = 0;
   ENDIF

   IF (rRec.RMcurrent -> used) + r2 -> length >= MAXRELMEM
   THEN
     IF rRec.RMcurrent->next == NULL
     THEN
       IF NULL == (rRec.RMcurrent -> next = (RMWORK *) malloc(sizeof(RMWORK)))
       THEN
	 IF !restarted 
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in storeTempRel. \n");
	 return(1);
       ENDIF

       rRec.RMcurrent-> next -> next = NULL;
     ENDIF

     rRec.RMcurrent -> next -> used = 0;
   ENDIF

   IF (int) (rRec.RMPcurrent->used+((r2->length+MAXRMLST-1)>>SHIFT)) >=
							 MAXRMPTR
   THEN
     IF (r2->length+MAXRMLST-1)/MAXRMLST >= MAXRMPTR
     THEN
       PRINTF(" Maximum number of paths in an element has been exceeded.");
       PRINTF(" This length is %d \n which is the product",MAXRMLST*MAXRMPTR);
       PRINTF(" of the parameters MAXRMLST and MAXRMPTR.\n");
       return(1);
     ENDIF

     IF rRec.RMPcurrent->next == NULL
     THEN
       IF NULL == (rRec.RMPcurrent->next=(RMPWORK *) malloc(sizeof(RMPWORK)))
       THEN
	 IF !restarted 
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in storeTempRel. \n");
	 return(1);
       ENDIF

       rRec.RMPcurrent-> next -> next = NULL;
     ENDIF

     rRec.RMPcurrent = rRec.RMPcurrent -> next;
     rRec.RMPcurrent -> used = 0;
   ENDIF

   k = index = numlet = 0;

   for(i=0;i < (int) r2->length;++i)
   {
     numlet += r2 -> mem[k][index].pathlength;
     IF ++index == MAXRMLST THEN index = 0; ++k; ENDIF
   }

   IF rRec.Gcurrent -> used + (unsigned) numlet >= MAXGAGET
   THEN
     IF rRec.Gcurrent->next == NULL
     THEN
       IF NULL == (rRec.Gcurrent->next=(GWORK *) malloc(sizeof(GWORK)))
       THEN
	 IF !restarted 
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in storeTempRel. \n");
	 return(1);
       ENDIF

       rRec.Gcurrent-> next -> next = NULL;
     ENDIF

     rRec.Gcurrent -> next -> used = 0;
   ENDIF

/*  Copy relation into permanant storage. */

   r1 = rRec.Rcurrent->rels + rRec.Rcurrent->used++;
   *r1 = *r2;

   IF (r1 -> usedAt)!=NULL 
   THEN 
     addrPtr = r2->usedAt;
     r2 -> usedAt = NULL;
     r1 -> usedAt = addrPtr;
     *(r1->usedAt) = r1; 
   ENDIF

   r1 -> next = NULL;
   r1 -> index = ++rIndex;

/*  Space assigned for r1's array of arrays of RELMEMs */

   r1->mem = &rRec.RMPcurrent -> rmp[rRec.RMPcurrent->used];
   rRec.RMPcurrent->used += ((r2->length+MAXRMLST-1)/MAXRMLST);

   for(numleft =(int) r2->length,i=0;numleft>0;++i,numleft -= (int) MAXRMLST)
   {

/*  Space assigned for r1's i-th array of RELMEMs */

     IF (rRec.RMcurrent->used + (unsigned)
	(index =(numleft < (int) MAXRMLST)?(unsigned) numleft:MAXRMLST))
						       >= MAXRELMEM
     THEN
       IF rRec.RMcurrent->next != NULL 
       THEN 
	 rRec.RMcurrent = rRec.RMcurrent -> next; 
       ELSE
	 IF NULL == (rRec.RMcurrent->next = (RMWORK *) malloc(sizeof(RMWORK)))
	 THEN
	   PRINTF(" out of space in storeTempRel. \n");
	   return(1);
	 ELSE
	   rRec.RMcurrent = rRec.RMcurrent->next;
	   rRec.RMcurrent -> next = NULL;
	   rRec.RMcurrent -> used = 0;
	 ENDIF
       ENDIF
     ENDIF 

     rmPtr = r1 -> mem[i] = &rRec.RMcurrent->relmems[rRec.RMcurrent->used];
     rRec.RMcurrent -> used += (unsigned) index;
     rmPtr2 = rmPtr + index;

/* assign ptr for array of gaget and copy path for RELMEM */

     for(rmPtr1 = r2 -> mem[i]; rmPtr < rmPtr2; ++rmPtr,++rmPtr1)
     {
	*rmPtr = *rmPtr1;

	IF rRec.Gcurrent->used + (index= rmPtr1->pathlength) >=(int) MAXGAGET
	THEN
	  IF rRec.Gcurrent->next != NULL
	  THEN
	    rRec.Gcurrent = rRec.Gcurrent -> next;
	  ELSE
	    IF NULL == (rRec.Gcurrent->next = (GWORK *) malloc(sizeof(GWORK)))
	    THEN
	      PRINTF(" out of space in storeTempRel. \n");
	      return(1);
	    ELSE
	      rRec.Gcurrent = rRec.Gcurrent->next;
	      rRec.Gcurrent -> next = NULL;
	      rRec.Gcurrent -> used = 0;
	    ENDIF
	  ENDIF
	ENDIF

	gPtr=rmPtr->edgelist = &rRec.Gcurrent->gagets[rRec.Gcurrent->used];
	rRec.Gcurrent -> used += (unsigned) index;
	gPtr2 = gPtr + index;
	gPtr1 = rmPtr1->edgelist;
	while(gPtr < gPtr2) *gPtr++ = *gPtr1++;
     }
   } 

/*  Enter relation in linked list */

   IF lastR != NULL 
   THEN
     lastR -> next = r1;
     lastR = r1;
   ELSE
     firstR = lastR = r1;
   ENDIF

   r1 -> overPtr = NULL;
   return(0);
}
/* **************************************************************** */
#ifdef NOPROTO
int storeRel(tr,root)
TEMPREL *tr;
PATH **root;
#else
int storeRel(TEMPREL *tr,PATH **root)
#endif
/*
  If one stores a relation in permanant storage, there is a danger that
  it will be moved before it can be stored.  This call insures that this
  situation does not arise.
*/
{
   return(storeRel1(&tr->rel,root));
}
/* **************************************************************** */

#ifdef NOPROTO
int expandTempRel(tr,numRelMem)
TEMPREL *tr;
unsigned numRelMem;
#else
int expandTempRel(TEMPREL *tr,unsigned numRelMem)
#endif
/*
  increases the size of the storage allocated to tr.
  Since this routine may be called when tr is partially full,
  the order of the RELMEMs must be preserved.
*/
{
   unsigned i,j,k,size,newsize;
   unsigned int tmaxlength = maxlength,ii;
   gaget *gage;
   RELMEM *rmPtr,**rmPtrPtr,**tempRmPtrPtr;

   maxlength = 2 * maxlength;

   size = tr -> RMgroups;
   i = ((numRelMem + MAXRMLST - 1)/MAXRMLST);
   newsize = (2 * size > i)?2*size:2*i;
   if( newsize < 4) newsize = 4;

   rmPtrPtr = (RELMEM **) malloc((unsigned) newsize * sizeof(RELMEM *));
   rmPtr = (RELMEM *) malloc((ii=(unsigned) (newsize-size) * MAXRMLST) 
			      * sizeof(RELMEM));
   gage = (gaget *) malloc(ii * maxlength * sizeof(gaget));

   IF gage == NULL || rmPtr == NULL || rmPtrPtr == NULL
   THEN 
     PRINTF(" out of space in expandTempRel \n"); 
     return(1); 
   ENDIF

   tr->RMgroups = newsize;
   tempRmPtrPtr = tr -> rel.mem;
   tr->rel.mem = rmPtrPtr;

   for(j=0;j < size;++j)
   for(k=0;k < MAXRMLST; ++k)
   {
     tr->rel.mem[j] = tempRmPtrPtr[j];
   }

   rmPtrPtr += size;

   for(j = size; j < newsize;++j)
   {
     *rmPtrPtr++ = rmPtr;
     rmPtr += MAXRMLST;

     for(k=0;k < MAXRMLST;++k)
     {
       tr->rel.mem[j][k].edgelist = gage;
       gage = gage + maxlength;
     }
   }

   maxlength = tmaxlength;

   return(0);
}

/* **************************************************************** */
#ifdef NOPROTO
int enterTip(root)
PATH **root;
#else
int enterTip(PATH **root)
#endif
/*
   makes a path out of the leading RELMEM of r1 and stores the
   path in the tree of tips.  It is assumed that the path is
   not divisible by another tip in the tree.  If a memory error
   occurs 1 is returned, otherwise 0 is returned.
*/
{
   PATH *p1;
   RELMEM *rm;
   int restarted = 0;
   gaget *gPtr,*gPtr1,*gPtr2;

   IF lastR == NULL || !lastR -> length THEN return(0); ENDIF

   restart:

   rm = lastR -> mem[0];

   IF (pRec.Gcurrent -> used + (unsigned) rm -> pathlength) >= MAXGAGET
   THEN
     IF pRec.Gcurrent->next == NULL
     THEN
       IF NULL == (pRec.Gcurrent -> next = (GWORK *) malloc(sizeof(GWORK)))
       THEN
	 IF !restarted
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in enterTip. \n");
	 return(1);
       ENDIF

       pRec.Gcurrent-> next -> next = NULL;
     ENDIF

     pRec.Gcurrent = pRec.Gcurrent -> next;
     pRec.Gcurrent -> used = 0;
   ENDIF


   IF (pRec.Pcurrent -> used) >= MAXPATH
   THEN
     IF pRec.Pcurrent->next == NULL
     THEN
       IF NULL == (pRec.Pcurrent -> next = (PWORK *) malloc(sizeof(PWORK)))
       THEN
	 IF !restarted
	 THEN
	   restarted = 1;
	   IF !CollectGarbage(root) THEN goto restart; ENDIF
	 ENDIF

	 PRINTF(" out of space in enterTip. \n");
	 return(1);
       ENDIF

       pRec.Pcurrent-> next -> next = NULL;
     ENDIF

     pRec.Pcurrent = pRec.Pcurrent -> next;
     pRec.Pcurrent -> used = 0;
   ENDIF

   p1 = lastR -> tipPtr  = pRec.Pcurrent->paths + pRec.Pcurrent->used++;

   gPtr = p1->edges = pRec.Gcurrent->gagets + pRec.Gcurrent->used;
   gPtr2 = gPtr + rm -> pathlength;
   pRec.Gcurrent -> used += (unsigned) (p1 -> length = rm->pathlength);
   gPtr1 = rm -> edgelist;

   while(gPtr < gPtr2) *gPtr++ = *gPtr1++;

   p1 -> lchild = p1 ->  rchild = NULL;
   p1 -> relptr = lastR;
   p1 -> init = rm -> init;
   p1 -> term = rm -> term;
   p1 -> length = rm -> pathlength;

   IF insertPath(p1,root) 
   THEN 
     PRINTF("\n Problem in enterTip\n");
     return(1);
   ENDIF

   return(0);
}
/* **************************************************************** */
#ifdef NOPROTO
int CollectGarbage(root)
PATH **root;
#else
int CollectGarbage(PATH **root)
#endif
/*
  reclaims space used by stored relations for which pmt = 0
  and paths which are not minimal tips.  It is assumed that the relations
  were stored in order beginning with firstR and ending with lastR;
*/
{
   RELATION *tempR,nextR;
   int done = 0;

   IF (tempR = firstR) == NULL THEN return(0); ENDIF
   firstR = lastR = NULL;
   *root = NULL;

   (rRec.Rcurrent = rRec.Rrestart) -> used = rRec.ReRused;
   (rRec.RMcurrent = rRec.RMrestart) -> used = rRec.ReRMused;
   (rRec.RMPcurrent = rRec.RMPrestart) -> used = rRec.ReRMPused;
   (rRec.Gcurrent = rRec.Grestart) -> used = rRec.ReGused;
   (pRec.Pcurrent = pRec.Prestart) -> used = pRec.RePused;
   (pRec.Gcurrent = pRec.Grestart) -> used = pRec.ReGused;

   while(!done)
   {
      nextR = *tempR;

      if(tempR -> pmt)
      {
	if(storeRel1(tempR,root)) return(1);

	if(nextR.pmt == 1) if(enterTip(root)) return(1);
      }

      done = (tempR = nextR.next) == NULL;
   }

   return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
int getALMems(temp, ii)
MEMADJLST **temp;
int ii;
#else
int getALMems(MEMADJLST **temp,int ii)
#endif
{
  *temp = (MEMADJLST *) malloc((unsigned) ii * sizeof(MEMADJLST));
  IF *temp == NULL THEN PRINTF(" out of space "); return(1); ENDIF
  return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
int getALMem(temp)
MEMADJLST **temp;
#else
int getALMem(MEMADJLST **temp)
#endif
{
   int nn = 16;
   MEMADJLST *tem,*bound;

   IF freeALmem == NULL 
   THEN
     if(getALMems(&tem,nn)) return(1); 

     bound = tem + nn;
     while(tem < bound) 
     {
       tem -> Next = freeALmem;
       freeALmem = tem++;
     }
   ENDIF

   *temp = freeALmem;
   freeALmem = freeALmem -> Next;

   return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
void freeALmems(temp)
MEMADJLST **temp;
#else
void freeALmems(MEMADJLST **temp)
#endif
{
  (*temp) -> Next = freeALmem;
  freeALmem = *temp;
  *temp = NULL;
  return;
}
/* *************************************************************  */
#ifdef NOPROTO
int newRelation(root,tr)
PATH **root;
TEMPREL *tr;
#else
int newRelation(PATH **root,TEMPREL *tr)
#endif
/*
    updates the list of possible members of Groebner basis.
*/
{
    RELATION *tempR;

    tempR = &tr->rel;
    LeadOne(tr);
    checkTree(tempR,root);
    tempR->creationTime      = rtime;
    tempR->pmt=1;

    IF storeRel(tr,root) THEN return(1); ENDIF
    IF enterTip(root) THEN return(1); ENDIF

    return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
void aRecoRecRestart()
#else
void aRecoRecRestart(void)
#endif
{
  IF aRec.ALcurrent != NULL
  THEN
    aRec.ALcurrent = aRec.ALstart;
    aRec.ALcurrent-> used = 0;
  ENDIF

  IF oRec.OVcurrent != NULL
  THEN
    oRec.OVcurrent = oRec.OVstart;
    oRec.OVcurrent-> used = 0;
    oRec.Lcurrent = oRec.Lstart;
    oRec.Lcurrent->used = 0;
  ENDIF

  return;
}

/* ************************************************************* */

#ifdef NOPROTO
int getALmem(temp)
MEMADJLST **temp;
#else
int getALmem(MEMADJLST **temp)
#endif
{
  IF aRec.ALcurrent == NULL
  THEN
    aRec.ALcurrent = aRec.ALstart = (ALWORK *) malloc(sizeof(ALWORK));

    IF aRec.ALcurrent == NULL 
    THEN
      PRINTF(" out of space in getALmem \n");
      return(1);
    ENDIF

    aRec.ALcurrent->used = 0;
    aRec.ALcurrent-> next = NULL;
  ENDIF

  IF aRec.ALcurrent->used >= MAXADJLST
  THEN
    IF aRec.ALcurrent->next == NULL
    THEN
      aRec.ALcurrent->next = (ALWORK *) malloc(sizeof(ALWORK));

      IF aRec.ALcurrent->next == NULL
      THEN
	PRINTF(" out of space in getALmem \n");
	return(1);
      ENDIF
    ENDIF

    aRec.ALcurrent = aRec.ALcurrent->next;
    aRec.ALcurrent->used = 0;
    aRec.ALcurrent-> next = NULL;
  ENDIF

  *temp = aRec.ALcurrent->mems + (aRec.ALcurrent->used)++;

  return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
int getOverlap(ov,numcells)
OVERLAPS **ov;
int numcells;
#else
int getOverlap(OVERLAPS **ov,int numcells)
#endif
{
   IF (oRec.OVcurrent -> used) >= (int) MAXOVERLAP
   THEN
     IF oRec.OVcurrent->next == NULL
     THEN
       IF NULL == (oRec.OVcurrent->next = (OVWORK *) malloc(sizeof(OVWORK)))
       THEN
	 PRINTF(" out of space in getOverlap. \n");
	 return(1);
       ENDIF

       oRec.OVcurrent-> next -> next = NULL;
     ENDIF

     oRec.OVcurrent = oRec.OVcurrent -> next;
     oRec.OVcurrent -> used = 0;
   ENDIF

   *ov = &oRec.OVcurrent -> overlaps[oRec.OVcurrent->used++];

   IF (oRec.Lcurrent -> used + (unsigned) numcells) >=  MAXLONG
   THEN
     IF oRec.Lcurrent->next == NULL
     THEN
       IF NULL == (oRec.Lcurrent->next = (LWORK *) malloc(sizeof(LWORK)))
       THEN
	 PRINTF(" out of space in getOverlap. \n");
	 return(1);
       ENDIF

       oRec.Lcurrent-> next -> next = NULL;
     ENDIF

     oRec.Lcurrent = oRec.Lcurrent -> next;
     oRec.Lcurrent -> used = 0;
   ENDIF

   (*ov) -> masklist = &oRec.Lcurrent -> longs[oRec.Lcurrent->used];
   oRec.Lcurrent->used += (unsigned) numcells;

   return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
int cleanUp()
#else
int cleanUp(void)
#endif
{
  int i,j,index,indexE,chr;
  RELATION *r;
  RELMEM   *RM;
  PLIST    *PL,*PLtemp;
  MEMADJLST *temp,*MALtemp;
  RMATRIX *tempM;

  for(i=0;i<=numberVertexes;++i)
  for(j=0;j<=numberVertexes;++j) matrix[i][j] = pmatrix[i][j];
/*
  temp = AdjLst[numberVertexes];
  AdjLst[numberVertexes] = NULL;

  while (temp != NULL)
  {
    MALtemp = temp;
    temp = temp -> Next;
    freeMemadjlst(&MALtemp);
  }
*/

  index = 0;
  for(i=0; i < matrix1->numRows; ++i)
  for(j=0; j < matrix1->numCols; ++j)
  {
    if(matrix1->entry[index] != NULL)
    {
      matrix1->entry[index] -> pmt = 0;
      matrix1->entry[index] -> usedAt = NULL;
      matrix1->entry[index] = NULL;
    }
    ++index;
  }

  matrix1->numRows = matrix1->numCols = 0;

  tempM = matrix1;
  matrix1 = matrix2;
  matrix2 = tempM;

/*
   Mark relations which are not in Groebner Basis of the ideal as
   not used.
*/
  r = firstR;
  while(r != NULL)
  {
    if(r->pmt == 1||r->pmt==0 )
    {
      r->pmt = 0;
      r -> tipPtr = NULL;
      r -> usedAt = NULL;
    }
    r = r ->next;
  }

  IF CollectGarbage(&PRrootPath) THEN return(1); ENDIF

  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
int getPlist(pl)
PLIST **pl;
#else
int getPlist(PLIST **pl)
#endif
{
  PLIST *pltemp;

  IF plistList == NULL
  THEN
     pltemp = plistList = (PLIST *) malloc(PLNUM * sizeof(PLIST));

     IF plistList == NULL
     THEN
	PRINTF(" Not enough space in getPlist. \n");
	return(1);
     ENDIF

     while(pltemp < plistList + PLNUM - 1) 
     {
       pltemp -> next = pltemp + 1;
       pltemp = pltemp -> next;
     }

     pltemp -> next = NULL;
  ENDIF

  *pl = plistList;
  plistList = (*pl) -> next;
  return(0);
}

/* ****************************************************************** */
#ifdef NOPROTO
void freePlist(pl)
PLIST **pl;
#else
void freePlist(PLIST **pl)
#endif
{
  IF *pl == NULL THEN return; ENDIF

  (*pl) -> next = plistList;
  plistList = *pl;
  *pl = NULL;
  return;
}
/* ****************************************************************** */
