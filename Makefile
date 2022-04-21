TOPTARGETS := all clean

SUBDIRS := lab1 lab2 lab3

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

dir:
	mkdir -p build

run1: dir
	$(MAKE) -C lab1 run

run2: dir
	$(MAKE) -C lab2 run

run3: dir
	$(MAKE) -C lab3 run

.PHONY: $(TOPTARGETS) $(SUBDIRS)
