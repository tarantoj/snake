CC? = clang
CFLAGS? = -Wall -O3 -march=native
LIBS = -lncurses
BINDIR?= $(PREFIX)/bin


snake: main.c
	$(CC) $(CFLAGS) -o snake main.c $(LIBS)

debug: CFLAGS += -g
debug: snake

clean:
	rm -f snake

install: snake
	test -d $(DESTDIR)$(BINDIR) || mkdir -p $(DESTDIR)$(BINDIR)
	install -m 0755 snake $(DESTDIR)$(BINDIR)/

