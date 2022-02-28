#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);

extern FILE* yyin;
extern char* yytext;
extern int   yylineno;

int main(int argc, char** argv) {
    if (argc == 2 && !(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }

    // 以 stdin 或 yyin 文件作为词法分析器的输入
    while (yylex() != 0)
        ;

    return 0;
}

/// 打印 \p token 信息
void token_info(char* token) {
    if (!strcmp(token, "INT10")) {
        printf("text: %-10stoken: %-10svalue: %-10ldlineno: %d\n", yytext,
               "INT", strtol(yytext, NULL, 10), yylineno);
    } else if (!strcmp(token, "INT8")) {
        printf("text: %-10stoken: %-10svalue: %-10ldlineno: %d\n", yytext,
               "INT", strtol(yytext, NULL, 8), yylineno);
    } else if (!strcmp(token, "INT16")) {
        printf("text: %-10stoken: %-10svalue: %-10ldlineno: %d\n", yytext,
               "INT", strtol(yytext, NULL, 16), yylineno);
    } else if (!strcmp(token, "FLOAT")) {
        printf("text: %-10stoken: %-10svalue: %-10flineno: %d\n", yytext, token,
               atof(yytext), yylineno);
    } else {
        printf("text: %-10stoken: %-10svalue: %-10slineno: %d\n", yytext, token,
               "_", yylineno);
    }

    if (!strcmp(token, "UNKNOWN")) {
        printf("Error type A at line %d: Mysterious character \"%s\".\n",
               yylineno, yytext);
    }
}
