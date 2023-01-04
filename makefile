CC = gcc
FLAG = -g -Wall

all: parta watchdog partb

parta: ping.o
	$(CC) ping.o -o parta

watchdog: watchdog.o
	$(CC) watchdog.o -o watchdog

partb: better_ping.o
	$(CC) better_ping.o -o partb

%.o: %.c.
	$(CC) $(FLAGS) -c $@ $<

.PHONEY: all clean

clean:
	rm -f *.o parta watchdog partb
