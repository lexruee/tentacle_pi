all:
	gcc -Wall -c tsl2561.c -o tsl2561.o -lm
	gcc -Wall tsl2561.o test.c -o test.o -lm

clean:
	rm *.o > /dev/null 2>&1 &
