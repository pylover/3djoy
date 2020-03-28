CC = gcc
#KERNEL = $(shell uname -r)
#CFLAGS = -I. -I/usr/src/linux-headers-$(KERNEL)/include
CFLAGS =-Wall -I.
LIBS = -lm
OBJECTS = common.o cli.o tty.o js.o
PREFIX := /usr/local
EXEC := 3djoy

3djoy: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
tty.o: tty.c tty.h common.h 
js.o: js.c js.h


.PHONY: clean 
clean:
	rm -f $(OBJECTS) $(EXEC)


.PHONY: install
install: $(EXEC)
	install -m 755 $(EXEC) $(DESTDIR)$(PREFIX)/bin
