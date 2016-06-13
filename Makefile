PROGRAMS = argc argv confirm toseconds forever noop write-file
PREFIX = /usr

build: $(PROGRAMS)

install:
	install -m 0755 $(PROGRAMS) $(PREFIX)/bin

$(PROGRAMS):
	gcc $@.c -o $@

clean:
	rm -f $(PROGRAMS)

