9cc: 9cc.c

test: 9cc
	chmod a+x ./test.sh && ./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

debug:
	gcc -g -O0 -o 9cc 9cc.c && gdb 9cc
