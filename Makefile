

##------------------------------------------------------------------------
## Auxiliary Rules and Actions
##------------------------------------------------------------------------

.PHONY : docs

all: app docs

app:
	$(MAKE) -C build/ all

clean:
	$(MAKE) -C build/ clean
	$(MAKE) -C docs/ clean

docs:
	$(MAKE) -C docs/ docs

setup:
	bin/project.sh

fromclean: clean all

