%{

# include "assembler.tab.h" // the output of bison

%}

%option noyywrap

%%

";" return SEMICOLON;
":" return COLON;
"," return COMMA;
"[" return O_BRACKET;
"]" return C_BRACKET;

\/\/[[:blank:][:alnum:]]+\n ; // Comment: do nothing

"MOV" return MOV;
"ADD" return ADD;
"JMP" return JMP;
"END" return END;

[0-9A-Fa-f]+ { yylval.int_token = strtol(yytext, NULL, 16); return INT;}
[A-Za-z]+ {
    yylval.str_token = strdup(yytext); // Replace the function at some point with something portable
    return STRING;
}

[\t\n]+ ; // do nothing on whitespace
