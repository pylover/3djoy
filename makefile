CC = gcc
KERNEL = $(shell uname -r)
LIBS = -lm
CFLAGS = -Wall -I. -I/usr/src/linux-headers-$(KERNEL)/include
OBJECTS = common.o cli.o js.o tty.o gcode.o timer.o output.o
PREFIX := /usr/local
EXEC := 3djoy

3djoy: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
js.o: js.c js.h common.h
tty.o: tty.c tty.h common.h
gcode.o: gcode.c gcode.h js.h common.h
timer.o: timer.c timer.h common.h
output.o: output.c output.h tty.h common.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS) $(EXEC)


.PHONY: install
install: $(EXEC)
	install -m 755 $(EXEC) $(DESTDIR)$(PREFIX)/bin


.PHONY: test
test: $(EXEC)
	./3djoy /dev/ttyACM0

