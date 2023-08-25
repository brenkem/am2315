SENSOR=am2315

all:
	gcc -Wall -c $(SENSOR).c -o $(SENSOR).o -lm -li2c
	gcc -Wall $(SENSOR).o test.c -o test.o -lm -li2c

clean:
	rm *.o > /dev/null 2>&1 &
