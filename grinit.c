/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
/* GRINIT.C */
#include "stdlib.h"
#include "conio.h"
#include "ctype.h"

#include "grs.h"
#include "grglobal.h"

/* ***************************   PROTOTYPES   *********************** */
#ifndef NOPROTO
int InitMem(void);
int getALMems(MEMADJLST **temp,int ii);
#endif

/* *************************************************************  */
#ifdef NOPROTO
int getshifter()
#else
int getshifter(void)
#endif
{
  unsigned int temp = 1;
  SHIFT = MASK = 0;

  while(temp < MAXRMLST)
  {
    MASK += temp;
    ++SHIFT;
    temp *= 2;
  }

  IF temp != MAXRMLST
  THEN
    PRINTF("\n MAXRMLST must be a power of two, program aborted.\n\n");
    return(1);
  ENDIF

  return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
void getConstants()
#else
void getConstants(void)
#endif
{
  long ttemp;
  char ch,*ptr1,*ptra,*ptrb;
  int i,bad;


  nextc:

  PRINTF(" Enter characteristic. (A prime.)\n");
  ttemp = getint();

  IF ttemp <= 32767 && ttemp > 1
  THEN
    prime = (int) ttemp;
  ELSE
    PRINTF(" \n\n Characteristic must greater than 1 and less than %d. \n "
			,32767);
    goto nextc;
  ENDIF

  bad = prime > 2 && (prime % 2) == 0;

  i = 3;

  while(!bad && i < prime)
  {
    bad = (prime % i) == 0;
    i += 2;
  }

  IF bad
  THEN
    PRINTF("\n\n Your characteristic must be a prime. Try again.\n");
    goto nextc;
  ENDIF

  nextmax:

  PRINTF("\n Enter maximum path length. \n\n");
  PRINTF(" 1 <= maximum path length <= 255\n");

  ttemp = getint();

  IF ttemp <= 255 && ttemp > 0
  THEN
    maxlength = (unsigned) ttemp;
  ELSE
    PRINTF("\n\n Maximum path length out of range. \n\n");
    goto nextmax;
  ENDIF

  PRINTF("\n");

  PRINTF(" Press d if your problem name is the default name gr.\n");
  PRINTF(" Press e to enter the problem name.\n");

  ch = 'd';

  ch = 'a';

  while((ch = (char) GETCH()) != 'd' && ch != 'e') doNothing;

  IF ch == 'd'
  THEN
    name1[0] = 'g';
    name1[1] = 'r';
    name1[2] = '\0';
    namea[0] = 'g';
    namea[1] = 'r';
    namea[2] = '\0';
    nameb[0] = 'g';
    nameb[1] = 'r';
    nameb[2] = '\0';
  ELSE
    PRINTF("\n Enter problem name.\n");
    newtry:
    (void) gets(name1);
    if (!isalnum(name1[0])) goto newtry;
    ptr1 = name1;ptra = namea; ptrb = nameb;

    do
     *ptra++ = *ptrb++ = *ptr1;
    while(*ptr1++ != '\0');
  ENDIF

  PRINTF("\n");


  PRINTF(" Press f if your problem name descriptor is fixed.\n");
  PRINTF(" Press o otherwise.\n");
  ch = 'a';
  while((ch = (char) GETCH()) != 'f' && ch != 'o') doNothing;
  automatic = ch == 'f';

  PRINTF("\n");

  PRINTF(" Press f if you want fallback.\n");
  PRINTF(" Press o otherwise.\n");
  ch = 'a';
  while((ch = (char) GETCH()) != 'f' && ch != 'o') doNothing;
  NoFallBack = ch == 'o';
  IF NoFallBack THEN maxlength *= 2; ENDIF

  return;
}
/* *************************************************************  */
#ifdef NOPROTO
void ShowEdges()
#else
static void ShowEdges(void)
#endif
{
  int i,ch;
  EDGE *tem;

  PRINTF(" The edges are ordered according to their indexes, i.e. the \n");
  PRINTF(" edge with the largest index is largest.\n");
  PRINTF(" index   init   term  char\n");

  for(i=0;i<numberEdges;)
  {
    tem = &EdgeList[i];
    printf("   %3d    %3d    %3d    %c\n",
	   tem->iname+1,tem->init+1,tem->term+1,tem->name);

    ++i;
  }

  return;
}
/* *************************************************************  */
#ifdef NOPROTO
static int MakeEdges()
#else
static int MakeEdges(void)
#endif
/*
  constructs edges and adjacency lists
*/
{
  int i,j,k,i1,i2,index;
  EDGE tempE;
  gaget chr;
  MEMADJLST *temp;
  int indexes[MaxNumEdges];

  for (i = 0; i <= numberVertexes; ++i)
  {
   AdjLst[i] = NULL;
   rAdjLst[i] = NULL;
   rpAdjLst[i] = NULL;
  }

  index = -1;
  chr = 'a'-1;

  for(i = 0; i < numberVertexes+1;++i)
  for(j = 0; j < numberVertexes+1;++j)
  {
      for (k = 0;k < matrix[i][j]; ++k)
      {
	if (++chr=='z'+1) {chr='A';}

	IF ++index > MaxNumEdges
	THEN
	  PRINTF(" Too many edges, program aborted \n");
	  return(1);
	ENDIF

	if (index > 51) chr = '#';

	EdgeList[index].init = i;
	EdgeList[index].term = j;
	EdgeList[index].iname = index;
	EdgeList[index].name = (char) chr;
       }
   }

   interchange:

   ShowEdges();

   PRINTF("\n If you wish to\n");
   PRINTF("   a. interchange the indexes of a pair of edges, press a\n");
   PRINTF("   b. completely reorder the indexes, press b\n");
   PRINTF("   n. do nothing, press n. \n");

   chr ='0';

   while( (chr = GETCH()) != 'a' && chr != 'b' && chr != 'n') doNothing;

   IF chr == 'a'
   THEN
     try1:
     PRINTF(" Enter first index from 1 to %d.\n ",numberEdges);

     IF (i1 = getint()) < 1 || i1 > numberEdges
     THEN
       PRINTF(" Index %d out of range.\n",i1);
       goto try1;
     ENDIF

     try2:
     PRINTF(" Enter second index from 1 to %d but not %d.\n ",
	      numberEdges,i1);

     IF (i2 = getint()) < 1 || i2 > numberEdges || i1 == i2
     THEN
       PRINTF(" Index %d out of range or equal to %d.\n",i2,i1);
       goto try2;
     ELSE
       --i1;
       --i2;
     ENDIF

     tempE = EdgeList[i1];
     EdgeList[i1] = EdgeList[i2];
     EdgeList[i2] = tempE;
     EdgeList[i1].iname = i1;
     EdgeList[i2].iname = i2;

     goto interchange;
   ENDIF

   IF chr == 'b'
   THEN
     again:
     PRINTF("\n Enter the current indexes of all edges in the order that\n");
     PRINTF(" you want to be used from smallest to largest. \n");
     for(i=0;i<numberEdges;++i) indexes[i] = -1;
     for(i=0;i<numberEdges;++i)
     {
	j = getint();

	IF j < 1 || j > numberEdges
	THEN
	  PRINTF(" Index out of range, you must start again ");
	  goto again;
	ENDIF

	IF indexes[j-1] == -1
	THEN
	  indexes[j-1] = i;
	ELSE
	  PRINTF(" Index %d has been used twice, please start again.\n",j);
	  goto again;
	ENDIF
     };

     for(i=0; i<numberEdges;++i)
     {
       IF indexes[i] != -1
       THEN
	 i2 = indexes[i];
	 tempE = EdgeList[(i1 = i)];
	 indexes[i1] = -1;

	 IF i1 != i2
	 THEN
	   while(i2 != i)
	   {
	     EdgeList[i1] = EdgeList[i2];
	     EdgeList[i1].iname = i1;

	     i1 = i2;
	     i2 = indexes[i2];
	     indexes[i1] = -1;
	   }

	   EdgeList[i1] = tempE;
	   EdgeList[i1].iname = i1;
	 ENDIF
       ENDIF
     }

     goto interchange;
   ENDIF



  IF getALMems(&temp,2*numberEdges)
  THEN
    printf("out of space\n");
    return(1);
  ENDIF

  for(i=0;i < numberEdges;++i)
  {
    temp->EdgeNum = i;
    temp->term = EdgeList[i].term;
    temp->Next = AdjLst[EdgeList[i].init];
    AdjLst[EdgeList[i].init] = temp++;

    temp->EdgeNum = i;
    temp->term = EdgeList[i].init;
    temp->Next = rAdjLst[EdgeList[i].term];
    rAdjLst[EdgeList[i].term] = temp++;
  }

   return (0);
 }
/* *************************************************************  */

int initialize()      /* reads in graph and initializes arrays */
{
  int ch,i,j,k,ii;
  char *name,name2[120];
  char ch1[256];
  FILE *f1;

  name = ".gph";

  TryAgain:

  IF !automatic
  THEN
    PRINTF("\n If your graph is in %s.gph, press d.\n",name1);
    PRINTF(" If your graph is in another file, press e.\n");
    PRINTF(" If you wish to abort the program, press q.\n");

    ch = 'a';
  ELSE
    ch = 'd';
  ENDIF

  while(ch != 'd' && ch != 'e' && ch != 'q') ch = GETCH();

  switch(ch)
  {
    case 'd' : if(getfile(&f1,name1,name2,name,'r')) goto TryAgain;
	       break;
    case 'e' : PRINTF(" Enter name of data file. \n\n");

	       newtry:
	       (void) gets(name2);
	       if (name2[0] == '\0') goto newtry;
	       f1 = fopen(name2,"r");
	       break;
    case 'q' : PRINTF(" Program aborted. \n");
	       return(2);
    default   : doNothing;
  }

  IF f1 == NULL
  THEN
    PRINTF(" %s does not exist \n",name2);
    automatic = 0;
    goto TryAgain;
  ENDIF

    k = fscanf(f1," %d",&numberVertexes);
    if(k != 1) { PRINTF(" The first number must be the number of vertexes.\n");
		 PRINTF(" Program aborted. \n");
		 return(2); }

    numberEdges = 0;

    IF InitMem() THEN return(1); ENDIF

    while( (ch = fgetc(f1)) != '\n') doNothing;

    for (i = 0;i<numberVertexes+1;++i)
    for (j = 0;j<numberVertexes+1;++j)
    {
	IF i < numberVertexes && j < numberVertexes
	THEN
	  k = fscanf(f1," %d",&ii);
	  if(k!=1) { PRINTF("\n Too few matrix elements \n");return(1);}
	  numberEdges += (matrix[i][j] = pmatrix[i][j] = (short) ii);
	ELSE
	  matrix[i][j] = pmatrix[i][j] = 0;
	ENDIF
     }

    IF fscanf(f1," %d",&k) >= 0
    THEN
      PRINTF(" There are too many matrix entries in %s .\n",name2);
      PRINTF(" Program aborted. \n");
      return(1);
    ENDIF

    fclose(f1);

    PRINTF("\n");
    PRINTF(" The matrix of the graph follows. \n\n");

    for(i = 0; i < numberVertexes; ++i)
    {
      for(j = 0; j < numberVertexes; ++j)
      {
	 PRINTF(" %d ",matrix[i][j]);
       }
       PRINTF("\n");
     }

    for (i = 0; i < MaxNumEdges; ++i) {
      EdgeList[i].iname = i; }

    IF MakeEdges() THEN return(1); ENDIF

    for(i = 0; i < 256; ++i) translator[i] = -1;

    for(i = 0; i < numberEdges; ++i)
       translator[EdgeList[i].name] = EdgeList[i].iname;

    return(0);
}
/* *************************************************************  */

#ifdef NOPROTO
int nonhomo(tr)
TEMPREL *tr;
#else
int nonhomo(TEMPREL *tr)
#endif
/*
  returns 1 if paths in relation have different lengths.
*/
{
  unsigned int index = 1,i;
  RELATION *r1;

  r1 = &tr->rel;

  i = r1 -> mem[0] -> pathlength;
  while(index < r1 -> length)
  {
    if((r1 ->mem[index>>SHIFT]+(index&MASK))->pathlength!= i) return(1);
    ++index;
  }
  return(0);
}
/* *************************************************************  */

int initializeR() /* Reads and stores relations */
{
  int ch,i,j,done,length,firstv,lastv;
  unsigned index;
  char *name,*name3,name2[120];
  char ch1[256],*ptra,*ptrb;
  FILE *f1;
  RELATION *tempR;
  RELMEM   *tempRM,**addrRM;
  PATH    *tempP,*pathPtr;


    homogeneous = 1;

    rMark = 4;

    name = ".rel";

    TryAgain:

    IF !automatic
    THEN
	 PRINTF("\n If your relations are in %s.rel, press d.\n",namea);
	 PRINTF(" If your relations are in grdump.rel, press p.\n");
	 PRINTF(" If your relations are in another file, press e.\n");
	 PRINTF(" If you wish to enter a new descriptor, press n.\n");
	 PRINTF(" If you wish to abort the program, press q.\n");

      ch = 'a';
    ELSE
	 ch = 'd';
    ENDIF

    while(ch!='d' && ch!='p' && ch!='e' && ch!='n'&& ch!='q') ch=GETCH();

    switch(ch)
    {
	 case 'd' : if(getfile(&f1,namea,name2,name,'r'))goto TryAgain;
		 break;
	 case 'p' : name3  = "grdump";
		 if(getfile(&f1,name3,name2,name,'r'))goto TryAgain;
		 break;
	 case 'e' : PRINTF(" Enter name of data file. \n\n");
		 newtry:
		 (void) gets(name2);
		 if (name2[0] == '\0') goto newtry;
		 f1 = fopen(name2,"r");
		 break;
	 case 'n' : PRINTF(" Enter relation descriptor. \n\n");
		 nextTry:
		 (void) gets(namea);
		 if(namea[0] == '\0') goto nextTry;
		 if(getfile(&f1,namea,name2,name,'r'))goto TryAgain;

		 ptra = namea; ptrb = nameb;

		 do
		   *ptrb++ = *ptra;
		 while(*ptra++ != '\0');

		 automatic = 1;
		 break;
	 case 'q' : PRINTF(" Program aborted. \n");
		 return(2);
      default  : doNothing;
    }

    IF f1 == NULL
    THEN
      PRINTF(" %s does not exist \n",name2);
      automatic = 0;
      goto TryAgain;
    ENDIF

    done = 0;

    while(!done)
    {
      while(isspace(ch = getc(f1))) doNothing;

      IF ch == EOF
      THEN
	PRINTF("\n All elements in this file have now been entered. \n");
	goto out;
      ELSE
	IF ungetc(ch,f1) !=ch THEN PRINTF(" Trouble with ungetc.\n");ENDIF
      ENDIF

      IF GetRelation(f1,tr0,1) == 1 THEN return(1); ENDIF;

      Reorder(tr0);

      firstv = (tempRM = (tempR = &tr0->rel) -> mem[0]) -> init;
      lastv = tempRM -> term;

      index = 0;

      while (++index < tr0->rel.length)
      {
	tempRM = tempR -> mem[index>>SHIFT]+(index&MASK);

	IF (firstv != tempRM->init)||(lastv != tempRM->term)
	THEN
	  PRINTF(" Relation has two distinct init or term vertexes.\n");
	  return(1);
	ENDIF
      }

      IF (j = BackReduce(&tr0->rel,0)) == 1
      THEN
	return(1);
      ELSE
	trtemp = j == 2 ? tr1: tr0;

	if(trtemp->rel.length)
	IF newRelation(&rootPath,trtemp) THEN return(1); ENDIF
	if(nonhomo(trtemp)) homogeneous = 0;
      ENDIF
    }

    out:
    fclose(f1);
    return(0);
}

/* *************************************************************  */

#ifdef NOPROTO
int getMatrix(repnum)
int repnum;
#else
int getMatrix(int repnum)
#endif
{
  int ch,firstv= -1,lastv= -1,k,ii,jj,index1,index2;
  unsigned i,j,index;
  char *name,name2[120];
  FILE *f1;
  RELATION *r;
  RELMEM *tempRM;

  name = ".mod";

  TryAgain:

  IF !automatic || repnum
  THEN
    PRINTF(" If your matrix is in %s.mod, press d.\n",nameb);
    PRINTF(" If your matrix is in another file, press e.\n");
    PRINTF(" If you wish to give a new module descriptor, press n.\n");
    PRINTF(" If you wish to abort the resolution subroutine, press q.\n");

    ch = 'a';
  ELSE
    ch = 'd';
  ENDIF

  while(ch != 'd' && ch != 'e' && ch != 'n' && ch != 'q') ch = GETCH();

  switch(ch)
  {
    case 'd' : if(getfile(&f1,nameb,name2,name,'r'))goto TryAgain;
	       break;
    case 'e' : PRINTF(" Enter name of data file. \n\n");

	       newtry:
	       (void) gets(name2);
	       if (name2[0] == '\0') goto newtry;
	       f1 = fopen(name2,"r");
	       break;
    case 'n' : PRINTF(" Enter name of module descriptor. \n\n");
	       retry:
	       (void) gets(nameb);
	       if (nameb[0] == '\0') goto retry;
	       if(getfile(&f1,nameb,name2,name,'r'))goto TryAgain;
	       break;
    case 'q' : PRINTF(" Program aborted. \n");
	       return(2);
    default   : doNothing;
  }

  IF f1 == NULL
  THEN
    PRINTF(" %s does not exist \n",name2);
    automatic = 0;
    goto TryAgain;
  ENDIF

  IF 2 != fscanf(f1,"%d %d",&matrix1->numRows,&matrix1->numCols)
  THEN
    PRINTF("\n The first two entries in %s must be the \n",name);
    PRINTF(" numbers of rows and columns.  Program aborted. \n");
    return(1);
  ENDIF

  PRINTF(" Number of rows    = %d\n",matrix1->numRows);
  PRINTF(" Number of columns = %d\n",matrix1->numCols);

  IF 1 > matrix1 -> numRows || 1 > matrix1 -> numCols
  THEN
    PRINTF(" This matrix is trivial. Program aborted.\n\n");
    return(1);
  ENDIF

  IF matrix1 -> numRows * matrix1 -> numCols > MAXMATRIXSIZE
  THEN
    PRINTF(" This matrix is too large. Program aborted.\n\n");
    return(1);
  ENDIF

  for(ii=0; ii < matrix1->numRows; ++ii)
  for(jj=0; jj < matrix1->numCols; ++jj)
  {
    IF GetRelation(f1,tr0,0)
    THEN
      PRINTF("\n\n An invalid relation has been entered in getMatrix.\n");
      PRINTF(" Program aborted. \n\n");
      return(1);
    ENDIF

    r = &tr0->rel;

    IF r != NULL
    THEN
      index = 0;
      firstv = lastv = -1;

/* The paths in a relation must have a common init and term point. */

      while (index < r -> length)
      {
	tempRM = r -> mem[index>>SHIFT] + (index&MASK);

	IF firstv == -1 THEN
	  firstv = tempRM->init;
	  lastv  = tempRM->term;
	ELSE
	  IF (firstv != tempRM->init)||(lastv != tempRM->term)
	  THEN
	    PRINTF(" Relation has two distinct init or term vertexes.\n");
	    return(1);
	  ENDIF
	ENDIF

	++index;
      }
    ENDIF

    Reorder(tr0);
    IF storeRel(tr0,&PRrootPath) THEN return(1); ENDIF
    lastR -> pmt = 5;
    index2 = ii * matrix1->numCols + jj;
    matrix1->entry[index2] = lastR;
    lastR -> usedAt = &matrix1 -> entry[index];
  }

/* Relations in a row must have the same initial point. */

  for(ii=0; ii < matrix1->numRows; ++ii)
  {
    firstv = -1;

    for(jj=0; jj < matrix1->numCols; ++jj)
    {
       index2 = ii * matrix1->numCols + jj;

       if( (r = matrix1->entry[index2]) == NULL) continue;
       if (!r -> length) continue;

       k = r -> mem[0] -> init;
       IF firstv != -1
       THEN
	 IF k != firstv
	 THEN
	   PRINTF(" Initial vertexes in row %d are not consistent.\n",ii+1);
	   return(1);
	 ENDIF
       ELSE
	 firstv = k;
       ENDIF
    }
  }

/* Relations in a column must have the same terminal point. */

  for(jj=0; jj < matrix1->numCols; ++jj)
  {
    lastv = -1;

    for(ii=0; ii < matrix1->numRows; ++ii)
    {
       index2 = ii * matrix1->numCols + jj;

       if((r = matrix1->entry[index2]) == NULL) continue;
       if(!r->length) continue;

       k = r->mem[0] -> term;

       IF lastv != -1
       THEN
	 IF k != lastv
	 THEN
	   PRINTF(" Terminal vertexes in column %d not consistent.\n",jj+1);
	   return(1);
	 ENDIF
       ELSE
	 lastv = k;
       ENDIF
    }
  }

  fclose(f1);
  return(0);
}
/* ************************************************************* */

