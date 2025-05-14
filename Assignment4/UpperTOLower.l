%{
#include "y.tab.h"
%}

%%
.   { yylval.c = yytext[0]; return CHAR; }
\n  { return END; }
%%

int yywrap() {
    return 1;
}
