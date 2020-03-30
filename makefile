CC = gcc
KERNEL = $(shell uname -r)
CFLAGS = -Wall -I. -I/usr/src/linux-headers-$(KERNEL)/include
LIBS = -lm
OBJECTS = common.o cli.o js.o tty.o
PREFIX := /usr/local
EXEC := 3djoy

3djoy: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
js.o: js.c js.h
tty.o: tty.c tty.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS) $(EXEC)


.PHONY: install
install: $(EXEC)
	install -m 755 $(EXEC) $(DESTDIR)$(PREFIX)/bin


.PHONY: test
test: $(EXEC)
	./3djoy /dev/input/js0

