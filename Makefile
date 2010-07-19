f: f.c
	cc f.c -Wall -o f -O2
clean:
	rm -rf f *.o
