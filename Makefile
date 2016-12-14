PROGRAMS = argc argv confirm toseconds forever noop respawn write-file rmempty uniq_stable
PREFIX = /usr

build: $(PROGRAMS)

install:
	install -m 0755 $(PROGRAMS) $(PREFIX)/bin

$(PROGRAMS):
	gcc $@.c -o $@

clean:
	rm -f $(PROGRAMS)

