#include "tree.h"
#include <stdio.h>

extern void yyrestart(FILE* input_file);
extern int  yyparse();

extern unsigned     error_lexical;
extern unsigned     error_syntax;
extern struct node* root;

int main(int argc, char** argv) {
    if (argc <= 1) {
        return 1;
    }

    // 待词法分析的文件
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }

    // 词法分析
    yyrestart(f);
    yyparse();

    // 无错误则打印语法树
    if (!error_lexical && !error_syntax) {
        pre_order_traversal(root, 0);
    }

    return 0;
}
