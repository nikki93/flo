CC=cc
CFLAGS=-ansi -pedantic -Wall -Wextra -O2

flo: flo.o
	${CC} ${CFLAGS} -o flo flo.o
flo.o: flo.c flo.h
	${CC} ${CFLAGS} -c flo.c
clean:
	rm -f flo flo.o
