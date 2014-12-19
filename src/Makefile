CC     = gcc
CFLAGS = `pkg-config --cflags gtk+-2.0 gmodule-2.0` `libgcrypt-config --cflags`
LIBS   = `pkg-config --libs   gtk+-2.0 gmodule-2.0` `libgcrypt-config --libs`
DEBUG  = -Wall -g

OBJECTS = main.o callbacks.o fileconf.o confcrypt.o gui.o

.PHONY: clean

all: vpnconfig

vpnconfig: $(OBJECTS)
	$(CC) $(DEBUG) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

main.o: main.c support.h
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

callbacks.o: callbacks.c support.h
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

fileconf.o: fileconf.c support.h
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

confcrypt.o: confcrypt.c support.h
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

gui.o: gui.c support.h
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o main
