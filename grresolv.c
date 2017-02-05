/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"
#include "conio.h"
/* *************** PROTOTYPES ********************************** */
#ifndef NOPROTO
int NullMatrix(void);
/* *************** IN GRNEW   ********************************** */
int removeVertexes(int first);
void transpose(RMATRIX **mat1,RMATRIX **mat2);
void clearRmatrix(RMATRIX *mat);
int RelToMatrix(RMATRIX *mat,int numRows);
int RelFromMatrix(RMATRIX *mat);
int getBasis(int projres);
int cleanUp(void);
int useMatrx(void);
/* *************** IN GRINIT  ********************************** */
int getMatrix(int repnum);
/* ************************************************************* */
void freePlist(PLIST **pl);
int getPlist(PLIST **pl);
#endif
/* ************************************************************* */
#ifdef NOPROTO
static int store(r1,r2,num)
RELATION *r1,*r2;
int num;
#else
static int store(RELATION *r1,RELATION *r2,int num)
#endif
{
  int i,numcells;
  unsigned length1,index;
  OVERLAPS *ov;

  length1 =  r1 -> mem[0] -> pathlength;
  numcells = (int) (length1 >> 5) + 1;
  IF getOverlap(&ov,numcells) THEN return(1); ENDIF

  ov->rel = r2;
  ov->next = r1 -> overPtr;
  r1 -> overPtr = ov;

  for(i=0; i<numcells;++i) ov -> masklist[i] = 0;

  for( i=0;i < num; ++i)
  {
    index = (unsigned) ((int) length1 - list[i]);
    --index;
    ov -> masklist[index >> 5] |= mask[index & 31];
  }

  return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
static int FindOverlaps(projres)
int projres;
#else
static int FindOverlaps(int projres)
#endif
/*
  It is assumed that all relations in list have minimal tips.
  Also generator list contains the list of sharp elements for the ideal.
*/
{
  RELATION *r1,*r2;
  RELMEM   *rm1,*rm2;
  gaget    *g1,*g2;
  int num,length1,length2;

  r1 = firstR;

  while(r1 != NULL)
  {
    if(r1->pmt != 1){ r1 = r1->next; continue;}

    g1 = (rm1 = r1->mem[0])->edgelist;
    length1 = (int) rm1 -> pathlength;
    r2 = projres ? generatorList : firstR;

    while(r2 != NULL)
    {
      if(r2->pmt != 1){ r2 = r2->next;continue;}
      g2 = (rm2 = r2->mem[0])->edgelist;
      length2 = (int) rm2 -> pathlength;

      IF (num = overlap(g1,g2,length1,length2,(int) (2*maxlength-1))) != 0
      THEN
	IF store(r1,r2,num) THEN return(1); ENDIF
      ENDIF

      r2 = r2 -> next;
    }

    r1 = r1 -> next;
  }

  return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
static int useGamma(r1,r2)
RELATION *r1,*r2;
#else
static int useGamma(RELATION *r1,RELATION *r2)
#endif
{
  int i,j,coef;
  unsigned long index;
  RELATION *tempR,*r3,*r4;
  RELMEM   *rm,*rm1,*rm2,*rm3;
  gaget *g1,*g2,*g3,*gbd;
  RELMEM *p;
  PATH *result;

#ifdef DEBUG
  printf("*******\n");
  fShowRelation(stdout,r1,0);
  fShowRelation(stdout,r2,1);
#endif

  ++(matrix2->numRows);

  IF matrix2 -> numRows * matrix2 -> numCols > MAXMATRIXSIZE
  THEN
    PRINTF("\n\n matrix2 too large, program aborted. \n");
    PRINTF(" numRows %d  numCols %d  product %d maxsize %d\n",
	     matrix2->numRows,matrix2->numCols,
	     matrix2->numRows*matrix2->numCols,MAXMATRIXSIZE);
    return(2);
  ENDIF

  p = bgm.paths[1];
  pre[0] = 0;
  tr4->rel.length = 1;
  (rm=tr4->rel.mem[0])->pathlength= (unsigned)
		((int) p->pathlength - (bgm.LastIndex[1]+1));
  post[0]= (int) rm->pathlength;
			  
  g3 = rm-> edgelist;

  g2 = post + 1;
  gbd = (g1 = p->edgelist + (bgm.LastIndex[1]) + 1) + post[0];
  while(g1 < gbd) *g3++ = *g2++ = *g1++;

  IF PRPmult(r1,tr1,pre,post) THEN return(1); ENDIF

  rm -> init = EdgeList[post[1]].init;
  rm -> term = EdgeList[post[post[0]]].term;
  rm -> coef = 1;

  index = ((unsigned) ((matrix2->numRows-1) * matrix2->numCols)) + r1->index;
  --index;

  IF (tempR = matrix2 -> entry[index]) != NULL
  THEN
    tempR -> usedAt = NULL;
    tempR -> pmt = 0;
    IF addRels(tempR,&tr4->rel,tr5,1) THEN return(1); ENDIF
    trtemp = tr5; tr5 = tr4; tr4 = trtemp;
  ENDIF

  IF tr4 -> rel.length
  THEN
    IF storeRel(tr4,&PRrootPath) THEN return(1); ENDIF
    matrix2 -> entry[index] = lastR;
    lastR -> usedAt = &matrix2 -> entry[index];
    lastR -> pmt = 5;
  ELSE
    matrix2 -> entry[index] = NULL;
  ENDIF

  p = bgm.paths[0];
  pre[0] = (int) p->pathlength - 1 - bgm.LastIndex[1];
  post[0] = 0;

  gbd = (g1 = p->edgelist) + pre[0];
  g2 = pre + 1;
  while(g1 < gbd) *g2++ = *g1++;

  IF PRPmult(r2,tr2,pre,post) THEN return(1); ENDIF
  IF addRels(&tr2->rel,&tr1->rel,tr4,-1) THEN return(1); ENDIF
  IF (i = reduce(&tr4->rel,0,0)) == 1 THEN return(1); ENDIF

  IF i == 2 THEN trtemp = tr1; tr1 = tr4; tr4 = trtemp; ENDIF

  tr5 -> rel.length = 1;

  while((r4 = &tr4->rel)->length != 0)
  {
    rm = (r4 -> mem[0]);
/*
   If tipSearch returns a value other than zero the relation could not
   be reduced by one of the PR basis elements. 
*/
    IF tipSearch(rm->edgelist,(int) rm->pathlength,&result,1) 
    THEN
      fShowRelation(stdout,&tr4->rel,1);
      PRINTF(" Above relation could not be fully reduced in useGamma,");
      PRINTF(" program aborted. \n");
      return(1);
    ENDIF

    tr5->rel.length = 1;
    pre[0] = 0;
    rm2 = (r3 = result -> relptr) -> mem[0];
    rm3 = tr5 -> rel.mem[0];
    rm3->pathlength= rm->pathlength - rm2->pathlength;
    gbd = (g1 = post + 1) + (post[0]= (int) rm3->pathlength);
    g2 = rm->edgelist + rm2->pathlength;
    g3 = rm3 -> edgelist;
    rm3->coef = -rm->coef;

#ifdef DEBUG
    fShowRelation(stdout,r3,0);
    fShowRelation(stdout,r4,1);
#endif

    while(g1 < gbd) *g3++ = *g1++ = *g2++;

    IF post[0]
    THEN
      rm3 ->init = EdgeList[post[1]].init;
      rm3 ->term = EdgeList[post[post[0]]].term;
    ELSE
      rm3 -> init = rm3 -> term = rm -> term;
    ENDIF

    index = (unsigned)((matrix2->numRows-1) * matrix2->numCols) + r3->index;
    --index;

    IF (tempR = matrix2 -> entry[index]) != NULL
    THEN
      tempR -> usedAt = NULL;
      tempR -> pmt = 0;
      IF addRels(tempR,&tr5->rel,tr0,1) THEN return(1); ENDIF
      trtemp = tr5; tr5 = tr0; tr0 = trtemp;
    ENDIF

    IF tr5 -> rel.length
    THEN
      IF storeRel(tr5,&PRrootPath) THEN return(1); ENDIF
      matrix2 -> entry[index] = lastR;
      lastR -> usedAt = &matrix2 -> entry[index];
      lastR -> pmt = 5;
    ELSE
      matrix2 -> entry[index] = NULL;
    ENDIF

/* The multiple of r3 is removed from r4 */
    IF PRPmult(r3,tr0,pre,post) THEN return(1); ENDIF
    IF addRels(&tr0->rel,&tr4->rel,tr5,-rm->coef) == 1 THEN return(1);ENDIF
    IF (i = reduce(&tr5->rel,0,0)) == 1 THEN return(1); ENDIF

    IF i == 2
    THEN
      trtemp = tr1; tr1 = tr4; tr4 = trtemp;
    ELSE
      trtemp = tr5; tr5 = tr4; tr4 = trtemp;
    ENDIF
  }

  return(0);
}
/* ************************************************************* */
#ifdef NOPROTO
static int LastIndex(index,ov,length)
int index,length;
OVERLAPS *ov;
#else
static int LastIndex(int index,OVERLAPS *ov,int length)
#endif
{
  int i,j,lbd,ubd,jnum=0;
  unsigned long tmask;

  for(j = 0; j < (int) (((unsigned) length) >> 5) + 1; ++j)
  {
     tmask = ov -> masklist[j];
     if(!(tmask & EndMask[index + 1])) continue;
     lbd = j==0 ? index + 1:0;
     ubd = maxlength+1 <= 32 ? (int) maxlength+1 : 32;

     for(i = lbd; ubd; ++i)
       IF tmask & mask[i] THEN return(length - i - 1 - jnum); ENDIF

     jnum += 32;
  }

  return(-1);
}
/* ************************************************************* */
#ifdef NOPROTO
static int divides(prl1,prl2,ov1)
PLIST *prl1,*prl2;
int ov1;
#else
static int divides(PLIST *prl1, PLIST *prl2, int ov1)
#endif
/*
  returns 1 if the overhang of prl1 matches the overhang of prl2
  else returns 0.
*/
{
  gaget *g1,*g2,*gbd;

  gbd = (g1 = prl1->rm->edgelist + prl1->LastIndex + 1) + ov1;
  g2 = prl2->rm->edgelist + prl2->LastIndex + 1;

  while(g1 < gbd) if( *g1++ != *g2++) return(0);
/*
  for(i = 1;i <= ov1;++i)
  {
    IF p1->edges[prl1->LastIndex+i] != p2->edges[prl2->LastIndex+i]
    THEN
      return(0);
    ENDIF
  }
*/
  return(1);
}
/* ************************************************************* */
#ifndef NOPROTO
void fShowGamma(FILE *f1,BG *bg);
#endif
/* ************************************************************* */

#ifdef NOPROTO
static int BuildNext(f1,r1,maxlevel,projres)
FILE *f1;
RELATION *r1;
int maxlevel,projres;
#else
static int BuildNext(FILE *f1,RELATION *r1, int maxlevel,int projres)
#endif
{
  RELATION *r2;
  RELMEM *rm2;
  OVERLAPS *overPtr;
  int length,length1,index,i,j;
  PATH *p1;
  PLIST *prl,*prltemp,**addrprltemp;

  IF bgm.level < maxlevel
  THEN
    overPtr = r1 -> overPtr;

    length = (int) bgm.paths[bgm.level]->pathlength;
    index = bgm.LastIndex[bgm.level];
    ++bgm.level;

/*
   Best overlap is found with previous minimal tip for each overlapping
   minimal tip.  These are stored according to the length of the
   overhang.
*/

    while(overPtr != NULL)
    {
      i = LastIndex(index,overPtr,length);

      IF i >= 0
      THEN
	r2 = overPtr -> rel;
	length1 = (int) (rm2 = r2 -> mem[0]) -> pathlength;

	IF getPlist(&prl)
	THEN
	  PRINTF(" Not enough space in BuildNext ");
	  return(1);
	ENDIF

	prl -> rm = rm2;
	prl -> relPtr = r2;
	prl -> next = Overhang[bgm.level][length1-1-i];
	Overhang[bgm.level][length1-1-i] = prl;
	prl -> LastIndex = i;
      ENDIF

      overPtr = overPtr -> next;
    }

/*
  If the overhang for one minimal tip is a substring of the
  overhang for another minimal tip, it is elimiated from the
  list of tips to be considered.
*/

    for(i = 2;i <= (int) maxlength - 1; ++i)
    {
      prltemp = Overhang[bgm.level][i];
      addrprltemp = &Overhang[bgm.level][i];

      while(prltemp != NULL)
      {
	for(j = 1;j < i; ++j)
	{
	  prl = Overhang[bgm.level][j];

	  while(prl != NULL)
	  {
	    IF  divides(prl,prltemp,j)
	    THEN
	      *addrprltemp = prltemp -> next;
	      freePlist(&prltemp);
	      prltemp = *addrprltemp;
	      goto continueOutside;
	    ENDIF

	    prl = prl -> next;
	  }
	}

	addrprltemp = &(prltemp->next);
	prltemp = prltemp -> next;
	continueOutside: ;
      }
    }

/*
  The current gammas are saved.
*/

    for(i = 1;i <= (int) maxlength; ++i)
    {
      prl = Overhang[bgm.level][i];
      Overhang[bgm.level][i] = NULL;

      while(prl != NULL)
      {
	bgm.LastIndex[bgm.level] = prl ->LastIndex;
	r2 = prl -> relPtr;
	bgm.paths[bgm.level] = prl -> rm;

	IF projres
	THEN
	  IF useGamma(r1,r2) THEN return(1); ENDIF
	ELSE
	 ++table[bgm.paths[0]->init][bgm.paths[bgm.level]->term][bgm.level+2];

	  IF BuildNext(f1,r2,maxlevel,projres) THEN return(1); ENDIF
	ENDIF

	if(f1 != stdout && f1 != NULL) fShowGamma(f1,&bgm);

	prltemp = prl;
	prl = prl -> next;
	freePlist(&prltemp);
      }
    }
    --bgm.level;
  ENDIF

  return(0);
}
/* ************************************************************* */

#ifdef NOPROTO
static maskGen()
#else
static void maskGen(void)
#endif
{
  int i,j,k,l,ind1,ind2;

  mask[0] = 1;

  for(i = 1;i <= 31;++i) mask[i] = (mask[i-1] * 2);

  EndMask[31] = mask[31];

  for(i = 30; i >= 0; i--) EndMask[i] = EndMask[i+1] + mask[i];

  return;
}
/* ************************************************************* */
#ifndef NOPROTO
void aRecoRecRestart(void);
#endif
/* ************************************************************* */

#ifdef NOPROTO
int BuildGammas(projres)
int projres;
#else
int BuildGammas(int projres)
#endif
/*
  Builds Gammas, then super reduces the results putting paths in
  matrix2.  
*/
{
  RELATION *r1;
  RELMEM *rm1;
  int *addr,i,mult,level,maxlevel,numRels=0;
  char chr,name2[120],*name;
  MEMADJLST *mal;
  gaget *g1,*g2,*gbd;
  FILE *f1;
  unsigned index,tmaxlength = maxlength;

  IF !projres
  THEN
    name = ".gam";
    chr = 'w';
    if(getfile(&f1,nameb,name2,name,chr)) return(2);
  ELSE
    f1 = stdout;
  ENDIF

  maskGen();

  index = 0;
  r1 = firstR;
  while(r1 != NULL) 
  {
    if(r1->pmt==1) r1-> index = ++index;
    else r1 -> index = 0;
    r1 = r1 -> next;
  }

  maxlength = 2 * maxlength - 1;

  if(!tr4->RMgroups)
  IF expandTempRel(tr4,1) THEN return(1); ENDIF

  if(!tr5->RMgroups)
  IF expandTempRel(tr5,1) THEN return(1); ENDIF

  aRecoRecRestart();

  IF FindOverlaps(projres)
  THEN
    PRINTF(" problem in FindOverlaps.\n");
    return(1);
  ENDIF

  IF projres
  THEN
    maxlevel = 3;
  ELSE 
    PRINTF(" How many levels of gammas should be generated?\n");
    PRINTF(" Enter an integer between 3 and %d.\n",MAXLEVEL);
    PRINTF(" The program is aborted when an out of range value is given.\n");
    maxlevel = getint();
    IF maxlevel < 3 || maxlevel > MAXLEVEL THEN return(0); ENDIF
  ENDIF

  maxlevel -= 2;         /* adjust maxlevel for Ed. */

  r1 = firstR;

  while(r1 != NULL)
  {
    if(r1->pmt == 1) ++numRels;
    r1 = r1 -> next;
  }

  matrix2 -> numRows = 0;
  matrix2 -> numCols = numRels;

  for(i=1;i<MAXLEVEL;++i) bgm.LastIndex[i]  = -1;

  bgm.level = 0;
  bgm.LastIndex[0] = 0;


  r1 = firstR;

  while (r1 != NULL)
  {
    IF  r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

    bgm.paths[0] = r1->mem[0];

    IF BuildNext(f1,r1,maxlevel,projres)
    THEN
      PRINTF(" Problems in BuildNext. \n");
      return(1);
    ENDIF

    r1 = r1 -> next;
  }

  while(r1 != NULL)
  {
    r1 -> overPtr = NULL;
    r1 = r1 -> next;
  }

  if(!projres) PRINTF(" The gammas are in %s.\n",name2);
  maxlength = tmaxlength;
  return(0);
}
/* ************************************************************* */

#ifdef NOPROTO
int projectiveResolution(repnum)              
int repnum;
#else
int projectiveResolution(int repnum)              
#endif
{
    int index,i,maxNumberReps,numberReps,repeat = 0,numrows;
    int thomogeneous = homogeneous;
    char chr = 'w';
    struct MemAdjLst *temp;
    RMATRIX *tempM;
    RELATION *r1;
    FILE *f1,*f2;
    char name2[120],*name = "mat",*n11;


#ifdef DEBUG
    n11 = "grdebug.dmp";
    f2 = fopen(n11,"w");
    fclose(f2);
#endif

    PRINTF(" How many repetitions would you like?\n");
    maxNumberReps = getint();

    if ((i=getMatrix(repnum))!=0) return(i);

    name = ".res";
    if(getfile(&f1,nameb,name2,name,chr)) return(2);

    homogeneous = 0;

    FPRINTF(f1," input matrix for projective resolution #%d\n",repnum+1);
    fShowRmatrix(f1,matrix1);
    FPRINTF(f1," \n\n");
    fclose(f1);

    IF removeVertexes(1) THEN return(1); ENDIF


    ++maxlength;
    numberReps = 1;

    loop:

    if (useMatrx()) {PRINTF("problems in useMatrx.\n");goto out;} /* ok */

#ifdef DEBUG
    f2 = fopen(n11,"a");
    FPRINTF(f2,"\n\n Matrix1 at begin repetition #%d \n",numberReps);
    fShowRmatrix(f2,matrix1);
#endif

    IF RelFromMatrix(matrix1) THEN return(1); ENDIF

#ifdef DEBUG
    fprintf(f2," RelFromMatrix\n");

    r1 = firstR;
    while(r1 != NULL)
    {
      if(r1-> pmt == 1) fShowRelation(f2,r1,0);
      r1 = r1 -> next;
    }
    fprintf(f2," \n\n");
#endif

    numrows = matrix1 -> numRows;
    clearRmatrix(matrix1);

    if (getBasis(1)) {PRINTF("problems in getBasis\n");return(1);}

#ifdef DEBUG
    fprintf(f2," Basis\n");

    r1 = firstR;
    while(r1 != NULL)
    {
      if(r1-> pmt == 1) fShowRelation(f2,r1,0);
      r1 = r1 -> next;
    }
    fprintf(f2," \n\n");
    fclose(f2);
#endif


    if(RelToMatrix(matrix1,numrows)) return(1);

#ifdef DEBUG    
    f2 = fopen(n11,"a");
    FPRINTF(f2,"\n\n matrix1 after RelToMatrix rep #%d \n",numberReps);
    fShowRmatrix(f2,matrix1);
    fclose(f2);
#endif

    if (BuildGammas(1)) return(1);

    transpose(&matrix2,&tmatrix);

#ifdef DEBUG    
    f2 = fopen(n11,"a");
    FPRINTF(f2,"\n\n Matrix2 #%d \n",numberReps);
    fShowRmatrix(f2,matrix2);
    fclose(f2);
#endif

    IF (i=NullMatrix()) == 1 THEN return(0); ENDIF

#ifdef DEBUG
    printf(" after BuildGammas\n");
    fShowRmatrix(stdout,matrix1);
    fShowRmatrix(stdout,matrix2);
    printf(" \n\n");
#endif

    IF removeVertexes(0) THEN return(1); ENDIF

#ifdef DEBUG
    printf(" after removeVertexes\n");
    printf(" matrix1 \n");
    fShowRmatrix(stdout,matrix1);

    printf(" matrix2 \n");
    fShowRmatrix(stdout,matrix2);
    printf(" \n\n");
#endif

    IF (i=NullMatrix()) == 1 THEN return(0); ENDIF

    IF  i != 0
    THEN
      PRINTF("\n\n 2 !!!!! The matrix product is not null. \n");
      PRINTF(" Program aborted. \n");
      return(1);
    ENDIF

    IF matrix1->numRows && matrix1->numCols
    THEN
      f1 = fopen(name2,"a");
      FPRINTF(f1,"\n\n Matrix1 at repetition #%d \n",numberReps);
      fShowRmatrix(f1,matrix1);
      fclose(f1);
      PRINTF(" repetition #%d \n",numberReps);
    ENDIF

    repeat = matrix2 -> numRows * matrix2 -> numCols != 0;

    if (cleanUp()){PRINTF("problems in cleanUp.\n");return(1); }

    IF repeat
    THEN
      numberReps++;
      if(numberReps <= maxNumberReps) goto loop;
    ENDIF

    out:

    clearRmatrix(matrix1);

    --maxlength;

    pause();
    homogeneous = thomogeneous;
    return(0);
}

