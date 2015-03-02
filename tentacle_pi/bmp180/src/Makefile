all:
	gcc -Wall -c bmp180.c -o bmp180.o -lm
	gcc -Wall bmp180.o test.c -o test.o -lm

clean:
	rm *.o > /dev/null 2>&1 &
