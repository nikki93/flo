f: f.c
	cc f.c -Wall -o f
clean:
	rm -rf f *.o
log:
	git log '--pretty=format:%ad %s (%cn)' --date=short > changelog
