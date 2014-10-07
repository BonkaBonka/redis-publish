#

CFLAGS?=-Wall -pedantic -Os -s -std=c99
PREFIX?=/usr/local

HIREDIS?=hiredis

BINS?=redis-publish
MANS?=redis-publish.1

%: %.c $(HIREDIS)/libhiredis.a
	$(CC) $(CFLAGS) -I$(HIREDIS) $^ -o $@

%.1: %.1.ronn
	ronn --warnings --roff $<

all:	$(BINS) $(MANS)

$(HIREDIS)/hiredis.h:
	git submodule init
	git submodule update

$(HIREDIS)/libhiredis.a: $(HIREDIS)/hiredis.h
	sh -cx "cd $(HIREDIS); make libhiredis.a"

install:	all
	mkdir -p "$(PREFIX)/bin/"
	cp $(BINS) "$(PREFIX)/bin/"
	mkdir -p "$(PREFIX)/share/man/man1/"
	cp $(MANS) "$(PREFIX)/share/man/man1/"

clean:
	rm -f $(BINS) $(MANS)
