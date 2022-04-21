%{
    #include "syntax.tab.h"
    #include "tree.h"

    /// 词法分析是否出现错误
    unsigned error_lexical = 0;

    /// 列号
    int yycolumn = 1;

    /// 列号自动更新
    #define YY_USER_ACTION                                                     \
        yylloc.first_line = yylloc.last_line = yylineno;                       \
        yylloc.first_column                  = yycolumn;                       \
        yylloc.last_column                   = yycolumn + yyleng - 1;          \
        yycolumn += yyleng;
%}

digit                               [0-9]
letter_                             [_a-zA-Z]
frac                                (\.{digit}+)?
exp                                 ([Ee][-+]?{digit}+)?

%%

0|[1-9]{digit}*                     { yylval.node = new_node(yylineno, TOK_INT,      yytext,     0); return INT;        }
{digit}+{frac}{exp}                 { yylval.node = new_node(yylineno, TOK_FLOAT,    yytext,     0); return FLOAT;      }
struct                              { yylval.node = new_node(yylineno, TOK_STRUCT,   "STRUCT",   0); return STRUCT;     }
return                              { yylval.node = new_node(yylineno, TOK_RETURN,   "RETURN",   0); return RETURN;     }
if                                  { yylval.node = new_node(yylineno, TOK_IF,       "IF",       0); return IF;         }
else                                { yylval.node = new_node(yylineno, TOK_ELSE,     "ELSE",     0); return ELSE;       }
while                               { yylval.node = new_node(yylineno, TOK_WHILE,    "WHILE",    0); return WHILE;      }
int|float                           { yylval.node = new_node(yylineno, TOK_TYPE,     yytext,     0); return TYPE;       }
{letter_}({letter_}|{digit})*       { yylval.node = new_node(yylineno, TOK_ID,       yytext,     0); return ID;         }
"//".*                              {                                                                                   }
";"                                 { yylval.node = new_node(yylineno, TOK_SEMI,     "SEMI",     0); return SEMI;       }
","                                 { yylval.node = new_node(yylineno, TOK_COMMA,    "COMMA",    0); return COMMA;      }
"="                                 { yylval.node = new_node(yylineno, TOK_ASSIGNOP, "ASSIGNOP", 0); return ASSIGNOP;   }
">"                                 { yylval.node = new_node(yylineno, TOK_RELOP,    ">",        0); return RELOP;      }
"<"                                 { yylval.node = new_node(yylineno, TOK_RELOP,    "<",        0); return RELOP;      }
">="                                { yylval.node = new_node(yylineno, TOK_RELOP,    ">=",       0); return RELOP;      }
"<="                                { yylval.node = new_node(yylineno, TOK_RELOP,    "<=",       0); return RELOP;      }
"=="                                { yylval.node = new_node(yylineno, TOK_RELOP,    "==",       0); return RELOP;      }
"!="                                { yylval.node = new_node(yylineno, TOK_RELOP,    "!=",       0); return RELOP;      }
"+"                                 { yylval.node = new_node(yylineno, TOK_PLUS,     "PLUS",     0); return PLUS;       }
"-"                                 { yylval.node = new_node(yylineno, TOK_MINUS,    "MINUS",    0); return MINUS;      }
"*"                                 { yylval.node = new_node(yylineno, TOK_STAR,     "STAR",     0); return STAR;       }
"/"                                 { yylval.node = new_node(yylineno, TOK_DIV,      "DIV",      0); return DIV;        }
"&&"                                { yylval.node = new_node(yylineno, TOK_AND,      "AND",      0); return AND;        }
"||"                                { yylval.node = new_node(yylineno, TOK_OR,       "OR",       0); return OR;         }
"."                                 { yylval.node = new_node(yylineno, TOK_DOT,      "DOT",      0); return DOT;        }
"!"                                 { yylval.node = new_node(yylineno, TOK_NOT,      "NOT",      0); return NOT;        }
"("                                 { yylval.node = new_node(yylineno, TOK_LP,       "LP",       0); return LP;         }
")"                                 { yylval.node = new_node(yylineno, TOK_RP,       "RP",       0); return RP;         }
"["                                 { yylval.node = new_node(yylineno, TOK_LB,       "LB",       0); return LB;         }
"]"                                 { yylval.node = new_node(yylineno, TOK_RB,       "RB",       0); return RB;         }
"{"                                 { yylval.node = new_node(yylineno, TOK_LC,       "LC",       0); return LC;         }
"}"                                 { yylval.node = new_node(yylineno, TOK_RC,       "RC",       0); return RC;         }
"\n"                                { yylineno += 1;                                                                    }
[ \t]+                              {                                                                                   }
.                                   { error_lexical = 1; printf("Error type A at Line %d.\n", yylineno);                }

%%
