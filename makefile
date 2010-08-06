CC=cc
CFLAGS=-ansi -pedantic -Wall -Wextra -O2

flo: main.c flo.c flo.o
	${CC} ${CFLAGS} -o flo main.c flo.o
flo.o: flo.c flo.h
	${CC} ${CFLAGS} -c flo.c
test: flo.o
	${CC} -o test test.c flo.o
clean:
	rm -f flo test flo.o
