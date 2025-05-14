%{
#include <stdio.h>
#include <string.h>
%}

%option noyywrap

%%
"the"|"a"|"an"                                 { printf("Article: %s\n", yytext); }
"i"|"you"|"he"|"she"|"it"|"we"|"they"|"me"|"him"|"her"|"us"|"them"   { printf("Pronoun: %s\n", yytext); }
"am"|"is"|"are"|"was"|"were"|"be"|"been"|"being"|"have"|"has"|"had"|"do"|"does"|"did"|"will"|"shall"|"would"|"should"|"can"|"could"|"may"|"might"|"must"|"ought" { printf("Verb (Auxiliary): %s\n", yytext); }
"run"|"arrives"|"imagine"|"kill"|"give"|"light"|"stop"|"questioning"|"exist"|"divide"|"need"|"go"|"come" { printf("Verb: %s\n", yytext); }
"important"|"own"|"more"|"intelligent"|"wiser"|"dramatic"|"binary"|"basic"|"strong" { printf("Adjective: %s\n", yytext); }
"from"|"to"|"for"|"in"|"on"|"at"|"by"|"with"|"of"|"about"|"between"|"into"|"through"|"before"|"after" { printf("Preposition: %s\n", yytext); }
"and"|"but"|"or"|"yet"|"so"|"if" { printf("Conjunction: %s\n", yytext); }
"destiny"|"thing"|"curiosity"|"reason"|"generation"|"cigarette"|"gap"|"instinct"|"urge"|"group"|"power"|"world"|"sentence"|"people"|"man"|"beings"|"mind" { printf("Noun: %s\n", yytext); }
"hello"|"thank"|"fine"|"never"|"always"|"still"|"same"|"before"|"after"|"now"|"how"|"why"|"what"|"when"|"where" { printf("Adverb or Interjection: %s\n", yytext); }
[0-9]+                      { printf("Number: %s\n", yytext); }
[.,!?;:\"\']                { printf("Punctuation: %s\n", yytext); }
[ \t\n]+                   { /* skip whitespace */ }
[A-Za-z]+                  { printf("Unknown word: %s\n", yytext); }
.                          { printf("Unknown character: %s\n", yytext); }
%%

int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        perror("input.txt");
        return 1;
    }
    yyin = fp;
    yylex();
    fclose(fp);
    return 0;
}
/* code for get input manaly 
int main() {
    printf("Enter your input (Ctrl+D to end input):\n");
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