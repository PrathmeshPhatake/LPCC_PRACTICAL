%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void yyerror(const char *s);
int yylex();

%}

%union {
    float f;
    char *s;
}

%token <f> NUMBER
%token <s> STRING
%token ID
%token SQRT STRLEN SIN COS POW LOG

%type <f> expr

%%

stmt:
    ID '=' expr { printf("%s = %.2f\n", yytext, $3); }
  ;

expr:
    expr '+' expr   { $$ = $1 + $3; }
  | expr '-' expr   { $$ = $1 - $3; }
  | expr '*' expr   { $$ = $1 * $3; }
  | expr '/' expr   { $$ = $1 / $3; }
  | SQRT '(' expr ')'  { $$ = sqrt($3); }
  | SIN '(' expr ')'   { $$ = sin($3); }
  | COS '(' expr ')'   { $$ = cos($3); }
  | LOG '(' expr ')'   { $$ = log($3); }
  | POW '(' expr ',' expr ')' { $$ = pow($3, $5); }
  | STRLEN '(' STRING ')' {
        $$ = strlen($3) - 2; // subtract quotes
        free($3);
    }
  | NUMBER          { $$ = $1; }
  ;

%%

int main() {
    printf("Enter an expression:\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
/*
yacc -d builtin.y
flex builtin.l
gcc y.tab.c lex.yy.c -lm -o builtin
./builtin
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