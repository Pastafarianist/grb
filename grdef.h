/* #define NOPROTO  */
#define gwkstn 


#ifdef gwkstn

/* #define clock_t int */
#define CLK_TCK 1000000.0
#define GETCHAR()  getchar()
#define GETCH()    getchar()
#define MULT            16
#define MAXMATRIXSIZE 3000000 

#else

#define GETCHAR()  getchar()
#define GETCH()    getch()
#define MULT            1
#define MAXMATRIXSIZE  500

#endif

#define MaxNumEdges       256*MULT   /* maximum number of edges */
#define MAXREL      (16*MULT)   /* number of relations in work page */
#define MAXRMPTR   (256*MULT)   /* number of relation ptrs in work page */
#define MAXRMLST     (8*MULT)   /* max number of relmems in sublist */
#define MAXRELMEM   (64*MULT)   /* number of relmems in work page */
#define MAXGAGET   (512*MULT)   /* number of gagets in work page */
#define MAXPATH     (16*MULT)   /* number of paths in work page */
#define MAXOVERLAP  (16*MULT)   /* number of overlaps in page  */
#define MAXLONG     (32*MULT)   /* number of longs in page      */
#define MAXLEVEL    10
#define MAXADJLST   (16*MULT)   /* number of mems in page       */
#define PLNUM        (8*MULT)   /* plist elements created at one time */


