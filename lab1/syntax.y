%{
    #include "tree.h"
    #include <stdio.h>

    extern int yylex();
    extern int yylineno;

    /// 语法分析是否出现错误
    int error_syntax = 0;

    /// 语法树
    struct node* root = NULL;

    /// 列号
    int yycolumn = 1;

    /// 列号自动更新
    #define YY_USER_ACTION                                                     \
        yylloc.first_line = yylloc.last_line = yylineno;                       \
        yylloc.first_column                  = yycolumn;                       \
        yylloc.last_column                   = yycolumn + yyleng - 1;          \
        yycolumn += yyleng;
%}

/// yylval 类型
%union { struct node* node; }

%token <node> INT
%token <node> FLOAT
%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> TYPE
%token <node> ID
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> PLUS
%token <node> MINUS
%token <node> STAR
%token <node> DIV
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> NOT
%token <node> LP
%token <node> RP
%token <node> LB
%token <node> RB
%token <node> LC
%token <node> RC

%type <node> Program
%type <node> ExtDefList
%type <node> ExtDef
%type <node> ExtDecList
%type <node> Specifier
%type <node> StructSpecifier
%type <node> OptTag
%type <node> Tag
%type <node> VarDec
%type <node> FunDec
%type <node> VarList
%type <node> ParamDec
%type <node> CompSt
%type <node> StmtList
%type <node> Stmt
%type <node> DefList
%type <node> Def
%type <node> Dec
%type <node> DecList
%type <node> Exp
%type <node> Args

%%

Program: ExtDefList                 { root = new_node(@$.first_line, NOT_TOKEN, "Program",       1, $1                        ); }
    ;
ExtDefList: /* empty */             { $$ = NULL;                                                                                 }
    | ExtDef ExtDefList             { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDefList",      2, $1, $2                    ); }
    ;
ExtDef: Specifier SEMI              { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDef",          2, $1, $2                    ); }
    | Specifier ExtDecList SEMI     { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDef",          3, $1, $2, $3                ); }
    | Specifier FunDec CompSt       { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDef",          3, $1, $2, $3                ); }
    | error SEMI                    { error_syntax = 1;                                                                          }
    ;
ExtDecList: VarDec                  { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDecList",      1, $1                        ); }
    | VarDec COMMA ExtDecList       { $$ = new_node(@$.first_line, NOT_TOKEN, "ExtDecList",      3, $1, $2, $3                ); }
    ;

Specifier: TYPE                     { $$ = new_node(@$.first_line, NOT_TOKEN, "Specifier",       1, $1                        ); }
    | StructSpecifier               { $$ = new_node(@$.first_line, NOT_TOKEN, "Specifier",       1, $1                        ); }
    ;
StructSpecifier: STRUCT Tag         { $$ = new_node(@$.first_line, NOT_TOKEN, "StructSpecifier", 2, $1, $2                    ); }
    | STRUCT OptTag LC DefList RC   { $$ = new_node(@$.first_line, NOT_TOKEN, "StructSpecifier", 5, $1, $2, $3, $4, $5        ); }
    ;
OptTag: /* empty */                 { $$ = NULL;                                                                                 }
    | ID                            { $$ = new_node(@$.first_line, NOT_TOKEN, "OptTag",          1, $1                        ); }
    ;
Tag: ID                             { $$ = new_node(@$.first_line, NOT_TOKEN, "Tag",             1, $1                        ); }
    ;

VarDec: ID                          { $$ = new_node(@$.first_line, NOT_TOKEN, "VarDec",          1, $1                        ); }
    | VarDec LB INT RB              { $$ = new_node(@$.first_line, NOT_TOKEN, "VarDec",          4, $1, $2, $3, $4            ); }
    | error RB                      { error_syntax = 1;                                                                          }
    ;
FunDec: ID LP VarList RP            { $$ = new_node(@$.first_line, NOT_TOKEN, "FunDec",          4, $1, $2, $3, $4            ); }
    | ID LP RP                      { $$ = new_node(@$.first_line, NOT_TOKEN, "FunDec",          3, $1, $2, $3                ); }
    | error RP                      { error_syntax = 1;                                                                          }
    ;
VarList: ParamDec COMMA VarList     { $$ = new_node(@$.first_line, NOT_TOKEN, "VarList",         3, $1, $2, $3                ); }
    | ParamDec                      { $$ = new_node(@$.first_line, NOT_TOKEN, "VarList",         1, $1                        ); }
    ;
ParamDec: Specifier VarDec          { $$ = new_node(@$.first_line, NOT_TOKEN, "ParamDec",        2, $1, $2                    ); }
    ;

CompSt: LC DefList StmtList RC      { $$ = new_node(@$.first_line, NOT_TOKEN, "CompSt",          4, $1, $2, $3, $4            ); }
    | error RC                      { error_syntax = 1;                                                                          }
    ;
StmtList: /* empty */               { $$ = NULL;                                                                                 }
    | Stmt StmtList                 { $$ = new_node(@$.first_line, NOT_TOKEN, "StmtList",        2, $1, $2                    ); }
    ;
Stmt: Exp SEMI                      { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            2, $1, $2                    ); }
    | CompSt                        { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            1, $1                        ); }
    | RETURN Exp SEMI               { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            3, $1, $2, $3                ); }
    | IF LP Exp RP Stmt             { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            5, $1, $2, $3, $4, $5        ); }
    | IF LP Exp RP Stmt ELSE Stmt   { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt          { $$ = new_node(@$.first_line, NOT_TOKEN, "Stmt",            5, $1, $2, $3, $4, $5        ); }
    | error SEMI                    { error_syntax = 1;                                                                          }
    ;

DefList: /* empty */                { $$ = NULL;                                                                                 }
    | Def DefList                   { $$ = new_node(@$.first_line, NOT_TOKEN, "DefList",         2, $1, $2                    ); }
    ;
Def: Specifier DecList SEMI         { $$ = new_node(@$.first_line, NOT_TOKEN, "Def",             3, $1, $2, $3                ); }
    ;
DecList: Dec                        { $$ = new_node(@$.first_line, NOT_TOKEN, "DecList",         1, $1                        ); }
    | Dec COMMA DecList             { $$ = new_node(@$.first_line, NOT_TOKEN, "DecList",         3, $1, $2, $3                ); }
    ;
Dec: VarDec                         { $$ = new_node(@$.first_line, NOT_TOKEN, "Dec",             1, $1                        ); }
    | VarDec ASSIGNOP Exp           { $$ = new_node(@$.first_line, NOT_TOKEN, "Dec",             3, $1, $2, $3                ); }
    ;

Exp: Exp ASSIGNOP Exp               { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp AND Exp                   { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp OR Exp                    { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp RELOP Exp                 { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp PLUS Exp                  { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp MINUS Exp                 { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp STAR Exp                  { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp DIV Exp                   { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | LP Exp RP                     { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | MINUS Exp                     { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             2, $1, $2                    ); }
    | NOT Exp                       { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             2, $1, $2                    ); }
    | ID LP Args RP                 { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             4, $1, $2, $3, $4            ); }
    | ID LP RP                      { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | Exp LB Exp RB                 { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             4, $1, $2, $3, $4            ); }
    | Exp DOT ID                    { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             3, $1, $2, $3                ); }
    | ID                            { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             1, $1                        ); }
    | INT                           { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             1, $1                        ); }
    | FLOAT                         { $$ = new_node(@$.first_line, NOT_TOKEN, "Exp",             1, $1                        ); }
    ;
Args: Exp COMMA Args                { $$ = new_node(@$.first_line, NOT_TOKEN, "Args",            3, $1, $2, $3                ); }
    | Exp                           { $$ = new_node(@$.first_line, NOT_TOKEN, "Args",            1, $1                        ); }
    ;

%%

void yyerror(char* msg) {
    fprintf(stderr, "Error type B at line %d.\n", yylineno);
}
