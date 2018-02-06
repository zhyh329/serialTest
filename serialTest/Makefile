# serialTest Makefile, quickly done by hand to get rid of all the QT stuff.
# this can be further optimized.

CFLAGS=-Wall -g -O2
LDFLAGS=-L 

serTest: serialTest.o ATOLog.o SetTimer.o
	gcc $(LDFLAGS)  $(CFLAGS) -o serTest serialTest.o ATOLog.o SetTimer.o -lrt

serialTest.o: serialTest.c serialTest.h typedef.h
	gcc $(CFLAGS) -c serialTest.c

ATOLog.o: ATOLog.c
	gcc $(CFLAGS) -c ATOLog.c

SetTimer.o: SetTimer.c
	gcc $(CFLAGS) -c SetTimer.c

clean:
	rm *.o serTest

