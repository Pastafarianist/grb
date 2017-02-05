 short prime;
 short NoFallBack;
 unsigned SHIFT,MASK;

 EDGE EdgeList[MaxNumEdges];  /* edges by edge number */

 MEMADJLST **AdjLst; /* pointers at linked lists of
			edges beginning at vertex */
 MEMADJLST **rAdjLst; /* pointers at linked lists of
			edges beginning at vertex */

 MEMADJLST **rpAdjLst; /* pointers at linked lists of
			inverse edges ending at vertex */

 MEMADJLST freeALmem;  /* pointer at linked list of adj lst mems. */

 short **matrix;                   /* matrix of graph */
 short **pmatrix;                  /* matrix of graph */

 int translator[256];           /* translates characters to
				      corresponding path numbers */

 unsigned int ***tab,***table;

 RELATION *firstR;         /* pointers at first and last relations */
 RELATION *lastR;
 RELATION *generatorList,*endGeneratorList;

 PATH *rootPath,*PRrootPath;    /* pointers to roots of trees of tips */

 int numberEdges;
 int numberVertexes;
 unsigned maxlength;
 int automatic;

 char name1[60],namea[60],nameb[60];

 TEMPREL *tr0,*tr1,*tr2,*tr3,*tr4,*tr5,*trtemp;
 TEMPREL tempRel[6];
 RMATRIX *matrix1,*matrix2,*tmatrix;
 BG bgm;
 gaget *pre,*post,*list,*extpre,*extpost;

 unsigned long rIndex;
 unsigned int rtime;
 int rMark;
 int homogeneous;
 double **ppmatrix,**rows;
 TEMPREL X[10],T[10];
 unsigned long mask[32], EndMask[32];

 PLIST **Overhang[MAXLEVEL + 1],*plistList;

