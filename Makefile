PROJECT = mailto2
VERSION = 2.0.0

BINDIR = /usr/lib/cgi-bin

SOURCES = mailto.c
OBJECTS = mailto.o
TARGETS = mailto

CFLAGS = -O2 -Wall

INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -g root -o root -m 755 -s

all: $(TARGETS)

mailto: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

install: all
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(INSTALL_PROGRAM) mailto $(BINDIR)

clean:
	rm -f *.o core $(TARGETS)

dist: clean
	cd ..; tar cf - $(PROJECT)/* | gzip -9 >$(PROJECT)-$(VERSION).tar.gz

check:
	gcc -Wall -fsyntax-only $(SOURCES)
