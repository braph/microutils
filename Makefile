PROGRAMS = argc argv confirm toseconds forever noop respawn write-file rmempty uniq_stable ret
PREFIX = /usr

build: $(PROGRAMS)

install:
	install -m 0755 $(PROGRAMS) $(PREFIX)/bin

$(PROGRAMS):
	gcc -O2 $@.c -o $@

clean:
	rm -f $(PROGRAMS)

