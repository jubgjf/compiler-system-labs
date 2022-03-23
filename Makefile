TOPTARGETS := all clean

SUBDIRS := lab1

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

dir:
	mkdir -p build

run1: dir
	$(MAKE) -C lab1 run

.PHONY: $(TOPTARGETS) $(SUBDIRS)
