/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
#include "grs.h"
#include "grglobal.h"
#include "ctype.h"
#include "conio.h"
#include "stdlib.h"
#include "string.h"

#ifndef NOPROTO
static void Show(void);
static void show(int choice, RELATION *r);
#endif

/* *************************************************************  */

#ifdef NOPROTO
static TEMPREL *Select()
#else
static TEMPREL *Select(void)
#endif
{
  int i,ch,chd;

  again:

  PRINTF("\n Choose x or t then 0 - 9  ");
  PRINTF(" or press s to show elements.\n");

  while( (ch  = GETCH()) != 'x' && ch != 't' && ch != 's') doNothing;

  IF ch == 's'
  THEN
    ch = '0';
    Show();
    goto again;
  ENDIF

  PRINTF("\n %c",ch);
  while( (chd = GETCH()) < '0' || chd > '9') doNothing;
  PRINTF("%c = ",chd);

  switch(chd)
  {
    case '0': i = 0; break;
    case '1': i = 1; break;
    case '2': i = 2; break;
    case '3': i = 3; break;
    case '4': i = 4; break;
    case '5': i = 5; break;
    case '6': i = 6; break;
    case '7': i = 7; break;
    case '8': i = 8; break;
    case '9': i = 9; break;
    default : PRINTF(" Error in select.\n"); return(NULL);
  }

  switch(ch)
  {
    case 'X':
    case 'x':  return(&X[i]);
    case 'T':
    case 't':  return(&T[i]);
    default : PRINTF(" Error in select.\n"); return(NULL);
  }
}
/* ****************************************************************** */
#ifdef NOPROTO
static void saveElements()
#else
static void saveElements(void)
#endif
{
  RELATION **d,*r,*dtemp;
  FILE *f1;
  char *name,*name2;
  char name5[100];
  int done,ch,i,index;

  tryagain:

  PRINTF("\n Press a to save elements to the file grdump.elt. \n");
  PRINTF(" Press b to save elements to another file. \n");
  PRINTF(" Press q to quit. \n");

  nextc:
  ch = GETCH();

  switch(ch)
  {
    case 'a' : name = "grdump.elt";
	       f1 = fopen(name,"w");

	       IF f1 == NULL
	       THEN
		 PRINTF(" Cannot open %s.\n",name);
		 goto tryagain;
	       ENDIF

	       break;

    case 'b' : newtry:
	       PRINTF(" In which file should your elements be stored. \n ");
	       (void) gets(name5);
	       if (name5[0] == '\0') goto newtry;
	       f1 = fopen(name5,"w");

	       IF f1 == NULL
	       THEN
		 PRINTF(" Cannot open %s.\n",name5);
		 goto tryagain;
	       ENDIF

	       break;

    case 'q' : return;

    default  : goto nextc;

  }

  for(i = 0; i < 10; ++i)
  {
    IF X[i].rel.length != 0 THEN fShowRelation(f1,&X[i].rel,0); ENDIF
  }

  for(i = 0; i < 10; ++i)
  {
    IF T[i].rel.length != 0 THEN fShowRelation(f1,&T[i].rel,0); ENDIF
  }

  fclose(f1);

  return;
}
/* ****************************************************************** */
#ifdef NOPROTO
static int Enter()
#else
static int Enter(void)
#endif
{
  RELATION *r,*dtemp;
  TEMPREL *d;
  FILE *f1;
  char *name,*name3, name2[60];
  int done,ch,index,i;

  name3 = "grdump.elt";
  name = ".elt";

  tryagain:

  PRINTF("\n Press a to enter elements from the file %s.elt. \n",nameb);
  PRINTF(" Press b to enter elements from terminal. \n");
  PRINTF(" Press c to enter elements from another file. \n");
  PRINTF(" Press d to enter elements from file grdump.elt. \n");
  PRINTF(" Press q to quit. \n");

  nextc:

  ch = GETCH();

  switch(ch)
  {
    case 'b' : f1 = stdin;
	       break;

    case 'a' : strcpy(name2,nameb);
	       strcat(name2,name);
	       f1 = fopen(name2,"r");

	       IF f1 == NULL
	       THEN
		 PRINTF(" Cannot open %s.\n",name2);
		 automatic = 0;
		 goto tryagain;
	       ENDIF

	       break;

    case 'd' : strcpy(name2,name3);
	       f1 = fopen(name2,"r");

	       IF f1 == NULL
	       THEN
		 PRINTF(" Cannot open %s.\n",name2);
		 goto tryagain;
	       ENDIF

	       break;

    case 'c' : newtry:
	       PRINTF(" In which file are your elements. \n ");
	       (void) gets(name2);
	       if (name2[0] == '\0') goto newtry;
	       f1 = fopen(name2,"r");

	       IF f1 == NULL
	       THEN
		 PRINTF(" Cannot open %s.\n",name2);
		 goto tryagain;
	       ENDIF

	       break;

    case 'q' : return(0);

    default  : goto nextc;

  }

  done = 0;

  while (!done)
  {
    IF f1 == stdin
    THEN
      PRINTF("\n You may enter your element now.\n");
    ENDIF

    IF (index = GetRelation(f1,tr0,0)) == 1
    THEN
      return(1);
    ELSE
      IF index == 0
      THEN
	PRINTF("\n Do you want to store this element? y or n \n");
	while ( (ch = GETCH()) != 'y' && ch != 'n') doNothing;

	IF ch == 'y'
	THEN
	  PRINTF("\n What is the destination of this element?\n");
	  PRINTF(" The destination is ");
	  d = Select();
	  show(0,&tr0->rel);
	  PRINTF("\n");

	  IF copyTEMPREL(&tr0->rel,d) THEN return(1); ENDIF
	ENDIF
      ELSE
	PRINTF("\n Your last attempt to enter an element was aborted. \n");
      ENDIF
    ENDIF

    IF f1 != stdin
    THEN
      while(isspace(ch = getc(f1))) doNothing;

      IF ch == EOF
      THEN
	PRINTF("\n All elements in this file have now been entered. \n");
	goto out;
      ELSE
	IF ungetc(ch,f1) != ch
	THEN
	  PRINTF(" Problem with ungetc \n");
	  return(1);
	ENDIF
      ENDIF
    ENDIF

    PRINTF("\n Do you wish to enter another element? y or n. \n");
    while ( (ch = GETCH()) != 'y' && ch != 'n') doNothing;

    done = ch == 'n';
  }

  out:

  IF f1 != stdin
  THEN
    fclose(f1);
  ENDIF

  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static show(choice, r)
int choice;
RELATION *r;
#else
static void show(int choice, RELATION *r)
#endif
/*
  If choice <> 0, the relation to be shown is to be selected;
  otherwise the relation r will be shown.
*/
{
  char ch;

  IF choice
  THEN
    PRINTF(" Which element do you wish to show?\n");
    r = &(Select() -> rel);
  ENDIF

  fShowRelation(stdout,r,0);

  return;
}
/* ****************************************************************** */
#ifdef NOPROTO
static Show()
#else
static void Show(void)
#endif
{
  int ch = 'z';
  int i;

  PRINTF("\n Press t if you wish to show all elements labeled by t.\n");
  PRINTF(" Press x if you wish to show all elements labeled by x.\n");
  PRINTF(" Press b if you wish to show both.\n\n");

  while( (ch = GETCH()) != 't' && ch != 'x' && ch != 'b') doNothing;

  IF ch == 'b' || ch == 'x'
  THEN
    for( i = 0; i<= 9; ++i)
    {
       PRINTF("x%d:",i); show(0,&X[i].rel);
    }
  ENDIF

  PRINTF("\n");

  IF ch == 'b' || ch == 't'
  THEN
    for( i = 0; i<= 9; ++i)
    {
       PRINTF("t%d:",i); show(0,&T[i].rel);
    }
  ENDIF

  PRINTF("\n");
  pause();

  return;
}
/* ****************************************************************** */
#ifdef NOPROTO
static int MultiplyRelations()
#else
static int MultiplyRelations(void)
#endif
/*
  Multiplies two elements modulo paths of length > maxlength.
*/
{
  RELATION *r1,*r2;
  TEMPREL *d;

  PRINTF(" What element is the first argument for your multiplication?\n");
  PRINTF(" Your first argument is:\n ");

  r1 = &(Select()->rel);
  show(0,r1);
  PRINTF("\n");

  PRINTF(" What element is the second argument for your multiplication?\n");
  PRINTF(" Your second argument is: \n");

  r2 = &(Select()->rel);
  show(0,r2);
  PRINTF("\n");

  PRINTF(" What is the destination of the result?\n");
  PRINTF(" The destination is ");
  d = Select();

  IF !r1 -> length || !r2 -> length
  THEN
    d -> rel.length = 0;
  ELSE
    IF multRelations(r1,r2,&tr0) THEN return(1); ENDIF
    IF copyTEMPREL(&tr0->rel,d) THEN return(1); ENDIF
  ENDIF

  show(0,&d->rel);
  PRINTF("\n");

  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static int AddRelations()
#else
static int AddRelations(void)
#endif
/*
  Multiplies two elements modulo paths of length > maxlength.
*/
{
  RELATION *r1,*r2;
  TEMPREL *d;

  PRINTF("\n What element is the first argument for your addition?\n");
  PRINTF(" Your first argument is:\n");

  r1 = &Select()->rel;
  show(0,r1);
  PRINTF("\n");

  PRINTF(" What element is the second argument for your multiplication?\n");
  PRINTF(" Your second argument is:\n ");

  r2 = &Select()->rel;
  show(0,r2);

  PRINTF("\n");
  PRINTF(" What is the destination of the result?\n");
  PRINTF(" The destination is: \n");
  d = Select();

  IF addRels(r1,r2,tr0,1) THEN return(1); ENDIF
  IF copyTEMPREL(&tr0->rel,d) THEN return(1); ENDIF

  show(0,&d->rel);
  PRINTF("\n");

  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static int multc()
#else
static int multc(void)
#endif
/*
  Multiplies an element by a field element.
  Returns 1 if there are memory problems, otherwise 0.
*/
{
  RELATION *r,*r1,*r2;
  TEMPREL *d,*d1;
  RELMEM *rm;
  int coef;
  unsigned index;

  PRINTF(" Which element would you like to multiply by a constant?\n");
  PRINTF(" You are multiplying:\n ");

  r = &(d = Select())->rel;
  show(0,r);
  PRINTF("\n");

  PRINTF(" By what field element would you like to multiply?\n");
  coef = getint() % prime;
  PRINTF(" Your field element is %d. \n",coef);


  PRINTF(" What is the destination of the result?\n");
  PRINTF(" The destination is:\n ");
  r1 = &(d1 = Select())->rel;
  r2 = &(tr0->rel);

  IF coef && r->length
  THEN
    IF copyTEMPREL(&d->rel,tr0) THEN return(1); ENDIF

    index = 0;
    
    while(index <  r -> length)
    {
      rm =  r2 -> mem[index>>SHIFT] + (index&MASK);
      rm -> coef = pMult(coef,rm ->coef);
      ++index;
    }

    IF copyTEMPREL(&tr0->rel,d1) THEN return(1); ENDIF
  ELSE
    r1 ->length = 0;
  ENDIF

  show(0,r1);
  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static int divide()
#else
static int divide(void)
#endif
/*
  Divides an element by a field element.
  Returns 1 if there are memory problems, otherwise 0.
*/
{
  RELATION *r,*r1;
  TEMPREL *d,*d1;
  RELMEM *rm;
  int coef;
  unsigned index;

  PRINTF(" Which element would you like to divide by a constant?\n");
  PRINTF(" You are dividing:\n ");

  r = &Select()->rel;
  show(0,r);
  PRINTF("\n");

  nextCoef:

  PRINTF(" By what field element would you like to divide?\n");
  coef = getint() % prime;
  PRINTF(" Your field element is %d. ",coef);

  IF coef % prime == 0
  THEN
    PRINTF(" You may not divide by zero. \n\n");
    goto nextCoef;
  ENDIF

  coef = pInverse(coef);
  PRINTF(" It's inverse is %d.\n",coef);

  PRINTF(" What is the destination of the result?\n");
  PRINTF(" The destination is:\n ");

  r1 = &(d1 = Select()) ->rel;

  IF r -> length
  THEN
    IF copyTEMPREL(r,d1) THEN return(1); ENDIF

    index = 0;
    while(index <  r1 -> length)
    {
      rm =  r1 -> mem[index>>SHIFT] + (index&MASK);
      rm -> coef = pMult(coef,rm ->coef);
      ++index;
    }
  ELSE
    r1 -> length = 0;
  ENDIF

  show(0,r1);
  PRINTF("\n");
  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static int reduceR()
#else
static int reduceR(void)
#endif
{
  RELATION *r,*r1;
  TEMPREL *d,*d1;
  int i,projres = 0;;

  PRINTF(" Which element would you like to reduce?\n");
  PRINTF(" You are reducing:\n ");

  r = &(d = Select())->rel;

  show(0,r);
  PRINTF("\n");


  PRINTF(" What is the destination of the result?\n");
  PRINTF(" The destination is:\n ");
  r1 = &(d1 = Select())->rel;

  IF r-> length
  THEN
    IF (i = reduce(r,0,projres)) == 1 THEN return(1); ENDIF

    IF copyTEMPREL(&(i==2?tr1:d)->rel,d1) THEN return(1); ENDIF
  ELSE
    r1 -> length = 0;
  ENDIF

  show(0,r1);
  PRINTF("\n");

  return(0);
}
/* ****************************************************************** */
#ifdef NOPROTO
static void graph()
#else
static void graph(void)
#endif
{
  int i,ch;

  PRINTF("\n");

  for(i = 0;i < numberEdges;++i)
  {
    PRINTF("%c   %3d %3d \n",EdgeList[i].name,EdgeList[i].init+1,
		      EdgeList[i].term+1);
  }
  pause();
  return;
}
/* ****************************************************************** */

#ifdef NOPROTO
int algebra(init)
int *init;
#else
int algebra(int *init)
#endif
{
  int i,ch;
  RELATION *tempR;
  static int menu;
  int thomogeneous = homogeneous;

  homogeneous = 0;

  if (*init)
  {
     menu = 1;

     for(i=0;i<=9;++i)
     {
       X[i].RMgroups = 0; T[i].RMgroups = 0;
       X[i].rel.mem = NULL; T[i].rel.mem = NULL;
     }

    PRINTF("\n\n");

    PRINTF(" This procedure allows you to do manipulations in the quotient \n");
    PRINTF(" algebra which you have defined.  You will be given the oppor- \n");
    PRINTF(" tunity to enter and save elements, which may then be operated \n");
    PRINTF(" upon. You may reduce modulo your ideal, add, or multiply \n");
    PRINTF(" your elements.  Paths of length greater than %d \n",maxlength);
    PRINTF(" are automatically eliminated.  You may also display the \n");
    PRINTF(" elements which you have saved or store them in a file. \n\n");

    PRINTF(" Should you exit and reenter this procedure but not the program,\n");
    PRINTF(" your elements will be saved.\n\n");
    pause();
  }

  *init = 0;

  again:
    IF menu
    THEN
     PRINTF("\n\n");
     PRINTF(" Press e to enter an element.\n");                    /* ok */
     PRINTF(" Press s to show all elements in a group.\n");
     PRINTF(" Press S to show an element.\n");
     PRINTF(" Press r to reduce an element modulo your ideal.\n");
     PRINTF(" Press z to save elements. \n");
     PRINTF(" Press p to take the product of two elements.\n");
     PRINTF(" Press a to add two elements.\n");
     PRINTF(" Press m to multiply an element by a constant.\n");
     PRINTF(" Press d to divide an element by a constant.\n");
     PRINTF(" Press g to display the edges of the graph. \n");
     PRINTF(" Press M to remove Menu. \n");
     PRINTF(" Press q to quit.\n\n");
    ELSE
     PRINTF(" Press M to display Menu. \n");
    ENDIF

    nextc:
    ch = GETCH();
    switch(ch)
    {
      case 'q' : homogeneous = thomogeneous;
		 return(0);

      case 'M' : menu = !menu; goto again;

      case 'e' : IF Enter() THEN return(1); ENDIF
		 goto again;

      case 's' : Show();
		 goto again;

      case 'd' : IF divide() THEN return(1); ENDIF
		 goto again;

      case 'a' : IF AddRelations() THEN return(1); ENDIF
		 goto again;

      case 'm' : IF multc() THEN return(1); ENDIF
		 goto again;

      case 'p' : IF MultiplyRelations() THEN return(1); ENDIF;
		 goto again;

      case 'S' : tempR = NULL;
		 show(1,tempR);
		 goto again;

      case 'r' : IF reduceR() THEN return(1); ENDIF
		 goto again;

      case 'g' : graph();
		 goto again;

      case 'z' : saveElements();
		 goto again;

      default  : goto nextc;
    }
}

