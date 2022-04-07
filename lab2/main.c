#include "semantic.h"
#include <stdio.h>

extern void yyrestart(FILE* input_file);
extern int  yyparse();

extern unsigned     error_lexical;
extern unsigned     error_syntax;
extern struct node* root;

extern struct sym_table* table;

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

    // 无错误则进行语义分析
    if (!error_lexical && !error_syntax) {
        table = init_table();
        semantic_analysis(root);
    }

    return 0;
}
