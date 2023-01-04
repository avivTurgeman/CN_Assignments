all: ping watchdog new_ping
parta: ping.o
	gcc ping.o -o parta
watchdog: watchdog.o
	gcc watchdog.o -o watchdog
partb: new_ping.o
	gcc new_ping.o -o partb

ping.o: ping.c
	gcc -Wall -c ping.c -o ping.o

watchdog.o: watchdog.c
	gcc -Wall -c watchdog.c -o watchdog.o

new_ping.o: new_ping.c
	gcc -Wall -c new_ping.c -o new_ping.o
.PHONEY: all clean

clean:
	rm -f *.o parta watchdog partb
