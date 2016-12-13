PROGRAMS = argc argv confirm toseconds forever noop respawn write-file rmempty
PREFIX = /usr

build: $(PROGRAMS)

install:
	install -m 0755 $(PROGRAMS) $(PREFIX)/bin

$(PROGRAMS):
	gcc $@.c -o $@

clean:
	rm -f $(PROGRAMS)

