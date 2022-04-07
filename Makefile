TOPTARGETS := all clean

SUBDIRS := lab1 lab2

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

dir:
	mkdir -p build

run1: dir
	$(MAKE) -C lab1 run

run2: dir
	$(MAKE) -C lab2 run

.PHONY: $(TOPTARGETS) $(SUBDIRS)
