#include "grs.h"
#include "grglobal.h"

/* ************************************************************** */
static int gLessThan(gaget *g1, gaget *g2, int length)
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

int rmLessThan(RELMEM *rm1, RELMEM *rm2)
{
  int length1,length2;
  
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

int comparePaths(PATH *p1,PATH *p2)
/* 
  length is the minimum of p1->length and p2->length.  If length is
  greater than 0, the first length elements of p1 and p2 are compared
  until a difference is detected. 1 is returned if this element of p1
  is less than the same element of p2. Otherwise 2 is returned.  If
  all elements are equal 0 is returned.
*/
{
  int length;
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


int compareSubstring(gaget *ss,int lengthS,PATH *pp)
/*
  compares string ss to the path edges.  Returns 0 if identical as far
  as minimum of lengths, 1 if ss is less, and 2 if ss is greater.
  Length must be > 0.
*/
{
  int length;
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

/* ********************************************************************** */
