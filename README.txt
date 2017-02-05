GRB   Version 1.0


Copyright (c) 1990  All rights reserved.  C.D. Feustel and E.L.Green
Virginia Polytechnic Institute and State University
Blacksburg,  Virginia 24061
USA
    

OVERVIEW

       This program provides a computational basis to do algebraic and
homological manipulations on algebras  and modules.   The algebras
in question should be given as a factor of a path algebra (which includes
factors of free algebras).   Things provided by the current program:
          1.  A basis of the algebra  (if finite dimensional).
          2.  The reduced Groebner basis for the ideal of relations of
the algebra.
          3.  The Cartan matrix and determinant of the algebra (if finite
dimensional).
          4.   A package that allows one to add, multiply  elements of the
path algebra and then reduce by the ideal of relations.
          5.  Construction of the higher Gammas (overlaps) described in
Anick - Green "On the homology of path algebras", Comm. Alg 15, 1987.
          6.  Construction of minimal projective resolutions of finite dimensional
modules over finite dimensional algebras.
          7.  Coefficients of the Hilbert and Poincare' series of an algebra.


BASICS ON RUNNING   GRB

     If you compile the code, the name of the executable program
is  GRB for workstations (unless you change the Makefile) and for 
TurboC++.  If we have sent you a compiled version for an IBM compatible,
then the executable file is  GRB1 for  an 8086 coprocessor and GRB2 for
an 80286 or 80386 machine.  
     Before running the program you must have  three ASCII data files
(or more) ready for the program.   There must be a  *.gph file which encodes 
the graph,  a *.rel file which contains a set of generators of the ideal of relations 
of the algebra and *.mod  which encodes a module via a projective presentation
of the module.  (If you do not run the resolution option, the *.mod file
is NOT necessary.) 

     i).  The  *.gph  file:   Let  G denote the graph for the path algebra.  For
example, if  you are studying the free algebra in  n  variables, then G would be 
the graph with one vertex and  n  loops.   More generally,  we assume  G has
v  vertices .   Label the vertices (1), (2), ... , (n).  [The program uses this 
notation.]    Assume  we have   a(i,j) arrows from vertex (i) to vertex (j).   Then
*.gph  will have the following form:
     The first line  will be :    v = vertices
      The  rest of the file will be the nonnegative integers  a(i,j) , separated by 
at least one space,  entered  row by row:  
                                    a(1,1)  a(1,2) ...  a(1,v)
                                   a(2,1)  ....             a(2,v)
                                               .......
                                    a(v,1)  ...              a(v,v)

Examples:
         ex1.gph
--------------------
1 = vertex
3
----------------------
ex1.gph  is the graph with one vertex and three loops.   The path algebra, KG 
for this graph is the free algebra in three noncommuting variables.

        ex2.gph
-------------------
3 = vertices
0  2  0
0  1  1
1  1  0
--------------------
ex1.gph is the graph with 3 vertices and 6 arrows;  two arrows from
(1) to (2),  one loop at (2),  an arrow from (2) to (3), an arrow from (3)
to (1) and from (3) to (2).

     ii).  The  *.rel  file:   To understand the output and input you must
understand how the program reads and writes linear combinations of paths.

          ii) a).    The field K:   The program allows only finite fields
of the form the integers modulo a prime  p (with the prime   p  smaller than
the largest integer accepted by the computer).   When the program is run,
this prime is chosen.   To work over characteristic  0,  choose a large prime 
like  32117  (and hope that the prime will not affect the computations!!).
To enter elements of the field,  just enter an integer between -p and p  in
the equivalence class of the element.

          ii) b).  Paths:  The program labels the arrows in the graph
G  by reading  left to right, row by row.  It calls the arrows sequentially a,b,c,... 
.
Thus, in example  ex1.gph,  the three loops are labelled a,b and c.  In example
ex2.gph,  the arrows are labelled as follows:  a and b are arrows from (1) to (2),
c is the loop at (2),  d is the arrow from (2) to (3),  e is the arrow from (3) to (1) 
and f is the arrow from (3) to (2).
     Paths are entered as a sequence of arrows (using their letter names)
with the left most arrow being the first arrow, etc.   For example,   in the graph
of example ex2.gph,    ac  is an allowable path  (first  arrow a from (1) to (2) and
the  arrow c from (2) to (2)) whereas  ca  is not an allowable path.    The semi-colon
is used to end a linear combination of (allowable) paths and the coefficients from
K can be entered next to the path it multiplies or separated by a '*'.  (Note:  to
enter  a loop, say c, followed by itself three you must use  ccc;  since we have 
no exponential operator.)

     Examples of relations for the graph in ex2.gph:
7*(1) + 4*ade;                            This is 7 times the vertex (1) plus 4 times 
                                              the path ade
3c - 5eacdf;                              This is 3 time the path c minus 5 times 
                                              the path eacdf
 
               
               iic): The files *.rel: We give two examples of relation files, one 
for the graph in ex1.gph called ex1.rel and one for the graph in ex2.gph called ex1.rel.
Note that the entries in these files should be Z/P - linear combinations of paths - each
path in a given linear combination should begin at one fixed vertex and end at a
possibly different vertex.  Remember to end the relations with a semicolon.  Finally,
if you are interested in a finite dimensional algebra in which all paths of length
N+1 are in the ideal I you need not enter these into *.rel since the program will
do this for you.  Thus, you need only enter linear combinations of paths whose length
is at most N.

ex1.rel
---------------------
5aabc - 4aac;
ab - ba;
ccccc;



ex2.rel
----------------------------
ade - bde;
ccc- 3ccc;
(3) + ebcd;


              Both of these examples have 3 different relations in them.

    
              iii) The file *.mod:  We enter modules via a projective presentation.
If R = KG/I is the ring in question (where G is the graph given in the *.gph file
and I is the ideal generated by the relations in the *.rel file) the projectives
we are interested in are of the form  (i)R, where (i) is a vertex (and hence 
idempotent).  Note the we are considering right R-modules.  An R-homomorphism F
from (i)R to (j)R is completely determined by the value of F((i)) in (j)R.  Note
elements of (j)R are classes modulo I of  Z/p-linear combinations of paths that
start at vertex (j) and end at vertex (i).   Thus, to determine an R-homomorphism
from  (i_1)R + ... + (i_n)R  to   (j_1)R + ... + (j_m)R  we need an nxm matrix

M = [m_(a,b)]

where m_(a,b) is the entry in the a^th row and b^th column.  The entry m_(a,b) 
must be a Z/p -linear combination of paths from vertex (b) to vertex (a).  Again
we use the rules for entering relations described above.  Finally the first
line of the file *.mod must be the two integers, m  n , where m is the number
of rows in M and n is the number of columns in M.

Examples:
ex1.mod
__________
1 3
a; b; c;

We are using ex1.gph for the graph (one vertex with three loops a, b and c).
This corresponds to F:(1)R+(1)R+(1)R -> (1)R  where F((1),0,0) = a, 
F(0,(1),0) = b and F(0,0,(1)) = c.  It is not hard to see that the right module
presented by F is the simple module Z/p.


ex2.mod
--------
2 2
ac-cc;       ;
ad + bd;  ccd; 

We are using ex2.gph for the graph.  This corresponds to 
F:(2)R+(3)R -> (1)R+(2)R where
F((2),0) = (ac-cc, 0)
F(0, (3)) = (ad+bd, ccd)
Here the module is cokernel of F - of course it is more complicated to see
which module this is since among other things it is dependent on the relations.


                   iv): THE ACTUAL RUNNING:

Type grb [enter]  (or grb1 or grb2 if you are given compiled programs for the pc).

The program asks for the characteristic: you should enter a prime between 2 and 
the largest integer allowed by your machine.

The program asks for the maximum path length.  If you are dealing with a finite 
dimensional algebra with index N you should enter N.  THE PROGRAM AUTOMATICALLY
ENTERS ALL PATHS OF LENGTH N+1 INTO THE SET OF RELATIONS GENERATING THE IDEAL I.
If you are dealing with a nonfinite dimensional algebra, you should enter 
an integer - the program will IGNORE ALL PATH OF LENGTH N+1.  Thus, it will find
the elements of a Groebner basis all of whose paths have length at most N and it
will calculate various things, such as a basis of the algebra - up to paths of
length N.  
WARNING:  On a pc, the larger N is the more likely you are to run out of memory.
Even on a workstation, since the number of paths grows exponentially, if N is
very large, you might have memory problems.

The program asks if you are using the descriptor  gr.   If you type 1 [enter]
then  gr is the default descriptor (which will be explained in a bit).  If
you type 2 [enter] the program asks you to type in a descriptor - for example
ex2 [enter].  We will call the descriptor  DES  (either gr or ex2 in the above
example).

Next the program asks if the descriptor is fixed.  If you type 1 [enter] then 
program will look for the files DES.gph, DES.rel and DES.mod.  Thus, if you have
entered all the *.gph, *.rel and *.mod using one fixed filename, the program will
look for that filename automatically unless you tell it not to or you change the
descriptor.  If you type 2 [enter] then the program will allow you to enter
different file names for *.gph, *.rel and *.mod each time the program needs to
read one.

The program proceeds to read your *.gph file (either with DES.gph or one you have
entered).  It reads the file and displays the arrows and their ordering.  The
default ordering is alphabetical - with the arrows read in as described above.
At this point in the program you have a chance to change the ordering as the 
menu describes.  (Typing n [enter] keeps the default ordering.)

The program proceeds to read the DES.rel file (or one you enter).  Then the program
creates the reduced Groebner basis which we call the minimal sharp elements.  When
this is complete you are given a number of options which we explain below.
                             
COMPILING  GRB

     Compiling from code:  We have provided all the necessary files
for the program to be compiled on an ANSI  C compiler that allows 
prototyping.  If one is using Borland's  TurboC++ the program should compile
directly.  If one is using another compiler, one must create the following
empty files:

alloc.h         dos.h      stdlib.h    conio.h     io.h

If using a DEC workstation:  in the file grdef.h,  keep the first line uncommented.
If using a pc, make the first line a comment (by surrounding it by /*    and  */).
We include a Makefile.  A Makefile ( or  .prj  - in TurboC) must include all *.c
files and all four  gr*.h files must be accessible.


 
