 extern short prime;
 extern short NoFallBack;
 extern unsigned SHIFT,MASK;

 extern EDGE EdgeList[MaxNumEdges];

 extern MEMADJLST **AdjLst;
 extern MEMADJLST **rAdjLst;
 extern MEMADJLST **rpAdjLst;
 extern MEMADJLST *freeALmem;

 extern short **matrix;                   /* matrix of graph */
 extern short **pmatrix;                  /* matrix of graph */

 extern int translator[256];

 extern unsigned int ***tab,***table;
 extern RELATION *firstR;
 extern RELATION *lastR;
 extern RELATION *generatorList;

 extern PATH *rootPath,*PRrootPath;

 extern int numberEdges;
 extern int numberVertexes;
 extern unsigned  maxlength;
 extern int automatic;

 extern char name1[60],namea[60],nameb[60];

 extern TEMPREL *tr0,*tr1,*tr2,*tr3,*tr4,*tr5,*trtemp;
 extern TEMPREL tempRel[6];
 extern BG bgm;

 extern gaget *pre,*post,*list,*extpre,*extpost;
 extern  RMATRIX *matrix1,*matrix2,*tmatrix; 

 extern unsigned long rIndex;
 extern unsigned int rtime;
 extern int rMark;
 extern int homogeneous;
 extern double **rows,**ppmatrix;
 extern TEMPREL X[10],T[10];
 extern unsigned long mask[32], EndMask[32];

 extern PLIST **Overhang[MAXLEVEL + 1],*plistList;
