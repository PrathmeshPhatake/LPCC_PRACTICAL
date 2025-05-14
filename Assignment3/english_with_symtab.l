%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 1000

// Symbol Table Structure
struct symbol {
    char word[100];
    char type[100];
};

struct symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

// Function to insert into symbol table
void insert_symbol(const char *word, const char *type) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].word, word) == 0)
            return; // Already in table
    }
    strcpy(symtab[symbol_count].word, word);
    strcpy(symtab[symbol_count].type, type);
    symbol_count++;
}

// Function to display symbol table
void display_symbol_table() {
    printf("\n--- Symbol Table ---\n");
    printf("%-20s %-20s\n", "Word", "Type");
    for (int i = 0; i < symbol_count; i++) {
        printf("%-20s %-20s\n", symtab[i].word, symtab[i].type);
    }
}
%}

%option noyywrap

%%
"the"|"a"|"an"                             { printf("Article: %s\n", yytext); insert_symbol(yytext, "Article"); }
"i"|"you"|"he"|"she"|"it"|"we"|"they"|"me"|"him"|"her"|"us"|"them" { printf("Pronoun: %s\n", yytext); insert_symbol(yytext, "Pronoun"); }
"am"|"is"|"are"|"was"|"were"|"be"|"been"|"being"|"have"|"has"|"had"|"do"|"does"|"did"|"will"|"shall"|"would"|"should"|"can"|"could"|"may"|"might"|"must"|"ought" { printf("Auxiliary Verb: %s\n", yytext); insert_symbol(yytext, "Auxiliary Verb"); }
"run"|"arrives"|"imagine"|"kill"|"give"|"light"|"stop"|"questioning"|"exist"|"divide"|"need"|"go"|"come" { printf("Verb: %s\n", yytext); insert_symbol(yytext, "Verb"); }
"important"|"own"|"more"|"intelligent"|"wiser"|"dramatic"|"binary"|"basic"|"strong" { printf("Adjective: %s\n", yytext); insert_symbol(yytext, "Adjective"); }
"from"|"to"|"for"|"in"|"on"|"at"|"by"|"with"|"of"|"about"|"between"|"into"|"through"|"before"|"after" { printf("Preposition: %s\n", yytext); insert_symbol(yytext, "Preposition"); }
"and"|"but"|"or"|"yet"|"so"|"if" { printf("Conjunction: %s\n", yytext); insert_symbol(yytext, "Conjunction"); }
"destiny"|"thing"|"curiosity"|"reason"|"generation"|"cigarette"|"gap"|"instinct"|"urge"|"group"|"power"|"world"|"sentence"|"people"|"man"|"beings"|"mind" { printf("Noun: %s\n", yytext); insert_symbol(yytext, "Noun"); }
"hello"|"thank"|"fine"|"never"|"always"|"still"|"same"|"before"|"after"|"now"|"how"|"why"|"what"|"when"|"where" { printf("Adverb/Interjection: %s\n", yytext); insert_symbol(yytext, "Adverb/Interjection"); }
[0-9]+                                    { printf("Number: %s\n", yytext); insert_symbol(yytext, "Number"); }
[.,!?;:\"\'()\-]                           { printf("Punctuation: %s\n", yytext); insert_symbol(yytext, "Punctuation"); }
[ \t\n]+                                  { /* Skip whitespace */ }
[A-Za-z]+                                 { printf("Unknown Word: %s\n", yytext); insert_symbol(yytext, "Unknown"); }
.                                         { printf("Unknown Character: %s\n", yytext); insert_symbol(yytext, "Unknown Character"); }
%%

int main() {
    printf("Enter input (Press Ctrl+D to end input):\n");
    yylex();
    display_symbol_table();
    return 0;
}


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