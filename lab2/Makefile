BUILDDIR = ../build
TESTDIR = ../test

lex.yy.c: lexical.l
	flex $^

syntax.tab.h syntax.tab.c: syntax.y
	bison -d $^

parser: main.c lex.yy.c syntax.tab.h syntax.tab.c tree.h tree.c semantic.h semantic.c
	$(CC) $^ -lfl -ly -g -o $(BUILDDIR)/$@

run: parser
	@echo "===== 指导书样例 2.1 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-1.txt
	@echo "Error type 1 at Line 4: Undefined variable \"j\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.2 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-2.txt
	@echo "Error type 2 at Line 4: Undefined function \"inc\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.3 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-3.txt
	@echo "Error type 3 at Line 4: Redefined variable \"i\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.4 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-4.txt
	@echo "Error type 4 at Line 6: Redefined function \"func\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.5 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-5.txt
	@echo "Error type 5 at Line 4: Type mismatched for assignment. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.6 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-6.txt
	@echo "Error type 6 at Line 4: The left-hand side of an assignment must be a variable. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.7 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-7.txt
	@echo "Error type 7 at Line 4: Type mismatched for operands. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.8 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-8.txt
	@echo "Error type 8 at Line 4: Type mismatched for return. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.9 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-9.txt
	@echo "Error type 9 at Line 8: Function \"func(int)\" is not applicable for arguments \"(int, int)\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.10 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-10.txt
	@echo "Error type 10 at Line 4: \"i\" is not an array. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.11 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-11.txt
	@echo "Error type 11 at Line 4: \"i\" is not a function. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.12 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-12.txt
	@echo "Error type 12 at Line 4: \"1.5\" is not an integer. [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.13 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-13.txt
	@echo "Error type 13 at Line 9: Illegal use of \".\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.14 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-14.txt
	@echo "Error type 14 at Line 9: Non-existent field \"n\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.15 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-15.txt
	@echo "Error type 15 at Line 4: Redefined field \"x\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.16 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-16.txt
	@echo "Error type 16 at Line 6: Duplicated name \"Position\". [Expected]"
	@echo ""
	@echo "===== 指导书样例 2.17 ====="
	@$(BUILDDIR)/$^ $(TESTDIR)/test-lab2-2-17.txt
	@echo "Error type 17 at Line 3: Undefined structure \"Position\". [Expected]"
	@echo ""

clean:
	$(RM) $(BUILDDIR)/parser lex.yy.c syntax.tab.h syntax.tab.c
