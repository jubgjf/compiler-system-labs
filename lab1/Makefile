BUILDDIR = ../build
TESTDIR = ../test

lex.yy.c: lexical.l
	flex $^

syntax.tab.h syntax.tab.c: syntax.y
	bison -d $^

parser: main.c lex.yy.c syntax.tab.h syntax.tab.c tree.h tree.c
	$(CC) $^ -lfl -ly -g -o $(BUILDDIR)/$@

run: parser
	@echo "===== 指导书样例 1.1 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab1-1-1.txt
	@echo ""
	@echo "===== 指导书样例 1.2 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab1-1-2.txt
	@echo ""
	@echo "===== 指导书样例 1.3 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab1-1-3.txt
	@echo ""
	@echo "===== 指导书样例 1.4 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab1-1-4.txt
	@echo ""
	@echo "===== 指导书样例 1.7 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab1-1-7.txt
	@echo ""

clean:
	$(RM) $(BUILDDIR)/parser lex.yy.c syntax.tab.h syntax.tab.c
