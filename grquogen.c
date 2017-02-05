/*
*****************************************************************
*******
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward
Green, *
 * Virginia Polytechnic Institute and State University            
       *

*****************************************************************
*********/
#include "stdlib.h"
#include "math.h"
#include "grs.h"
#include "grglobal.h"

typedef struct 
{
  unsigned length;
  int term;
  gaget gg[256];
}  PP;
/* ************************************************************* */

static double MaxInt;

#ifdef NOPROTO
static GetMaxInt()
#else
static void GetMaxInt(void)
#endif
{
  double xx,temp;
  int done,i,n;

  xx = 1;
  for(i=0;i<40;++i) xx = xx * 2;

  MaxInt = xx-1;
}

#ifdef NOPROTO
static int rowSum(rowI, rowJ, i)
double *rowI, *rowJ;
int i;
#else
static int rowSum(double *rowI,double *rowJ,int i)
#endif
{
  int j;
  double multI,multJ,temp1,temp2;

  multI = *(rowJ + i);
  multJ = *(rowI + i);

  for(j=i;j<numberVertexes;++j)
  {
    *(rowJ + j) = (temp1 = multJ * (*(rowJ + j)))
            - (temp2 = multI * (*(rowI + j)));
    IF temp1 > MaxInt || temp1 < -MaxInt ||temp2 > MaxInt || temp2
< -MaxInt
       || *(rowJ + j) < -MaxInt || *(rowJ + j) > MaxInt
    THEN
      return(1);
    ENDIF
  }

  return(0);
}

#ifdef NOPROTO
static determinant(f1)
FILE *f1;
#else
static void determinant(FILE *f1)
#endif
{
  int i,j,k,bestj,ch;
  double *tempRow,det=1;
  double bestentry;

  GetMaxInt();

  FPRINTF(f1," Cartan Matrix \n\n");

  for(i=0;i<numberVertexes;++i)
  {
    for(j=0;j<numberVertexes;++j) FPRINTF(f1," %d",(int)
ppmatrix[i][j]);
    FPRINTF(f1,"\n");
  }

  for(i=0;i<numberVertexes;++i) rows[i] = &(ppmatrix[i][0]);

  for(i=0;i<numberVertexes-1;++i)
  {
/* put row whose j th entry has smallest absolute value > 1 next */

    bestj = -1;

    for(j=i;j<numberVertexes;++j)
      if ((rows[j][i])!=0)
      IF bestj == -1
      THEN
     bestj = j;
     if(rows[j][i]>=0)
     bestentry = (rows[j][i]);
     else bestentry = -rows[j][i]; 
      ELSE
     IF (rows[j][i]) < bestentry && -rows[j][i]<bestentry
     THEN
       bestj = j;
      if(rows[j][i]>=0) bestentry = (rows[j][i]); 
      else bestentry = -rows[j][i];
     ENDIF
      ENDIF

    IF bestj == -1 THEN FPRINTF(f1," det = 0 \n"); goto out; ENDIF

    IF bestj != i
    THEN
      det = -1 * det;
      tempRow = rows[i];
      rows[i] = rows[bestj];
      rows[bestj] = tempRow;
    ENDIF
/*
    Subtract multiple of i th row from each following row.
    To keep all entries integers row j is first multiplied by first
element
    in row i.
*/
    for(j = i + 1; j < numberVertexes; ++j)
    {
      IF rowSum(rows[i],rows[j],i)
      THEN
     FPRINTF(f1," Integer exceeds MaxInt in rowSum. \n");
     goto out;
      ENDIF
    }
  }

  for(i= 0;i<numberVertexes;++i) det = det * rows[i][i];

  IF det > MaxInt || det < -MaxInt
  THEN
    FPRINTF(f1," Integer exceeds MaxInt in forming determinant.\n");
    goto out;
  ENDIF

  for(i= 0;i<numberVertexes;++i)
  {
    for(j = i+1; j < numberVertexes; ++j) det =  (det/rows[i][i]);
  }
  FPRINTF(f1,"\n\n det of Cartan matrix = %g \n",(double) det);
  out:

  return;
}/* *************************************************************
*/
#ifdef NOPROTO
static int ExtendPath(f1,pp, init,root, projres, shwpths)
FILE *f1;
PP *pp;
int init;
PATH **root;
int projres, shwpths;
#else
static int ExtendPath(FILE *f1,PP *pp,int init,PATH **root,
                        int projres,int shwpths)
#endif
{
  MEMADJLST *mal;
  int pos,init1,length;
  RELATION *r1;
  RELMEM *rm1;
  PATH *p1;
  gaget *g1,*g2,*gPtrBd;

  pos = 256 - (length = (int) (++(pp->length)));

  IF length > (int) maxlength+1 THEN goto out; ENDIF
  IF NoFallBack && length == (int) maxlength + 1 THEN goto out;
ENDIF

  mal = length != ((int) maxlength + 1) || !projres
                      ?rAdjLst[init]:rpAdjLst[init];

  while(mal != NULL)
  {
    pp -> gg[pos] = mal -> EdgeNum;

    IF !seekSubstring((pp->gg+pos),(unsigned) length,*root,&p1)
    THEN
      init1 = mal->term;
      IF length != (int) maxlength + 1
      THEN
     ++tab[init1][pp->term][length];
     IF shwpths
     THEN
       FPRINTF(f1," ");
       fShowPath(f1,pp->gg+pos,(int) pp->length,1);
     ENDIF
      ELSE
     IF !NoFallBack 
     THEN
       rm1 = (r1 = &tr0->rel) ->mem[0];

       r1 -> pmt = 1;
       r1 -> length = 1;
       r1 -> creationTime = rtime;

       rm1 -> coef = 1;
       rm1 -> pathlength = (unsigned) length;
       rm1 -> term = pp -> term;
       rm1 -> init = init1;

       g2 = rm1 -> edgelist;
       g1 = (gPtrBd = pp->gg + 256) - length;
       while(g1 < gPtrBd) *g2++ = *g1++;

       IF storeRel(tr0,root) THEN return(1); ENDIF
       IF enterTip(root) THEN return(1); ENDIF
     ENDIF
      ENDIF

      IF ExtendPath(f1,pp,init1,root,projres,shwpths) THEN
return(1); ENDIF
    ENDIF
    mal = mal -> Next;
  }

  out:
  --(pp->length);
  return(0);
}
/* ************************************************************* */
#ifndef NOPROTO
void freeALmems(MEMADJLST **temp);
#endif
/* ************************************************************* */
#ifdef NOPROTO
int QuotientBasis(root,projres)
PATH **root;
int projres;
#else
int QuotientBasis(PATH **root,int projres)
#endif
/* 
   calculates the basis of paths for the Ideal mod the
   Groebner basis.  returns 1 if there are problems with
   memory otherwise 0.  Also finds the paths of length maxlength +1
   used in extra relations.
*/
{
  PP pp;
  int i,j,k,shpths=0;
  unsigned int totalnumberpaths;
  FILE *f1;
  MEMADJLST *temp;
  char name2[120],*name=".tab",ch='w';

  unsigned tempmaxlength = maxlength;

  IF !projres
  THEN
    PRINTF("\n Should the basis paths for the algebra be saved? y or n\n");
    ch = 'a';
    while(ch != 'n' && ch != 'y') ch = GETCH();
    shpths = ch == 'y';
    IF shpths== 'n' && NoFallBack THEN return(0); ENDIF

    ch = 'w';
    if(getfile(&f1,namea,name2,name,ch)) return(2);

    if(NoFallBack) maxlength = maxlength/2;

    for(i=0; i < numberVertexes;++i)
    for(j=0; j < numberVertexes;++j)
    for(k=0; k < (int) maxlength + 1;++k) tab[i][j][k] = 0;

    IF f1 == NULL
    THEN
      PRINTF(" %s can not be opened. ",name2);
      return(1);
    ENDIF
  ENDIF

  pp.length = 0;

  for (i=0; i< numberVertexes; ++i)
  {
     tab[i][i][0] = 1;
     pp.term = i;

     if(!projres) FPRINTF(f1," (%d)\n",i+1);

     IF ExtendPath(f1,&pp,i,root,projres,shpths) THEN return(1);
ENDIF
  }

  IF !projres
  THEN
    if(shpths)
    {
      FPRINTF(f1,"\n\n");
      PRINTF("\n The generating paths are in %s.\n",name2);
    }

    for(k = 0; k < numberVertexes; ++k)
    {
      FPRINTF(f1,"\n\n Initial Vertex = %2d \n\n",k+1);
      FPRINTF(f1," term vertexes ");
      for(j = 0; j < numberVertexes; ++j) FPRINTF(f1," %3d ",j +
1);
      FPRINTF(f1,"\n");
      FPRINTF(f1," length      \n");

      for(i = 0; i < (int) maxlength + 1; ++i)
      { FPRINTF(f1,"%10d     ",i);
     for(j = 0; j < numberVertexes; ++j) FPRINTF(f1," %3u ",tab[k][j][i]);
     FPRINTF(f1,"\n");

      }
    }

    totalnumberpaths = 0;
    for(i=0;i < numberVertexes;++i)
    for(j=0;j < numberVertexes;++j)
    for(k=0;k<= (int) maxlength;++k) totalnumberpaths +=
tab[i][j][k];

    for(i=0;i < numberVertexes;++i)
    for(j=0;j < numberVertexes;++j) ppmatrix[i][j] = 0;

    for(i=0;i < numberVertexes;++i)
    for(j=0;j < numberVertexes;++j)
    for(k=0;k<= (int) maxlength;++k) ppmatrix[i][j] +=
tab[i][j][k];

    determinant(f1);

    FPRINTF(f1,"\n\n total number generators = %d.\n",totalnumberpaths);
    fclose(f1);

    PRINTF("\n The numbers of generators are in %s. \n",name2);
  ENDIF

  maxlength = tempmaxlength;

  IF projres
  THEN
    for(i=0;i<numberVertexes;++i)
    {
      while((temp = rpAdjLst[i]) != NULL)
      {
     rpAdjLst[i] = temp -> Next;
     freeALmems(&temp);
      }
    }
  ENDIF

  return(0);
}
/* ************************************************************* */
