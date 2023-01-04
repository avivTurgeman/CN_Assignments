all: parta watchdog partb
parta: ping.o
	gcc ping.o -o parta
watchdog: watchdog.o
	gcc watchdog.o -o watchdog
partb: better_ping.o
	gcc better_ping.o -o partb
ping.o: ping.c
	gcc -Wall -c ping.c -o ping.o
watchdog.o: watchdog.c
	gcc -Wall -c watchdog.c -o watchdog.o
better_ping.o: better_ping.c
	gcc -Wall -c better_ping.c -o better_ping.o
.PHONEY: all clean

clean:
	rm -f *.o parta watchdog partb
