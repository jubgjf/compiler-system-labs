%{
    void token_info(char* token);
%}

digit                               [0-9]
oct                                 [0-7]
hex                                 [0-9a-fA-F]
letter_                             [_a-zA-Z]
frac                                (\.{digit}+)?
exp                                 ([Ee][-+]?{digit}+)?

%x                                  BLOCKCOMMENT

%%

0|[1-9]{digit}*                     { token_info("INT10");    }
0{oct}+                             { token_info("INT8");     }
0x{hex}+                            { token_info("INT16");    }
{digit}+{frac}{exp}                 { token_info("FLOAT");    }
struct|return|if|else|while         { token_info(yytext);     }
int|float                           { token_info("TYPE");     }
{letter_}({letter_}|{digit})*       { token_info("ID");       }
"//".*                              {                         }
"/*"                                { BEGIN(BLOCKCOMMENT);    }
<BLOCKCOMMENT>"*/"                  { BEGIN(INITIAL);         }
<BLOCKCOMMENT>\n                    { yylineno += 1;          }
<BLOCKCOMMENT>.                     {                         }
";"                                 { token_info("SEMI");     }
","                                 { token_info("COMMA");    }
"="                                 { token_info("ASSIGNOP"); }
">"                                 { token_info("RELOP");    }
"<"                                 { token_info("RELOP");    }
">="                                { token_info("RELOP");    }
"<="                                { token_info("RELOP");    }
"=="                                { token_info("RELOP");    }
"!="                                { token_info("RELOP");    }
"+"                                 { token_info("PLUS");     }
"-"                                 { token_info("MINUS");    }
"*"                                 { token_info("STAR");     }
"/"                                 { token_info("DIV");      }
"&&"                                { token_info("AND");      }
"||"                                { token_info("OR");       }
"."                                 { token_info("DOT");      }
"!"                                 { token_info("NOT");      }
"("                                 { token_info("LP");       }
")"                                 { token_info("RP");       }
"["                                 { token_info("LB");       }
"]"                                 { token_info("RB");       }
"{"                                 { token_info("LC");       }
"}"                                 { token_info("RC");       }
"\n"                                { yylineno += 1;          }
[ \t]+                              {                         }
.                                   { token_info("UNKNOWN");  }

%%
