CFLAGS= -s  -g
CC=gcc


grb: grmain.o gralg.o  grinit.o  grquogen.o  grmodule.o\
       grmem.o grrelops.o grtree.o grutil.o grnew.o grresolv.o
	${CC}  grmain.o gralg.o grinit.o grquogen.o  \
	grmem.o grrelops.o grtree.o grutil.o grmodule.o	\
        grnew.o grresolv.o -o grb
grmain.o: grglb.h  grglobal.h grs.h grdef.h
grinit.o:  grglb.h  grglobal.h grs.h grdef.h
gralg.o: grglb.h  grglobal.h grs.h grdef.h
grmem.o: grglb.h  grglobal.h grs.h grdef.h
grrelops.o: grglb.h  grglobal.h grs.h grdef.h
grtree.o: grglb.h  grglobal.h grs.h grdef.h
grutil.o: grglb.h  grglobal.h grs.h  grdef.h 
grquogen.o: grglb.h  grglobal.h grs.h grdef.h 
grmodule.o: grglb.h  grglobal.h grs.h grdef.h
grnew.o: grglb.h  grglobal.h grs.h grdef.h
grresolv.o: grglb.h  grglobal.h grs.h grdef.h

