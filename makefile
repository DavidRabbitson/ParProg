all: test

test: test.c
	gcc test.c -o test

clear:
	rm *.exe
