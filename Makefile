include Makefile.inc

SUBDIRS		= examples lib
CLEANDIRS	= $(SUBDIRS:%=clean-%)
BUILDDIRS	= $(SUBDIRS:%=build-%)

all: $(BUILDDIRS)

$(DIRS): $(BUILDDIRS)

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

# Build examples only after lib
build-examples: build-lib

lib: 
	$(MAKE) -C lib

doc:
	$(MAKE) -C doc

examples:
	$(MAKE) -C examples

run: $(BUILDDIRS)
	LD_LIBRARY_PATH=lib/ examples/cpp_example


clean: $(CLEANDIRS)

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean
	$(MAKE) -C doc clean
	rm -f include/eperf/*.mod
	rm -f Makefile

.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all lib examples clean

