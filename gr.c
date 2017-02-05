/*	gr.C   */
#include "conio.h"
#include "grs.h"

/* ******************* GLOBAL VARIABLES ************************ */
#include "grglb.h"

/* ***************************** PROTOTYPES ************************* */
/* ***************************** grinit.c *************************** */
void getConstants(void);
int initialize(void);
int initializeR(void);
/* ***************************** grrelops.c *************************** */
int expand(RELATION *r1,PATH **root);
/* ***************************** grquogen.c *************************** */
int QuotientBasis(PATH **root);


/* *************** FUNCTIONS  ********************************** */
void print_copyright(char *COPYRIGHT1,char *COPYRIGHT2) 
{
   fputs( " GROEBNER  VERSION. 1.0 \n",stderr); 
   fputs( COPYRIGHT1, stderr );
   fputs( COPYRIGHT2, stderr );
   return;
}

/* ************************************************************* */

int initializeList(PATH **root)
/*
  returns 1 if there are problems with memory, otherwise 0.
*/
{
   RELATION *r1,*r2;
   int  i;

   r2 = firstR;

   while(r2 != NULL)
   {
      IF r2 -> mem[0] -> pathlength < maxlength
      THEN
	r1 = firstR;

	while(r1 != NULL)
	{
	  IF r1 -> index > r2 -> index THEN break; ENDIF

	  IF r2 -> pmt != 1 THEN break; ENDIF

	  IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

	  if(r1 -> mem[0] -> pathlength < maxlength)
	  IF (i = useOverlap(r1,r2,root)) == 1 THEN return(1); ENDIF

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
	   IF (i = BackReduce(r1,*root)) == 1 THEN return(1); ENDIF
	   r1 -> pmt = 0;

	   if(i == 2 && tr1->rel.length)
	   IF newRelation(root,tr1) THEN return(1); ENDIF
	 ENDIF

	 r1 = r1 -> next;
      }

      if(!homogeneous&&!NoFallBack)
      {
	r1 = firstR;
	while(r1 != NULL)
	{
	  if(r1 -> pmt == 1)
	  IF (i = expand(r1,root)) == 1 THEN return(1); ENDIF

	  r1 = r1 -> next;
	}
      }

      r2 = r2 -> next;
   }  /* end while not done */

   out:

   return(0);
}
/* ************************************************************* */

int constructBasis(PATH **root)
/*
  constructs Groebner basis.  returns 1 if there are problems with
  memory, otherwise 0.
*/
{
   RELATION *r1,*r2,*tempR;
   int  done = 0,i;
   unsigned long oldIndexBound,indexBound=0;

   while(!done)
   {
      oldIndexBound = indexBound;
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
	   IF (i = BackReduce(r1,*root)) == 1 THEN return(1); ENDIF
	   r1 -> pmt = 0;

	   IF i == 2 && (tempR = &tr1->rel) ->length
	   THEN
	     done = 0;
	     IF newRelation(root,tr1) THEN return(1); ENDIF
	   ENDIF
	 ENDIF

	 r1 = r1 -> next;
      }

      r1 = firstR;

      while(r1 != NULL)
      {
	IF r1 -> index > indexBound THEN break; ENDIF

	IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

	r2 = firstR;

	if(r1 -> mem[0] -> pathlength < maxlength)
	while(r2 != NULL && r2->index <= r1->index)
	{
	  IF r2 -> pmt != 1 THEN r2 = r2 -> next; continue; ENDIF

	  IF (i = useOverlap(r1,r2,root)) == 1 THEN return(1); ENDIF

	  IF i THEN done = 0; ENDIF

	  r2 = r2 -> next;
	}

	r1 = r1 -> next;
      }

      if(!homogeneous&&!NoFallBack)
      {
	r1 = firstR;
	while(r1 != NULL)
	{
	  IF r1 -> pmt == 1
	  THEN
	    IF (i = expand(r1,root)) == 1 THEN return(1); ENDIF
	    if(i) done = 0;
	  ENDIF

	  r1 = r1 -> next;
	}
      }

      if(!done)
      {
	PRINTF(" This ideal gives a negative answer to a question of\n");
	PRINTF(" Ed Green.  Please send your data to him. \n");
      }
   }  /* end while not done */

   return(0);
}
/* ************************************************************* */
int sharpenBasis(PATH **root)
{
   RELATION *r1;
   int indexBound,i;

   indexBound = lastR -> index;
   r1 = firstR;

   while(r1 != NULL && r1->index <= indexBound)
   {
     IF r1 -> pmt != 1 THEN r1 = r1 -> next; continue; ENDIF

     IF (i = reduce(r1,*root,1)) == 1 THEN return(1); ENDIF

     IF (i == 2)
     THEN
       IF deletePath(r1 ->tipPtr,root) THEN return(1); ENDIF
       r1 -> tipPtr = NULL;
       r1 -> pmt = 0;
       tr1->rel.creationTime = r1 -> creationTime;
       tr1->rel.index = rIndex++;
       LeadOne(tr1);
       IF storeRel(&tr1->rel,root) THEN return(1); ENDIF
       IF enterTip(lastR,root) THEN return(1); ENDIF
       lastR -> pmt = 1;
     ENDIF

     r1 = r1 -> next;
   }

   out:

   CollectGarbage(root);
   return(0);
}

int main(void)               /* main program */
{

   long ttemp;
   char *COPYRIGHT1,*COPYRIGHT2;

   int index,i,ch,init=1,genpath=1,overlaps=1,gammas =1;
   struct MemAdjLst *temp;
   FILE *f1;

   COPYRIGHT1 =
" Copyright (c) 1990 All rights reserved. C.D.Feustel,Edward Green,\n";
   COPYRIGHT2 =
" Virginia Polytechnic Institute and State University\n\n";

   printf("\n");
   print_copyright(COPYRIGHT1,COPYRIGHT2);

   getConstants();

   IF (i = initialize())==1 THEN goto out; ENDIF

   printf(" The graph has been initialized.\n\n");

   IF (i = initializeR()) == 1 THEN goto out; ENDIF

   printf(" The list of relations has been entered.\n\n");

   IF (i = initializeList(&rootPath)) == 1 THEN goto out; ENDIF

   printf(" The list of relations has been initialized.\n\n");

   IF (i = constructBasis(&rootPath)) == 1 THEN goto out; ENDIF

   printf(" A basis whose tips are minimal has been constructed.\n\n");

   IF (i = sharpenBasis(&rootPath)) == 1 THEN goto out; ENDIF

   printf(" A sharp Groebner basis has been constructed.\n\n");




   repeater:
     PRINTF("\n");
     PRINTF(" Press m to make the generating paths.\n");
/*
     PRINTF(" Press g to get the Gammas.\n");
     PRINTF(" Press a to do algebraic manipulations.\n");
*/
     PRINTF(" Press d to display the minimal sharp vectors.\n");
     PRINTF(" Press s to save the minimal sharp vecors.\n");
     PRINTF(" Press q to quit. \n");
     PRINTF("\n");


     nextCh:
     ch = GETCH();
     switch (ch)
     {
       case   'q': return(0);
/*
       case   'a':
		   if (algebra(&init))
		     {PRINTF("problems in algebra.\n");goto out; }
		   goto repeater;
*/
       case   'm':
		   if (genpath)
		   if (QuotientBasis(&rootPath))
		     {PRINTF("problems in QuotientBasis.\n");goto out; }
		   genpath = 0;
		   goto repeater;
/*
       case   'g':
		   if (genpath)
		   if (QuotientBasis(&rootPath))
		     {PRINTF("problems in QuotientBasis.\n");goto out; }
		   genpath = 0;

		   if (overlaps)
		   if (FindOverlaps())
		     {PRINTF("problems in FindOverlaps.\n");goto out; }
		   overlaps = 0;

		   if (gammas)
		   if (BuildGammas())
		     {PRINTF("problems in BuildGammas.\n");goto out; }
		   gammas = 0;

		   goto repeater;
*/
       case   'd':
		   IF !NoFallBack
		   THEN
		     if (genpath)
		     if (QuotientBasis(&rootPath))
		       {PRINTF("problems in QuotientBasis.\n");goto out;}
		     genpath = 0;
		   ENDIF

		   fShowrelations(stdout,rootPath);
		   goto repeater;

       case   's':
		   IF !NoFallBack
		   THEN
		     if (genpath)
		     if (QuotientBasis(&rootPath))
		       {PRINTF("problems in QuotientBasis.\n");goto out;}
		     genpath = 0;
		   ENDIF

		   fShowrelations(NULL,rootPath);
		   goto repeater;

       default:    goto nextCh;
     }

   out:

   IF i == 1 THEN printf(" memory problems .\n"); ENDIF

   ch = GETCH();

   return(0);
}

