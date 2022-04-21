BUILDDIR = ../build
TESTDIR = ../test

lex.yy.c: lexical.l
	flex $^

syntax.tab.h syntax.tab.c: syntax.y
	bison -d $^

parser: main.c lex.yy.c syntax.tab.h syntax.tab.c tree.h tree.c semantic.h semantic.c inter.h inter.c
	$(CC) $^ -lfl -ly -g -o $(BUILDDIR)/$@

run: parser
	@echo "===== 指导书样例 3.1 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab3-3-1.txt
	@echo ""
	@echo "===== 指导书样例 3.2 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab3-3-2.txt
	@echo ""

clean:
	$(RM) $(BUILDDIR)/parser lex.yy.c syntax.tab.h syntax.tab.c
