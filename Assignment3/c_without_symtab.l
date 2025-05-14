%{
#include <stdio.h>
%}

%%

"int"|"char"|"float"|"printf"               { printf("KEYWORD\t\t=> %s\n", yytext); }
[0-9]+(\.[0-9]+)?                           { printf("NUMBER\t\t=> %s\n", yytext); }
\"([^\"\\]|\\.)*\"                          { printf("STRING\t\t=> %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*                      { printf("IDENTIFIER\t=> %s\n", yytext); }
"="                                         { printf("ASSIGNMENT\t=> =\n"); }
"=="|"!="|"<"|">"|"&&"|"||"|"<="|">="       { printf("COMPARISON OP\t=> %s\n", yytext); }
"("                                         { printf("LEFT PAREN\t=> (\n"); }
")"                                         { printf("RIGHT PAREN\t=> )\n"); }
"{"                                         { printf("LEFT BRACE\t=> {\n"); }
"}"                                         { printf("RIGHT BRACE\t=> }\n"); }
";"                                         { printf("SEMICOLON\t=> ;\n"); }
","                                         { printf("COMMA\t\t=> ,\n"); }
"*"|"/"|"+"|"-"                             { printf("OPERATOR\t=> %s\n", yytext); }
[ \t\n\r]+                                  ; // Ignore whitespace
.                                           { printf("UNKNOWN\t\t=> %s\n", yytext); }

%%

int yywrap() {
    return 1;
}

int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        printf("Cannot open input.txt\n");
        return 1;
    }
    yyin = fp;
    yylex();
    fclose(fp);
    return 0;
}
/*
int main() {
   printf("Enter C code below (press Ctrl+D when done):\n");
   yylex();
   return 0;
}
*/

/*
Save the file 
 command :nano symbol_table.l


Install Flex and GCC (if not already installed):
sudo apt update
sudo apt install flex gcc -y

Compile the Lex file:

flex symbol_table.l
gcc lex.yy.c -lfl -o symbol_table

Run the program:

./symbol_table
*/