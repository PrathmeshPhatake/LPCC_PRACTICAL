%{
#include <stdio.h>
#include <stdlib.h>
%}

%token IDENTIFIER INVALID

%%
input:
    IDENTIFIER   { printf("Valid variable name.\n"); }
  | INVALID      { printf("Invalid variable name.\n"); }
  ;
%%

int main() {
    printf("Enter a variable name: ");
    yyparse();
    return 0;
}

int yyerror(const char *s) {
    return 0;
}
/*
yacc -d varname.y
flex varname.l
gcc y.tab.c lex.yy.c -o varname_eval -lfl
./varname_eval

*/

/*
Install tools (only once):

sudo apt update
sudo apt install flex bison build-essential
If you're using a distro that uses dnf (like Fedora), replace apt with dnf.

Compile and build:

bison -d expr.y
flex expr.l
gcc y.tab.c lex.yy.c -lfl -o expr_evaluator


Run the evaluator:
./expr_evaluator
Then input: 0.33*12-4-4+(3*2) and press Enter.
*/