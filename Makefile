
APPNAME ?= gravblur
SRCNAME ?= main.c

$(APPNAME): bitmap.h gravityblur.h pngio.h jpegio.h

MFLAGS := -march=native
CFLAGS := -Wall -Wextra -pedantic -O3 $(MFLAGS)
LIBS := -ljpeg -lpng

.PHONY: clean all

all: $(APPNAME)

clean:
	rm -f $(APPNAME)

$(APPNAME): $(SRCNAME)
	$(CC) -DAPPNAME=$(APPNAME) $(CFLAGS) -s -o $@ $< $(LIBS) -lm

