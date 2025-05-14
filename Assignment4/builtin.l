%{
#include "y.tab.h"
#include <string.h>
%}

%%
[ \t\n]+               ;       // skip whitespace
[0-9]+\.[0-9]+         { yylval.f = atof(yytext); return NUMBER; }
[0-9]+                 { yylval.f = atof(yytext); return NUMBER; }
\"[^\"]+\"             { yylval.s = strdup(yytext); return STRING; }
[a-zA-Z_][a-zA-Z_0-9]* {
                          if (strcmp(yytext, "sqrt") == 0) return SQRT;
                          if (strcmp(yytext, "strlen") == 0) return STRLEN;
                          if (strcmp(yytext, "sin") == 0) return SIN;
                          if (strcmp(yytext, "cos") == 0) return COS;
                          if (strcmp(yytext, "pow") == 0) return POW;
                          if (strcmp(yytext, "log") == 0) return LOG;
                          return ID;
                        }
"="                    return '=';
","                    return ',';
"("                    return '(';
")"                    return ')';
"+"                    return '+';
"-"                    return '-';
"*"                    return '*';
"/"                    return '/';
.                      return yytext[0];

%%
int yywrap() { return 1; }
