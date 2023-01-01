all: ping watchdog new_ping
ping: ping.c
	gcc ping.c -o parta
watchdog: watchdog.c
	gcc watchdog.c -o watchdog
new_ping: new_ping.c
	gcc new_ping.c -o partb

clean:
	rm -f *.o parta watchdog partb
