#include "inter.h"
#include "semantic.h"
#include <stdio.h>

extern void yyrestart(FILE* input_file);
extern int  yyparse();

extern unsigned error_lexical;
extern unsigned error_syntax;
extern unsigned error_iter;

extern struct node*      root;  // 语法分析：语法分析树
extern struct sym_table* table; // 语义分析：符号表
extern struct inter_code_list* inter_code_list; // 中间代码生成：中间代码链表

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

    // 中间代码默认输出到 stdout，除非指定了一个输出文件
    FILE* f_result = stdout;
    if (argc >= 3) {
        f_result = fopen(argv[2], "wt+");
        if (!f_result) {
            perror(argv[2]);
            return 1;
        }
    }

    // 词法分析
    yyrestart(f);
    yyparse();

    // 无错误则进行语义分析
    if (!error_lexical && !error_syntax) {
        // 语义分析
        table = init_table();
        semantic_analysis(root);

        // 中间代码生成
        inter_code_list = new_inter_code_list();
        gen_inter_codes(root);

        // 无错误则打印中间代码
        if (!error_iter) {
            print_iter_code(f_result, inter_code_list);
        }
    }

    return 0;
}
