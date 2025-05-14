%{
#include "y.tab.h"
%}

%%
[a-zA-Z_][a-zA-Z0-9_]*   { return IDENTIFIER; }
.                        { return INVALID; }
\n                       { return 0; }
%%

int yywrap() {
    return 1;
}
