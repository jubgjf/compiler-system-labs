BUILDDIR = ../build
TESTDIR = ../test
TESTFILE = $(TESTDIR)/test-syntax.txt

lex.yy.c: lexical.l
	flex $^

syntax.tab.h syntax.tab.c: syntax.y
	bison -d $^

scanner: main.c lex.yy.c syntax.tab.h syntax.tab.c tree.h tree.c
	$(CC) $^ -lfl -ly -g -o $(BUILDDIR)/$@

run: scanner
	$(BUILDDIR)/$^ $(TESTFILE)

clean:
	$(RM) $(BUILDDIR)/scanner lex.yy.c syntax.tab.h syntax.tab.c
