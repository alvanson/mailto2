PROJECT = mailto
VERSION = 1.3

PREFIX = /usr/machine/etc/httpd

BINDIR = $(PREFIX)/cgi-bin

SOURCES = mailto.c util.c
OBJECTS = mailto.o util.o
TARGETS = mailto

INSTALL = bsdinst
INSTALL_PROGRAM = $(INSTALL) -s -m 555

all: $(TARGETS)

mailto: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

install: all
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	$(INSTALL_PROGRAM) mailto $(BINDIR)

clean:
	rm -f *.o core $(TARGETS)

dist: clean
	cd ..; 	tar cf - $(PROJECT)-$(VERSION) | gzip -9 >$(PROJECT)-$(VERSION).tar.gz

check:
	lint -u $(SOURCES)
	gcc -Wall -fsyntax-only $(SOURCES)

depend:
	makedepend -- $(CFLAGS) $(COPTS) -- $(SOURCES)

