/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"

#include "conio.h"
#include "stdlib.h"
#include "io.h"
#include "ctype.h"
#include "string.h"
#include "dos.h"
/* ***************************************************************** */
#ifdef NOPROTO
static int FileExists(name)
char *name;
#else
static int FileExists(char *name)
#endif
{
   return(access(name,0) == 0);
}
/* ************************************************************** */
#ifdef NOPROTO
void pause()
#else
void pause(void)
#endif
{
  int ch;
  ch = 'a';
  PRINTF(" Enter 1 to continue.\n");
  while(ch != '1') ch = GETCH();
  return;
}
/* ************************************************************** */
#ifdef NOPROTO
int getfile(f1,name,name2,fext,mode)
FILE **f1;
char name[120], name2[120], *fext, mode;
#else
int getfile(FILE **f1,char name[120],char name2[120],char *fext,char mode)
#endif
{
    char *ch;

    strcpy(name2,name);
    strcat(name2,fext);

    IF mode == 'r'
    THEN
      IF !FileExists(name2)
      THEN
	PRINTF(" %s does not exist \n",name2);
	automatic = 0;
	return(1);
      ENDIF

      ch = "r";
    ELSE
      if(mode == 'a') ch = "a";
      else if(mode == 'w') ch = "w";
    ENDIF

    *f1 = fopen(name2,ch);

    if ((*f1) == NULL) { PRINTF("Cannot open %s\n",name2); return(1);}

    return(0);
}
/* ************************************************************** */
#ifdef NOPROTO
static int gLessThan(g1,g2,length)
gaget *g1, *g2;
unsigned length;
#else
static int gLessThan(gaget *g1, gaget *g2, unsigned length)
#endif
/*
  returns 1 if gaget string g1 < gaget string g2,
  returns 2 if gaget string g1 > gaget string g2,
  returns 0 otherwise.
*/
{
  gaget *g1p;
  g1p = g1 + length;
  while(g1 < g1p) if(*g1++ != *g2++) return((*(g1-1)<*(g2-1))?1:2);
  return(0);
}

/* ************************************************************** */
#ifdef NOPROTO
int rmLessThan(rm1,rm2)
RELMEM *rm1, *rm2;
#else
int rmLessThan(RELMEM *rm1, RELMEM *rm2)
#endif
{
  unsigned length1,length2;

  IF (length1 = rm1 -> pathlength) !=  (length2 = rm2 -> pathlength)
  THEN
    return(length1 < length2 ? 1:2);
  ENDIF

  IF length1
  THEN
    return( gLessThan(rm1->edgelist,rm2->edgelist,length1) );
  ELSE
    return( (rm1->init < rm2->init) ? 1:(rm1->init > rm2->init)? 2:0);
  ENDIF
}

/* ************************************************************** */

#ifdef NOPROTO
int comparePaths(p1,p2)
PATH *p1, *p2;
#else
int comparePaths(PATH *p1,PATH *p2)
#endif
/* 
  length is the minimum of p1->length and p2->length.  If length is
  greater than 0, the first length elements of p1 and p2 are compared
  until a difference is detected. 1 is returned if this element of p1
  is less than the same element of p2. Otherwise 2 is returned.  If
  all elements are equal 0 is returned.
*/
{
  unsigned int length;
  gaget *g1,*g2;

  length = (p1 -> length < p2 -> length) ? p1 -> length:p2 -> length;

  IF length == 0
  THEN
    IF p1 -> init < p2 -> init
    THEN
      return(1);
    ELSE
      IF p1 -> init > p2 -> init
      THEN
	return(2);
      ELSE
	return(0);
      ENDIF
    ENDIF
  ENDIF

  for( g1=p1->edges,g2=p2->edges;g1 < p1->edges + length;++g1,++g2) 
  {
    IF *g1 != *g2 
    THEN
      IF *g1 < *g2
      THEN
	return(1);
      ELSE
	return(2);
      ENDIF
    ENDIF
  }

  return(0);
}

/* ************************************************************** */

#ifdef NOPROTO
int compareSubstring(ss,lengthS,pp)
gaget *ss;
unsigned lengthS;
PATH *pp;
#else
int compareSubstring(gaget *ss,unsigned lengthS,PATH *pp)
#endif

/*
  compares string ss to the path edges.  Returns 0 if identical as far
  as minimum of lengths, 1 if ss is less, and 2 if ss is greater.
  Length must be > 0.
*/
{
  unsigned int length;
  gaget *g1,*g2;

  IF (length = (lengthS < pp -> length) ? lengthS : pp -> length) == 0
  THEN
    IF *ss < MaxNumEdges
    THEN
      return(EdgeList[*ss].init<pp->init?1:EdgeList[*ss].init>pp->init?2:0);
    ELSE
      return(2);
    ENDIF
  ENDIF

  for( g1=ss,g2=pp->edges;g1 < ss + length;++g1,++g2) 
  {
    if (*g1 != *g2) return( (*g1 < *g2) ? 1:2);
  }

  return(0);
}

/* *****************************************************************  */
#ifdef NOPROTO
int pInverse(arg)
int arg;
#else
int pInverse(int arg)
#endif
{
  long power = arg,temp;
  unsigned int i = (unsigned) (prime - 2);

  if(arg==1) return(1);
  if(arg==-1) return(prime-1);

  if(arg == prime - 1) return(arg);

  repeat:
  i = (unsigned) (prime - 2);
  power = arg;
  temp = (1&i) ? arg:1;
  (i >>= 1);

  while(i)
  {
    power = power * power;
    power %= (long) prime;
    if(i&1) temp = (temp*power)%prime;
    (i >>= 1);
  }

  IF (i = (unsigned) ((temp * (long) arg)%prime)) != 1
  THEN
    goto repeat;
  ENDIF

  return( (temp >= 0)? (int) temp:(int) (temp + (long) prime));
}
/* ***************************************************************** */
#ifdef NOPROTO
int pMult(arg1,arg2)
int arg1,arg2;
#else
int pMult(int arg1,int arg2)
#endif

{
  long temp1,temp2;
  temp1 = (long) arg1;
  temp2 = (temp1 * (long) arg2)%prime;
  return( (temp2 < 0)?(int) (temp2+(long) prime):(int) temp2);
}
/* ***************************************************************** */

#ifdef NOPROTO
int pAdd(arg1,arg2)
int arg1,arg2;
#else
int pAdd(int arg1,int arg2)
#endif

{
  long temp;

  temp = (long) arg1;
  temp +=(long) arg2;
  temp %= (long) prime;
  return((temp>=0)?(int) temp:(int) (temp+prime));
}
/* ***************************************************************** */

#ifdef NOPROTO
static int pAdjust(arg)
int arg;
#else
static int pAdjust(int arg)
#endif
{
  arg %= prime;
  return((arg>=0)?arg:arg+prime);
}

/****************************************************************** */
#ifdef NOPROTO
int getint()
#else
int getint(void)
#endif
{
  char ctab[10];
  int i,ch,try=0;
  long j;

  tryagain:

  i = -1;

  while( (ch = GETCHAR()) != 10 )
  {
    IF i > -1 && !isdigit(ch) 
    THEN
      break;
    ENDIF

    IF (ch >= '0' && ch <= '9') || (ch == '-' && i == -1)
    THEN
      ++i;
      IF i < 7
      THEN
	ctab[i] = (char) ch;
      ELSE
	PRINTF(" Too many digits, try again. \n");
	goto tryagain;
      ENDIF
    ENDIF
  }

  IF i == -1
  THEN
    if(try++)PRINTF(" You still need an integer. \n");
    goto tryagain;
  ENDIF

  ctab[++i] = '\0';

  j = atol(ctab);

  IF j <= 32767 && j >= -32767
  THEN
    i = (int) j;
  ELSE
    PRINTF(" Integer too large, try again. ");
    goto tryagain;
  ENDIF

  return(i);

}
/* ***************************************************************** */
#ifdef NOPROTO
void fShowPath(f1,gPtr,length,ret)
FILE *f1;
gaget *gPtr;
int length,ret;
#else
void fShowPath(FILE *f1,gaget *gPtr,int length,int ret)
#endif

/* shows path, new line if ret <> 0 */

{
  int i;
  gaget *gPtr1;
  char ch;

    for (gPtr1 = gPtr;gPtr1 < gPtr + length;++gPtr1)
    {
      i = *gPtr1;

      IF i < 52
      THEN
	ch = EdgeList[i].name;
	FPRINTF(f1,"%c",ch);
      ELSE
	FPRINTF(f1,"[%d]",i-52);
      ENDIF
    }

  if(ret) FPRINTF(f1,"\n");
  return;
}

/* *************************************************************  */

#ifdef NOPROTO
void fShowRelation(f1,tempR,extraline)
FILE *f1;
RELATION *tempR;
int extraline;
#else
void fShowRelation(FILE *f1,RELATION *tempR,int extraline)
#endif
{
  unsigned int ind;
  int coef;
  gaget *gPtr;
  RELMEM *RM;

  IF tempR == NULL || !tempR -> length 
  THEN
    FPRINTF(f1," ; \n");
    IF extraline THEN FPRINTF(f1,"\n"); ENDIF
    return;
  ENDIF


  for(ind = 0; ind < tempR -> length; ++ind)
  {
     RM = &tempR -> mem[ind/MAXRMLST][ind%MAXRMLST];
     coef  = RM -> coef;
     coef = coef > (int) ((float) prime/2.0) ? coef - prime: coef;

     IF coef > 0
     THEN
       FPRINTF(f1," + %d * ",coef);
     ELSE
       FPRINTF(f1," - %d * ",- coef);
     ENDIF

     gPtr = RM -> edgelist;

     IF RM -> pathlength > 0
     THEN
       fShowPath(f1,gPtr,(int) RM->pathlength,0);
     ELSE
       FPRINTF(f1,"(%d)",1 + RM->init);
     ENDIF
  }

  FPRINTF(f1,";\n");
  IF extraline THEN FPRINTF(f1,"\n"); ENDIF
  return;
}

/* *************************************************************  */
#ifdef NOPROTO
static showRelSize(f1,index,root)
FILE *f1;
int index;
PATH *root;
#else
static void showRelSize(FILE *f1,int index,PATH *root)
#endif
{
  int extraline = 1;
  
  IF root == NULL THEN return; ENDIF

  showRelSize(f1,index,root->rchild);

  if((int) root->relptr->mem[0]->pathlength==index)
    fShowRelation(f1,root->relptr,extraline);
  
  showRelSize(f1,index,root->lchild);
  return;
}

/* *************************************************************  */

#ifdef NOPROTO
void fShowRmatrix(f1,mat)
FILE *f1;
RMATRIX *mat;
#else
void fShowRmatrix(FILE *f1,RMATRIX *mat)
#endif
{
  int i,j,index,ch;

  IF mat->numRows && mat-> numCols
  THEN
    for(i= 0; i < mat->numRows; ++i)
    {
      FPRINTF(f1,"row%d \n",i+1);

      for( j = 0; j < mat -> numCols; ++j)
      {
	index = i * mat->numCols + j;
	FPRINTF(f1," %d   ",j+1);
	fShowRelation(f1,mat->entry[index],0);
      }
    }
  ELSE
    FPRINTF(f1," The matrix is empty. ");
  ENDIF

  return;
}

/* *************************************************************  */

#ifdef NOPROTO
void fShowrelations(f1,root)
FILE *f1;
PATH *root;
#else
void fShowrelations(FILE *f1,PATH *root)
#endif
{
  RELATION *r1;
  char *name,name2[100];
  char ch;
  int  index = (int) maxlength;

  IF !NoFallBack
  THEN
    FPRINTF(stdout," Show relations with pathlength %d? y or n\n",maxlength + 1);

    while( (ch = (char) GETCH()) != 'y' && ch != 'n') doNothing;
    if(ch=='y') ++index;
  ELSE
    index = index/2;
  ENDIF

  IF f1 == NULL
  THEN
    TryAgain:
    PRINTF(" Press 1 to print relations to grdump.rel.\n");
    PRINTF(" Press 2 to print relations to another file.\n");
    PRINTF(" Press 3 to do nothing.\n");
    PRINTF(" Press 4 to exit the program.\n");

    ch = 'a';

    while(ch !='1' && ch !='2' && ch !='3' && ch !='4') ch= (char) GETCH();

    switch(ch)
    {
      case '4' : return;
      case '3' : return;
      case '1' : name = "grdump.rel";
		 f1 = fopen(name,"w");
		 break;
      case '2' : PRINTF(" Enter name of file for relations.\n\n");
		 tryagain1:
		 (void) gets(name2);
		 IF name2[0] == '\0' THEN goto tryagain1; ENDIF
			 IF FileExists(name2)
		 THEN
		   PRINTF(" %s already exists. \n",name2);
		   PRINTF(" Overwrite? y or n \n\n");
		   ch = 'a';
		   while(ch != 'y' && ch != 'n') ch = (char) GETCH();
		   IF ch == 'n' THEN goto TryAgain; ENDIF
		 ENDIF

		 f1 = fopen(name2,"w");
		 break;
      default  : break; /* can't happen */
    }

    if (f1 == NULL) 
    IF ch == '1'
    THEN 
      PRINTF("Cannot open %s\n",name); goto TryAgain;
    ELSE
      PRINTF("Cannot open %s\n",name2); goto TryAgain;
    ENDIF
  ENDIF

  if(f1==stdout) FPRINTF(f1,"\n Relations follow.\n");

  while(index > 0)
  {
    showRelSize(f1,index,root); 
    --index;
  }

  if(f1==stdout) FPRINTF(f1,"\n End reduced relations.\n");

  IF NoFallBack
  THEN
    IF f1 == stdout
    THEN
      pause();
    ENDIF

    FPRINTF(f1,"\n\n Relations with length > %d follow.\n",maxlength/2);

    index = (int) maxlength;
    while(index > (int) maxlength/2)
    {
      showRelSize(f1,index,root);
      --index;
    }

    FPRINTF(f1,"\n End extra reduced relations.\n");
  ENDIF

  if(f1 != stdout) fclose(f1);
  return;
}

/* *************************************************************  */
#ifdef NOPROTO
static int FillPath(RM,ch1)
RELMEM *RM;
char ch1[256];
#else
static int FillPath(RELMEM *RM,char ch1[256])
#endif
{
   int index,i,j,k,use=1;
   char ch;

   for (j=0; j <= (int) maxlength; ++j)
   {
     ch = ch1[j];
     index = translator[ch];
     if ( index == -1) break;

     IF index >= numberEdges
     THEN
       PRINTF(" Edge %c does not exist.\n",ch);
       return(1);
     ENDIF

     RM->edgelist[j] = index;
    }

    IF index != -1
    THEN
      ++j;
      ch = ch1[maxlength+1];
      index = translator[ch];

      IF index != -1
      THEN
	PRINTF(" path is too long \n ");
	return(1);
      ENDIF
    ENDIF

    RM->pathlength = (unsigned) j;

    IF j
    THEN
      RM->init = EdgeList[RM->edgelist[0]].init;
      RM->term = EdgeList[RM->edgelist[j-1]].term;
    ENDIF

    for (k=0;k<=j-2;k++)
    {
      IF EdgeList[RM->edgelist[k]].term !=
			    EdgeList[RM->edgelist[k+1]].init
      THEN
	PRINTF("\n Edge ends don't match in fillPaths. \n");
	PRINTF(" Bad path is ");
	for(i = 0; i<= k; ++i)
	{
	  PRINTF("%c",EdgeList[RM->edgelist[i]].name);
	}

	PRINTF("*");

	for(i = k+1; i<= j - 1; ++i)
	{
	  PRINTF("%c",EdgeList[RM->edgelist[i]].name);
	}

	PRINTF("\n");
	PRINTF(" A break in your path is indicated by '*'.\n");
	return(2);
      ENDIF
    }

  return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
int GetRelation(f1,tr,leadone)
FILE *f1;
TEMPREL *tr;
int leadone;
#else
int GetRelation(FILE *f1,TEMPREL *tr,int leadone)
#endif
/*
  new relation is stored in
  returns 0 when no errors.
  returns 1 in case there is insufficient space.
  returns 2 if an invalid relation was entered.
*/
{
  char ch1[256],coefstg[30],vertexstg[30];
  int done,vertex,alphanum,ch,sign,state,coef,i,j,k,length;
  int index;
  RELATION *R;
  RELMEM *RM;

  index = 0;

  done = sign = 0;
  state = j = k = i = -1;

  R = &tr -> rel;
  R -> pmt = 1;
  length = 0;

  while (!done)
  {
    done = (ch = getc(f1)) == ';';
    IF isspace(ch) THEN continue; ENDIF

    IF (unsigned) length + 1 > tr->RMgroups * MAXRMLST
    THEN
      if (expandTempRel(tr,(unsigned) length+1)) return(1);
    ENDIF

    RM = &R -> mem[length/MAXRMLST][length%MAXRMLST];

    switch(state)
    {
      case -1: i = j = k = -1;
	       sign = 1;

	       IF ch == '+' || ch == '-'
	       THEN

		 IF ch == '-'
		 THEN
		   sign = -1;
		 ENDIF

		 state = 1;
		 break;
	       ENDIF

		IF ch == ';'
		THEN
		  state = -1;
		  PRINTF(";\n");
		  continue;
		ENDIF

	       IF isalnum(ch) || ch == '('
	       THEN
		 state = 1;
		 IF ungetc(ch,f1) != ch
		 THEN
		   PRINTF(" Problem with ungetc.\n");
		   return(1);
		 ENDIF
		 break;
	       ENDIF

	       PRINTF("\n\n Symbol %c is not allowed at 1.\n",ch);
	       index = 2;
	       goto out;

      case  0:  j = i = k = -1;

		IF !(ch == '+' || ch == '-' || ch == ';')
		THEN
		  PRINTF("\n\n Symbol %c is not allowed at 2.\n",ch);
		  index = 2;
		  goto out;
		ENDIF

		IF ch == ';'
		THEN
		  state = -1;
		  PRINTF(";\n");
		  break;
		ENDIF

		sign = 1;

		IF ch == '-'
		THEN
		  sign = -1;
		ENDIF

		state = 1;
		break;

      case  1:  IF ch >= '0' && ch < '0' + 10
		THEN
		  coefstg[++i] = (char) ch;
		  break;
		ENDIF

		IF ! (isalpha(ch) || ch == '(' || ch == '*')
		THEN
		  PRINTF("\n\n Symbol %c is not allowed at 3.\n",ch);
		  index = 2;
		  goto out;
		ENDIF

		state = 2;

		IF ch == '*'
		THEN
		  IF i == -1
		  THEN
		    PRINTF("\n\n A coef is required at 4. \n");
		    index = 2;
		    goto out;
		  ENDIF
		ENDIF

		IF i != -1
		THEN
		  coefstg[++i] = '\0';
		  coef = sign * atoi(coefstg);
		ELSE
		  coef = sign;
		ENDIF

		IF ch != '*' && ch != '('
		THEN
		  IF ungetc(ch,f1) != ch
		  THEN
		    PRINTF(" Problem with ungetc.\n");
		    return(1);
		  ENDIF
                ENDIF

		IF ch == '(' THEN state = 3; ENDIF

		break;

      case  2: IF isalpha(ch)
	       THEN
		 IF -1 == translator[ch]
		 THEN
		   PRINTF(" Nonexistent edge %c. Relation cancelled.\n",ch);
		   index = 2;
		   goto out;
		 ENDIF

		 ch1[++j] = (char) ch;
		 break;
	       ENDIF

	       IF ch == '(' && j == -1
	       THEN
		 state = 3;
		 continue;
	       ENDIF

	       ch1[++j] = '\0';

	       IF coef > 0
	       THEN
		 PRINTF(" + %d * %s",coef,ch1);
	       ELSE
		 PRINTF(" - %d * %s",-coef,ch1);
	       ENDIF

	       IF j != 0
	       THEN
		 IF (coef = pAdjust(coef)) == 0 THEN goto skipPath; ENDIF

		 IF j > (int) maxlength + 2
		 THEN
		   PRINTF(" Path too long.  Program aborted.\n");
		   index = 2;
		   goto out;
		 ELSE
		   if(j == (int) maxlength+1) goto skipPath;
                   /* path has length maxlength + 1 */
		 ENDIF

		 IF (index = FillPath(RM,ch1)) == 1 THEN return(1); ENDIF

		 RM -> coef = coef;
		 ++length;
	       ENDIF

	       skipPath:

               IF ch == '+' || ch == '-' || ch == ';'
	       THEN
		 IF ch == ';'
		 THEN
		   PRINTF(";\n");
		   state = -1;
		 ELSE
		   state = 0;
		   IF ungetc(ch,f1) != ch
		   THEN
		     PRINTF(" Problem with ungetc.\n");
		     return(NULL);
		   ENDIF
                 ENDIF

		 break;
	       ENDIF

	       PRINTF(" Unallowed symbol %c at 5.\n",ch);
	       index = 2;
	       goto out;

      case  3: IF ch >= '0' && ch < '0' + 10
	       THEN
		 vertexstg[++k] = (char) ch;
		 break;
	       ENDIF

	       IF ch == ')'
	       THEN
		 IF k == -1
		 THEN
		   PRINTF(" An integer is required for each vertex. \n");
		   index = 2;
		   goto out;
		 ENDIF

		 vertexstg[++k] = '\0';
		 vertex = atoi(vertexstg);

		 state = 0;

		 IF coef > 0
		 THEN
		   PRINTF(" + %d * (%d)",coef,vertex);
		 ELSE
		   PRINTF(" - %d * (%d)",-coef,vertex);
		 ENDIF

		 IF k != 0
		 THEN
		   IF (coef = pAdjust(coef)) == 0 THEN goto skipPath1; ENDIF

		   --vertex;

		   IF vertex < 0 || vertex > numberVertexes - 1
		   THEN
		     PRINTF("\n Vertex number %d out of range. \n",vertex+1);
		     index = 2;
		     goto out;
		   ENDIF

		   RM -> init = vertex;
		   RM -> term = vertex;
		   RM -> pathlength = 0;
		   RM -> coef = coef;
		   ++length;
		 ENDIF

		 skipPath1:
                 break;
	       ENDIF

	       PRINTF("\n\n Unallowed symbol %c at 6.\n",ch);
	       index = 2;
	       goto out;
      default: doNothing;
    }
  }

  out:

  if(index == 2)
  IF f1 == stdin
  THEN
    tr -> rel.length = 0;
    while(ch != ';') ch = getc(f1);
    return(2);
  ELSE
    tr -> rel.length = 0;
    return(1);
  ENDIF

  tr -> rel.length = (unsigned) length;
  Reorder(tr);
  if(tr->rel.length&&leadone)LeadOne(tr);

  return(0);
}
/* *************************************************************  */
/*
#ifdef NOPROTO
static void fshowGamma(f1,bg)
FILE *f1;
BG *bg;
#else
static void fshowGamma(FILE *f1,BG *bg)
#endif
{
  int i,j,num;
  PATH2 *p;

  for(i=0; i<= bg -> level; ++i)
  {
    if(i>0) FPRINTF(f1," %3d ",bg -> LastIndex[i]);
    fshowPath(f1,bg ->paths[i],0);
  }
  FPRINTF(f1,"\n");

    p = bg->paths[i];

}
*/
#ifdef NOPROTO
void fShowGamma(f1,bg)
FILE *f1;
BG *bg;
#else
void fShowGamma(FILE *f1,BG *bg)
#endif
{
  int i,j;
  char ch;

  for(i=0; i<= bg -> level; ++i)
  {
    IF i != 0
    THEN
      for(j = bg->LastIndex[i]+1;j < (int) bg->paths[i]->pathlength; ++j)
	FPRINTF(f1,"%c",EdgeList[bg->paths[i]->edgelist[j]].name);
    ELSE
      for(j=0;j < (int) bg->paths[i]->pathlength; ++j)
	FPRINTF(f1,"%c",EdgeList[bg->paths[i]->edgelist[j]].name);
    ENDIF
  }
  FPRINTF(f1,"\n");

  for(i=0; i<= bg -> level; ++i)
  {
    if(i>0) FPRINTF(f1," %3d ",bg -> LastIndex[i]);
    fShowPath(f1,bg->paths[i]->edgelist,(int) bg->paths[i]->pathlength,0);
  }
  FPRINTF(f1,"\n");

  return;
}
/* ************************************************************* */
