flo:
	cc -Wall -O2 -c flo.c
	cc -o flo main.c flo.o
test: flo
	cc -o test test.c flo.o
clean:
	rm -f flo test flo.o
