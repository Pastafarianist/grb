/* ************************************************************************
 * Copyright (c) 1990-1991 All rights reserved. C.D.Feustel,Edward Green, *
 * Virginia Polytechnic Institute and State University                    *
 **************************************************************************/
/*	Grnew.C   */
#include "grs.h"
#include "grglobal.h"
/* ****************************************************************** */
#ifdef NOPROTO
static int RelMatMult(mat1,mat2,mat3, projres)
RMATRIX *mat1, *mat2, *mat3;
int projres;
#else
static int RelMatMult(RMATRIX *mat1,RMATRIX *mat2,
					      RMATRIX *mat3,int projres)
#endif
/*
  It is assumed that mat3 is clear.
*/
{
  int i,j,k,ii,index;
  RELATION *r1,*r2;

  IF mat1->numCols != mat2->numRows
  THEN
    PRINTF("\n\n Number of rows of mat2 <> number of cols of mat1.\n");
    PRINTF(" Program aborted. \n");
    return(1);
  ENDIF

  index = -1;

  for(i = 0; i < mat1->numRows;++i)
  for(j = 0; j < mat2->numCols;++j)
  {
    ++index;
    tr0 ->rel.length = 0;

    for(k = 0; k < mat1 -> numCols; ++k)
    {
      IF (r1 = mat1->entry[k+i*mat1->numCols])==NULL THEN continue; ENDIF
      IF (r2 = mat2->entry[j+k*mat2->numCols])==NULL THEN continue; ENDIF
      IF multRelations(r1,r2,&tr1) THEN return(1); ENDIF

      IF addRels(&tr0->rel,&tr1->rel,tr2,1) == 1 THEN return(1); ENDIF
      trtemp = tr2;
      tr2 = tr0;
      tr0 = trtemp;
    }

    IF (ii = reduce(&tr0->rel,0,projres)) == 1 THEN return(1); ENDIF
    trtemp = ii == 2 ? tr1:tr0;

    IF trtemp -> rel.length 
    THEN
      IF storeRel(trtemp,&PRrootPath) THEN return(0); ENDIF
      mat3->entry[index] = lastR;
      lastR -> usedAt = &(mat3->entry[index]);
      lastR -> pmt = 5;
    ELSE
      mat3 -> entry[index] = NULL;
    ENDIF
  }

  mat3 -> numRows = mat1->numRows;
  mat3 -> numCols = mat2->numCols;

  return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
int NullMatrix()
#else
int NullMatrix(void)
#endif
/*
  returns 0 if matrix1 * matrix2 is the zero matrix.
  returns 1 if there is not enough space.
  returns 2 if the matrix is not zero.
*/
{
  int i,j,val = 0,index = -1;

  IF RelMatMult(matrix1,matrix2,tmatrix,1)
  THEN
    PRINTF(" Memory problems in RelMatMult. \n");
    return(1);
  ENDIF

  for(i = 0; i < tmatrix->numRows; ++i)
  for(j = 0; j < tmatrix->numCols; ++j)
  {

    IF tmatrix->entry[++index] != NULL
    THEN
      val = 2;
      tmatrix->entry[index] -> usedAt = NULL;
      tmatrix->entry[index] -> pmt = 0;
      tmatrix->entry[index] = NULL;
    ENDIF
  }

  tmatrix -> numCols = 0;
  tmatrix -> numRows = 0;

  return(val);
}
/* *************************************************************  */
#ifdef NOPROTO
void transpose(mat1, mat2)
RMATRIX **mat1, **mat2;
#else
void transpose(RMATRIX **mat1, RMATRIX **mat2)
#endif
/*
  Use mat2 to transposes mat1 and returns the result in mat1.  mat2
  is assumed to be clear.  mat1 and mat2 must be distinct.
*/
{
  int i,j,index1,index2;
  RMATRIX *temp;
  RELATION *r1;

  (*mat2)->numRows = (*mat1)->numCols;
  (*mat2)->numCols = (*mat1)->numRows;

  for(i=0;i<(*mat1)->numRows;++i)
  for(j=0;j<(*mat1)->numCols;++j)
  {
    index1 = i*(*mat1)->numCols + j;
    index2 = j*(*mat1)->numRows + i;
    r1 = (*mat2)->entry[index2] = (*mat1)->entry[index1];
    if(r1 != NULL) r1 -> usedAt = &(*mat2)->entry[index2]; 
  }

    for(i = 0; i < (*mat1) -> numRows; ++i)
    for(j = 0; j < (*mat1) -> numCols; ++j)
    {
      index1 = i * (*mat1) -> numCols + j;
      (*mat1) -> entry[index1] = NULL;
    }

  (*mat1) -> numRows = 0;
  (*mat1) -> numCols = 0;

  temp = *mat2;
  *mat2 = (*mat1);
  (*mat1) = temp;

  return;
}
/* *************************************************************  */
#ifdef NOPROTO
void clearRmatrix(mat)
RMATRIX *mat;
#else
void clearRmatrix(RMATRIX *mat)
#endif
{
  int i,j,index;

  index = -1;
  for(i=0;i<mat->numRows;++i)
  for(j=0;j<mat->numCols;++j)
  {
    if(mat->entry[++index] ==NULL) continue;

    mat->entry[index] -> pmt = 0;
    mat->entry[index]-> usedAt = NULL;
    mat->entry[index] = NULL;
  }

  mat -> numRows = 0;
  mat -> numCols = 0;

  return;
}
/* *************************************************************  */
#ifdef NOPROTO
int RelToMatrix(mat, numRows)
RMATRIX *mat;
int numRows;
#else
int RelToMatrix(RMATRIX *mat, int numRows)
#endif
/*
  Creates the Rmatrix given the relations as an ordered list
  beginning with firstR.  The number of rows must be given.
  The number of columns will be the number of relations in the
  relation list.
*/
{
  int i,j = -1,k=0,coef;
  unsigned index1,index;
  RELATION *r,*r1;
  RELMEM  *rm,*rm1;
  gaget *g1,*g2,*gbd;

  r = firstR;

  while(r != NULL)
  {
    if(r->pmt ==1) ++k;
    r = r -> next;
  }

  IF k * numRows > MAXMATRIXSIZE
  THEN
    PRINTF(" Result matrix in RelToMatrix too large. Program aborted.\n");
    return(1);
  ENDIF

  mat -> numCols = k;
  mat -> numRows = numRows;

  r = firstR;

  while(r != NULL) 
  {
    IF r->pmt == 1
    THEN
      ++j;
      index1 = 0;
      r -> usedAt = &r;

      while(index1 < r -> length)
      {
	rm = r -> mem[index1>>SHIFT] + (index1%MASK);

	IF (index = (unsigned) (j + k * (rm->edgelist[0] - numberEdges)))
					  > MAXMATRIXSIZE
	THEN
	  PRINTF(" Result matrix in RelToMatrix too large. Program aborted.\n");
	  return(1);
	ENDIF

	(r1 = &tr0 -> rel) -> length = 1;
	(rm1 = r1 -> mem[0]) -> pathlength = rm -> pathlength - 1;
	rm1 -> term = rm -> term;
	rm1 -> init = EdgeList[rm->edgelist[0]].term;
	rm1 -> coef = rm -> coef;
	gbd = (g1 = rm1 -> edgelist) + rm1->pathlength;
	g2 = rm->edgelist + 1;

	while(g1 < gbd) *g1++ = *g2++;                            

	IF (r1 = mat -> entry[index]) != NULL
	THEN
	  r1 -> pmt = 0;
	ENDIF

	IF addRels(r1,&tr0->rel,tr1,1) THEN return(1); ENDIF
	IF storeRel(tr1,&PRrootPath) THEN return(1); ENDIF

	lastR -> pmt = 5;
	lastR -> usedAt = &(mat -> entry[index]);
	mat -> entry[index] = lastR;
	++index1;
      }

      r -> usedAt = NULL;
    ENDIF

    r = r -> next;
  }
  return(0);
}
/* *************************************************************  */
#ifndef NOPROTO
int getALMem(MEMADJLST **temp);
int nonhomo(TEMPREL *tr);
#endif
/* *************************************************************  */
#ifdef NOPROTO
int RelFromMatrix(mat)
RMATRIX *mat;
#else
int RelFromMatrix(RMATRIX *mat)
#endif
/*
  Creates relations given the Rmatrix.
*/
{
  int ind,i,j,coef,*pcoef;
  unsigned index;
  RELATION *r,*r1;
  MEMADJLST *temp;

  if(numberEdges + mat-> numRows > MaxNumEdges) return(1);

/* For each row of matrix add and edge to rpAdjLst. */

  for(i = 0; i < mat -> numRows; ++i)
  {
    for(j = 0; j < mat -> numCols; ++j)
    {
      index = (unsigned) (i * mat->numCols + j);
      if((r = mat -> entry[index]) == NULL) continue;
      if(r->length==0) continue;
      if(getALMem(&temp)) return(1);
      temp -> EdgeNum = numberEdges + i;
      temp -> term = numberVertexes;
      ind = EdgeList[r -> mem[0] -> edgelist[0]].init;
      temp -> Next = rpAdjLst[ind];
      rpAdjLst[ind] = temp;
      break;
    }
  }

  for(j = 0; j < mat -> numCols; ++j)
  {
    tr0->rel.length = 0;

    for(i = 0; i < mat -> numRows; ++i)
    {
      /* The following two assignments must be inside the loop. */
      pre[0] = 1;
      post[0] = 0;
      pre[1] = numberEdges + i;
      index = (unsigned) (i * mat->numCols + j);

      IF mat->entry[index] == NULL THEN continue; ENDIF

      IF PRPmult(mat->entry[index],tr1,pre,post)
      THEN
	PRINTF("\n Memory problems in RelFromMatrix. \n\n");
	return(1);
      ENDIF

      IF addRels(&tr0->rel,&tr1->rel,tr2,1) THEN return(1); ENDIF

      trtemp = tr2;
      tr2 = tr0;
      tr0 = trtemp;
    }

    IF !((r = &(tr0 -> rel))->length) THEN continue; ENDIF

    IF (coef = r -> mem[0] -> coef) != 1
    THEN
      coef = pInverse(coef);
      r -> mem[0] -> coef = 1;

      index = 1;

      while (index < r -> length)
      {
	pcoef = &(r -> mem[index>>SHIFT][index&MASK].coef);
	*pcoef = pMult(*pcoef,coef);
	++index;
      }
    ENDIF

    IF (ind = BackReduce(&tr0->rel,1)) == 1
    THEN
      return(1);
    ELSE
      trtemp = ind == 2 ? tr1: tr0;

      if(trtemp->rel.length)
      IF newRelation(&PRrootPath,trtemp) THEN return(1); ENDIF
      if(nonhomo(trtemp)) homogeneous = 0;
    ENDIF
  }

  return(0);
}
/* *************************************************************  */
#ifdef NOPROTO
static int deleteRow(mat, rowNum)
RMATRIX *mat;
int rowNum;
#else
static int deleteRow(RMATRIX *mat,int rowNum)
#endif
{
  int i,j, OldIndex, NewIndex;
  RELATION *r1;

  IF rowNum > mat -> numRows-1 || rowNum < 0
  THEN
    PRINTF(" Number of deleted row out of range in deleteRow. \n");
    PRINTF(" %d = number rows %d = row index \n",mat->numRows,rowNum);
    PRINTF(" Program aborted. \n");
    return(1);
  ENDIF

  for(j = 0; j < mat -> numCols; ++j)
  {
    OldIndex = rowNum * mat->numCols + j;
    IF (r1 = mat->entry[OldIndex]) == NULL THEN continue; ENDIF
    r1 -> pmt = 0;
    mat->entry[OldIndex] -> usedAt = NULL;
    mat->entry[OldIndex] = NULL;
  }

  for(i = rowNum+1; i < mat -> numRows; ++i)
  for(j = 0; j < mat -> numCols; ++j)
  {
    OldIndex = i * mat->numCols + j;
    NewIndex = OldIndex - mat->numCols;

    IF (mat->entry[NewIndex]=r1=mat->entry[OldIndex])==NULL 
    THEN 
      continue; 
    ENDIF

    mat->entry[OldIndex] = NULL;
    mat->entry[NewIndex]->usedAt = &mat->entry[NewIndex];
  }

  --(mat->numRows);

  return(0);
}

/* *************************************************************  */
#ifdef NOPROTO
static int deleteColumn(mat, colNum)
RMATRIX *mat;
int colNum;
#else
static int deleteColumn(RMATRIX *mat,int colNum)
#endif
{
  int i,j, OldIndex = -1, NewIndex = -1;

  IF colNum > mat-> numCols-1|| colNum < 0
  THEN
    PRINTF(" Number of deleted column out of range in deleteColumn. \n");
    PRINTF(" %d = number columns %d = column index \n",mat->numCols,colNum);
    PRINTF(" Program aborted. \n");
    return(1);
  ENDIF

  for(i = 0; i < mat -> numRows; ++i)
  {
    for(j = 0; j < mat -> numCols; ++j)
    {
      ++OldIndex;
    
      if(j == colNum)
      {
	IF mat->entry[OldIndex] != NULL
	THEN
	  mat->entry[OldIndex] -> pmt = 0;
	  mat->entry[OldIndex] -> usedAt = NULL;
	  mat->entry[OldIndex] = NULL;
	ENDIF

	continue;
      }

      IF (mat->entry[++NewIndex] = mat->entry[OldIndex]) == NULL 
      THEN 
	continue; 
      ENDIF

      if(NewIndex != OldIndex)
      {
	mat->entry[NewIndex]->usedAt = &mat->entry[NewIndex];
	mat->entry[OldIndex] = NULL;
      }
      
    }
  }
  --(mat->numCols);

  return(0);
}

/* ************************************************************* */
#ifndef NOPROTO
int getALmem(MEMADJLST **temp);
#endif
/* ************************************************************* */
#ifdef NOPROTO
int useMatrx()
#else
int useMatrx(void)
#endif
{
  int i,j,k,index,indexE,chr;
  RELATION *r;
  RELMEM   *RM;
  PLIST    *PL,*PLtemp;
  MEMADJLST *temp,*MALtemp;

  IF numberEdges + matrix1->numCols > MaxNumEdges
  THEN
    PRINTF(" Too many edges to use matrx.  Program aborted.\n");
    return(1);
  ENDIF

  /* Create new edges */

  indexE = numberEdges - 1;
  chr = EdgeList[indexE].name;

  for(i=0; i < matrix1->numRows; ++i)
  {
    ++indexE;
    k = index = i * matrix1->numCols;

    while(k < index + matrix1->numCols && matrix1->entry[k] == NULL) ++k;

    IF k < index + matrix1->numCols THEN index = k; ELSE continue; ENDIF

    j = matrix1->entry[index]->mem[0] -> init;
    ++matrix[numberVertexes][j];

    if(chr!= '#'){ if(++chr=='z'+1){chr='A';} else if(chr =='Z'+1){chr='#';} }

    EdgeList[indexE].init = numberVertexes;
    EdgeList[indexE].term = j;
    EdgeList[indexE].iname = indexE;
    EdgeList[indexE].name = (char) chr;

    IF getALmem(&temp)
    THEN
      PRINTF(" Not enough memory in useMatrx edges. \n");
      return(1);
    ENDIF

    temp->EdgeNum = indexE;
    temp->term = j;
    temp->Next = AdjLst[numberVertexes];
    AdjLst[numberVertexes] = temp;
  }

  return(0);
}
/* ************************************************************* */
#ifndef NOPROTO
int rInverse(RELATION *r);
#endif
/* ************************************************************* */
#ifdef NOPROTO
static int eliminateVertex(i0, j0, first)
int i0,j0,first;
#else
static int eliminateVertex(int i0,int j0,int first)
#endif
{
  RELATION *r,*r1,*r2,*s;
  int i,j,index,ind;
  RMATRIX *tempM;

  r = matrix2 -> entry[i0*matrix2->numCols + j0];

  IF rInverse(r) THEN return(1); ENDIF
  IF storeRel(tr0,&PRrootPath) THEN return(1); ENDIF
  s = lastR;
  s->usedAt = &s;
  s->pmt    = 5;

  tmatrix -> numRows = matrix2 -> numRows;
  tmatrix -> numCols = matrix2 -> numCols;

  index = -1;

  for(i=0;i<matrix2->numRows;++i)
  {
/* r1 is product of matrix2[i][j0] with s */

    IF multRelations(matrix2->entry[j0+i*matrix2->numCols],s,&tr0)
    THEN
      return(1);
    ENDIF

    IF (ind = reduce(&tr0->rel,0,0)) == 1 THEN return(1); ENDIF;
    IF ind == 2 THEN trtemp = tr1; tr1 = tr0; tr0 = trtemp; ENDIF
    r1 = &tr0 -> rel;

    for(j=0;j < matrix2->numCols;++j)
    {
      IF i0 != i
      THEN
	IF multRelations(r1,matrix2->entry[j + i0 * matrix2->numCols],&tr1)
	THEN
	  return(1);
	ENDIF

	IF addRels(&tr1->rel,matrix2->entry[j + i * matrix2->numCols],tr5,-1)
	THEN
	  return(1);
	ENDIF
      ELSE
	IF multRelations(s,matrix2->entry[j + i0*matrix2->numCols],&tr1)
	THEN
	  return(1);
	ENDIF

	trtemp = tr1; tr1 = tr5; tr5 = trtemp;
      ENDIF

      IF (ind = reduce(&tr5->rel,0,0)) == 1 THEN return(1); ENDIF
      trtemp = ind == 2 ? tr1:tr5;
      ++index;

      IF trtemp -> rel.length
      THEN
	IF storeRel(trtemp,&PRrootPath) THEN return(1); ENDIF

	tmatrix->entry[index] = lastR;
	lastR -> usedAt = &tmatrix->entry[index];
	lastR -> pmt = 5;
      ELSE
	tmatrix->entry[index] = NULL;
      ENDIF
    }
  }

  s->usedAt = NULL;
  s->pmt    = 0;
  clearRmatrix(matrix2);
  tempM = matrix2;
  matrix2 = tmatrix;
  tmatrix = tempM;

/*    ***************************************           */
  tmatrix -> numRows = matrix2 -> numRows;
  tmatrix -> numCols = matrix2 -> numCols;

  index = -1;

  for(i=0;i<matrix2->numRows;++i)
  {
    for(j=0;j < matrix2->numCols;++j)
    {
      ++index;
      IF j0 != j
      THEN
	r1 = matrix2->entry[j0 + i * matrix2->numCols];

	IF multRelations(r1,matrix2->entry[j + i0*matrix2->numCols],&tr2)
	THEN
	  return(1);
	ENDIF
                             
	IF addRels(&tr2->rel,matrix2->entry[j+i*matrix2->numCols],tr5,-1) 
	THEN
	  return(1);
	ENDIF

	IF (ind = reduce(&tr5->rel,0,0)) == 1 THEN return(1); ENDIF;
	trtemp = ind == 2 ? tr1:tr5;
      ELSE
	r1 = matrix2->entry[j0 + i * matrix2->numCols];
	trtemp = tr5;
	if(r1 != NULL){IF copyTEMPREL(r1,trtemp) THEN return(1); ENDIF}
	else trtemp->rel.length = 0;
      ENDIF

      IF trtemp -> rel.length
      THEN
	IF storeRel(trtemp,&PRrootPath) THEN return(1); ENDIF

	tmatrix->entry[index] = lastR;
	lastR -> usedAt = &tmatrix->entry[index];
	lastR -> pmt = 5;
      ELSE
	tmatrix->entry[index] = NULL;
      ENDIF
    }
  }

  clearRmatrix(matrix2);
  tempM = matrix2;
  matrix2 = tmatrix;
  tmatrix = tempM;
/*   ******** */
  for(j=0;j< matrix2->numCols;++j)
  {
    IF j != j0
    THEN
      IF (r1 = matrix2->entry[j + i0 * matrix2->numCols]) != NULL
      THEN
	   PRINTF("***** %d %d ",i,j);fShowRelation(stdout,r1,1);
	   PRINTF(" problem in eliminateVertex, non null row element.\n");
	   return(1);
      ENDIF
    ENDIF
  }


  for(i=0;i< matrix2->numRows;++i)
  {
    IF i != i0
    THEN
      IF matrix2->entry[j0 + i * matrix2->numCols] != NULL
      THEN
	PRINTF(" problem in eliminateVertex, non null column element.\n");
	return(1);
      ENDIF
    ELSE
      IF (r1 = matrix2->entry[j0 + i * matrix2->numCols]) == NULL ||
	  r1 ->length != 1 ||
	  r1->mem[0]->pathlength != 0
      THEN
	PRINTF(" problem in eliminateVertex, %d %d element is not right.\n"
		       ,i0,j0);
	fShowRelation(stdout,r1,1);
	return(1);
      ENDIF
    ENDIF
  }


/*    ***************************************           */

  IF !first
  THEN
    IF deleteColumn(matrix1,i0) THEN return(1); ENDIF
  ENDIF

  IF deleteRow(matrix2,i0) THEN return(1); ENDIF
  IF deleteColumn(matrix2,j0) THEN return(1); ENDIF

  return(0);
}

/* *************************************************************  */

#ifdef NOPROTO
int removeVertexes(first)
int first;
#else
int removeVertexes(int first)
#endif
/*
  All vertexes in relations in matrix2 are to be eliminated.
  tmatrix is cleared and may be used for temporary storage.
*/

{
  int i,j,k,index;
  unsigned index1;
  RELMEM *rm;
  RELATION *r;
  RMATRIX *rmat;

  IF first
  THEN
    rmat = matrix1;
    matrix1 = matrix2;
    matrix2 = rmat;
  ENDIF

  another:
  IF !(matrix2 -> numRows * matrix2 -> numCols) THEN return(0); ENDIF

  for(i=0;i < matrix2->numRows;++i)
  for(j=0;j < matrix2->numCols;++j)
  {
    index = i * matrix2 -> numCols + j;

    IF (r = matrix2->entry[index])==NULL || !r-> length THEN continue; ENDIF

    index1 = 0;

    while(index1 < r -> length)
    {
      rm = r->mem[index1>>SHIFT] + (index1&MASK);

      IF rm -> pathlength == 0
      THEN
	IF eliminateVertex(i,j,first) THEN return(1); ENDIF;
	goto another;
      ENDIF

      ++index1;
    }
  }

  IF first
  THEN
    rmat = matrix1;
    matrix1 = matrix2;
    matrix2 = rmat;
  ENDIF

  return(0);
}
/* *************************************************************  */
