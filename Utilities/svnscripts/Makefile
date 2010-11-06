.PHONY: install

DESTDIR=$(HOME)/bin

install: fmdiff svnchanges svnunknowns svndiff
	install -d $(DESTDIR)
	install fmdiff $(DESTDIR)/fmdiff
	install	svnchanges $(DESTDIR)/svnchanges
	install	svnunknowns $(DESTDIR)/svnunknowns
	install	svndiff $(DESTDIR)/svndiff
