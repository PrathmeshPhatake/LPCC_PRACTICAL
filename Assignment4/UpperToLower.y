%{
#include <stdio.h>
#include <ctype.h>

%}

%union {
    char c;
}

%token <c> CHAR
%token END
%type <c> input

%%
start: input END { printf("\n"); }
     ;

input:
    input CHAR {
        if (islower($2))
            printf("%c", toupper($2));
        else if (isupper($2))
            printf("%c", tolower($2));
        else
            printf("%c", $2);
    }
  | /* empty */
  ;
%%

int main() {
    printf("Enter a string: ");
    yyparse();
    return 0;
}

int yyerror(const char *s) {
    return 0;
}


/*
yacc -d caseconv.y
flex caseconv.l
gcc y.tab.c lex.yy.c -o caseconv -lfl
./caseconv
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