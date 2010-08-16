DESTDIR?=
PREFIX?=/usr/local
CC=cc
CFLAGS=-ansi -pedantic -Wall -Wextra -O2

flo: flo.o
	${CC} ${CFLAGS} -o flo flo.o
flo.o: flo.c flo.h
	${CC} ${CFLAGS} -c flo.c
install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f flo ${DESTDIR}${PREFIX}/bin/flo
uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/flo
clean:
	rm -f flo flo.o
