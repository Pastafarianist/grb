/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
/*	grmain.C   */
#include "conio.h"
#include "grs.h"

/* ******************* GLOBAL VARIABLES ****************************** */
#include "grglb.h"

#ifndef NOPROTO
/* ***************************** PROTOTYPES ************************** */
/* ***************************** grinit.c **************************** */
void getConstants(void);
int initialize(void);
int initializeR(void);
/* ***************************** gralg.c ****************************** */
int algebra(int *init);
/* ***************************** grmodule.c *************************** */
int getBasis(int projres);
int getshifter(void);
/* ***************************** grresolv   *************************** */
int BuildGammas(int projres);
int projectiveResolution(int repnum);
#endif
/* ********************** FUNCTIONS  ********************************** */
#ifdef NOPROTO
static void print_copyright(COPYRIGHT1,COPYRIGHT2) 
char *COPYRIGHT1, *COPYRIGHT2;
#else
static void print_copyright(char *COPYRIGHT1,char *COPYRIGHT2) 
#endif
{
   fputs( " GROEBNER  VERSION. 1.0 \n",stderr); 
   fputs( COPYRIGHT1, stderr );
   fputs( COPYRIGHT2, stderr );
   return;
}

/* ******************************************************************** */
#ifdef NOPROTO
static void rdump(f1)
FILE *f1;
#else
static void rdump(FILE *f1)
#endif
{
  RELATION *r1;

  r1 = firstR;
  while(r1 != NULL)
  {
    fprintf(f1,"%3ld ",r1->index);
    fShowRelation(f1,r1,1);
    r1 = r1->next;
  }

  return;
}

#ifndef NOPROTO
void resetRestart(void);
#endif

#ifdef NOPROTO
static void cleanUp1()
#else
static void cleanUp1(void)
#endif
{
  rootPath = NULL;

  while(firstR != NULL)
  {
    firstR -> pmt = NULL;
    firstR -> usedAt = NULL;
    firstR -> tipPtr = NULL;

    firstR = firstR->next;
  }

  lastR = NULL;
  endGeneratorList = generatorList = NULL;
  resetRestart();

  return;
}

/* ******************************************************************** */
#ifdef NOPROTO
int main()               /* main program */
#else
int main(void)               /* main program */
#endif
{

   long ttemp;
   char *COPYRIGHT1,*COPYRIGHT2;

   int index,i,ch,init=1,genpath=1,overlaps=1,gammas =1,projres = 0;
   int repnum = 0;
   char *name = ".dmp",name2[120],chr = 'w';
   FILE *f1;

   COPYRIGHT1 =
" Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green,\n";
   COPYRIGHT2 =
" Virginia Polytechnic Institute and State University\n\n";

   PRINTF("\n");
   print_copyright(COPYRIGHT1,COPYRIGHT2);

   IF (i =getshifter()) == 1 THEN goto out; ENDIF

   getConstants();

   IF (i = initialize()) != 0 THEN goto out; ENDIF
   PRINTF("\n The graph has been initialized.\n");

   initializeRelations:

   IF (i = initializeR()) != 0 THEN goto out; ENDIF

   PRINTF("\n The list of relations has been entered.\n");

   IF (i = getBasis(projres)) == 1 THEN goto out; ENDIF

   generatorList = firstR;
   endGeneratorList = lastR;

   repeater:
     PRINTF("\n");
     PRINTF(" Press g to get the Gammas.\n");
     PRINTF(" Press r to calculate minimal resolution.\n");
     PRINTF(" Press a to do algebraic manipulations.\n");
     PRINTF(" Press d to display the minimal sharp elements.\n");
     PRINTF(" Press s to save the minimal sharp elements.\n");
     PRINTF(" Press i to reinitialize the relations.\n");
     PRINTF(" Press q to quit. \n");
     PRINTF("\n");

     nextCh:
     ch = GETCH();
     switch (ch)
     {
       case 'q': return(0);

       case 'a':
		   if (algebra(&init) == 1)
		     {PRINTF("\n Problems in algebra.\n");goto out; }
		   goto repeater;

       case 'r':
		 projres = 1;
		 firstR = lastR = NULL;

		 if( projectiveResolution(repnum++)==1)  
		   { PRINTF("\n Problems in projectiveResolution.\n");
		     goto out; 
		   }

		 firstR = generatorList;
		 lastR = endGeneratorList;
		 projres = 0;
		 goto repeater;
       case 'g':
		   if (gammas)
		   if (BuildGammas(projres))
		     {PRINTF("\n Problems in BuildGammas.\n");goto out; }
		   gammas = 0;

		   goto repeater;
       case 'd':
		   fShowrelations(stdout,rootPath);
		   pause();
		   goto repeater;

       case 's':
		   fShowrelations(NULL,rootPath);
		   goto repeater;

       case 'i':   automatic = 0;
		   gammas  = 1;
		   repnum  = 0;
		   cleanUp1();
		   goto initializeRelations;

       default:    goto nextCh;
     }

   out:

   IF i == 1 
   THEN 
     PRINTF("\n Memory problems.\n"); 
     free(matrix1);
     IF !getfile(&f1,namea,name2,name,chr)
     THEN  
       rdump(f1);
       PRINTF(" Current relation list is in %s.\n",name2);
     ENDIF
   ENDIF

   pause();

   return(0);
}


