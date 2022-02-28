BUILDDIR = ../build
TESTDIR = ../test
TESTFILE = $(TESTDIR)/test-lexical.txt

lex.yy.c: lexical.l
	flex $^

scanner: main.c lex.yy.c
	gcc $^ -lfl -o $(BUILDDIR)/$@

run: scanner
	$(BUILDDIR)/$^ $(TESTFILE)

clean:
	rm $(BUILDDIR)/scanner lex.yy.c
