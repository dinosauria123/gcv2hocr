PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

MKDIR = mkdir -p
CP = cp -v
GCC = gcc -std=c99

gcv2hocr: main.c
	$(GCC) -o $@ $<

clean:
	$(RM) gcv2hocr

install: gcv2hocr
	$(MKDIR) $(BINDIR)
	$(CP) gcv2hocr $(BINDIR)

uninstall:
	$(RM) $(BINDIR)/gcv2hocr
