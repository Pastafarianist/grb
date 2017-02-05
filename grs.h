#include <stdio.h>

#define FPRINTF (void) fprintf
#define PRINTF (void) printf
#define doNothing

#define gaget int
#include "grdef.h"

#define IF {if(
#define THEN ){
#define ELSE } else {
#define ENDIF }}

struct relation;

typedef struct edge                      /* definition of edge */
{
  int  init,term;
  int  iname;
  char name;
} EDGE ;


typedef struct MemAdjLst      /* definition of member of adjacency list */
{
  gaget EdgeNum,term;         /* term is number of terminal vertex */
  struct MemAdjLst *Next;
} MEMADJLST;

typedef struct path                          /* definition of path */
{
  gaget init,term;
  unsigned length;
  gaget *edges;                      /* list of edge numbers on path */
  struct path *lchild,*rchild;
  struct relation *relptr;           /* ptr to relation having path as tip */
} PATH;

typedef struct RelMem
{
  int coef;
  unsigned pathlength;
  short init,term;
  gaget *edgelist;
}RELMEM;

typedef struct overlaps
{
  unsigned long  *masklist;
  struct relation *rel;
  struct overlaps *next;
} OVERLAPS;

typedef struct relation
{
  short pmt;       /* If may be a minimal tip pmt is 1.
		      If is circled then pmt = 2.
		      If in box then pmt = 3. Otherwise pmt = 0. */
  unsigned int length;
  unsigned long index;
  unsigned int creationTime;

  RELMEM **mem;
  OVERLAPS *overPtr;
  PATH *tipPtr;
  struct relation **usedAt,*next;
} RELATION;

typedef struct rmatrix
{
  int numRows,numCols;
  RELATION *entry[MAXMATRIXSIZE];
} RMATRIX;

/*
typedef struct pth
{
   gaget *pathptr;
   int   init,term,length;
} PTH;
*/

typedef struct bg
{
  RELMEM  *paths[MAXLEVEL + 1];
  int  LastIndex[MAXLEVEL +1 ];     /* LastIndex[i] is the index of the
				       last edge of path i-1 in path i */
  int  level;
} BG;

typedef struct plist
{
  RELMEM *rm;
  RELATION *relPtr;
  int  LastIndex;
  struct plist *next;
} PLIST;

typedef struct temprel
{
  unsigned int RMgroups;
  RELATION rel;
} TEMPREL;
#ifndef NOPROTO    /* switch of prototyping for stupid compilers  */
/* ******************** PROTOTYPES ********************************** */
/* ***************************** grutil.c *************************** */
void pause(void);
int getfile(FILE **f1,char name[120],char name2[120],char *fext,char mode);
int rmLessThan(RELMEM *rm1, RELMEM *rm2);
int comparePaths(PATH *p1,PATH *p2);
int compareSubstring(gaget *ss,unsigned lengthS,PATH *pp);
int pMult(int arg1,int arg2);
int pAdd(int arg1,int arg2);
int pInverse(int arg);
int getint(void);
void fShowrelations(FILE *f1,PATH *root);
void fShowRelation(FILE *f1,RELATION *tempR,int extraline);
void fShowPath(FILE *f1,gaget *gPtr,int length,int ret);
void fShowRmatrix(FILE *f1,RMATRIX *mat);
int GetRelation(FILE *f1,TEMPREL *tr,int leadone);
/* ***************************** grmem.c **************************** */
int storeRel(TEMPREL *tr,PATH **root);
int CollectGarbage(PATH **root);
int enterTip(PATH **root);
int expandTempRel(TEMPREL *tr,unsigned numRelMem);
int newRelation(PATH **root,TEMPREL *tr);
void setRestart(void);
int getOverlap(OVERLAPS **ov,int numcells);
/* ***************************** grtree.c *************************** */
int insertPath(PATH *pp,PATH **tempP);
int deletePath(PATH *pp, PATH **top);
int tipSearch(gaget *g1,int length1,PATH **result,int projres);
void checkTree(RELATION *r1,PATH **root);
int seekSubstring(gaget *ss,unsigned lengthS,PATH *root,PATH **pp);
/* ***************************** grrelops.c *************************** */
void LeadOne(TEMPREL *tr);
void Reorder(TEMPREL *tr);
int PRPmult(RELATION *R1,TEMPREL *tr,gaget *pre1,gaget *post1);
int addRels(RELATION *r1,RELATION *r2,TEMPREL *tr,int coef);
int substring(gaget *g1,gaget *g2,int length1,int length2);
int BackReduce(RELATION *r,int projres);
int reduce(RELATION *r,unsigned index,int projres);
int copyTEMPREL(RELATION *r1, TEMPREL *trTwo);
int multRelations(RELATION *r1,RELATION *r2,TEMPREL **tr);
int overlap(gaget *g1,gaget *g2,int length1,int length2,int BigLength);

/* ******************************************************************* */


#endif     /* end of exclusion for no-prototype compilers */


