CC=cc
CFLAGS=-ansi -pedantic -Wall -Wextra -Werror -O2

flo: flo.o
	${CC} ${CFLAGS} -o flo main.c flo.o
flo.o: flo.c
	${CC} ${CFLAGS} -c flo.c
test: flo.o
	${CC} -o test test.c flo.o
clean:
	rm -f flo test flo.o
