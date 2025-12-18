
%{
	#include <string.h>
	#include <stdlib.h>

%}

 // Definições de padrões (macros)
 // NUM   : números (inteiros ou ponto flutuante)
 // VARS  : identificador de variável (letras seguidas de dígitos opcionais)
 // TEXTO : texto (token para strings)
 
NUM [0-9]+\.?[0-9]*
VARS [a-z]+[0-9]*
FIM "FIM"
IF "IF"
ELSE "ELSE"
WHILE "WHILE"
PRINT "PRINT"
PRINTT "PRINTT"
SCAN "LER"
SCANS "LERS"
DECL "var"
TEXTO [a-zA-Z]+   /* uma ou mais letras (texto simples) */

%%
 /* Regras de associação entre texto reconhecido e tokens retornados ao parser */
{FIM}     { return FIM; }
{IF}      { return IF; }
{ELSE}    { return ELSE; }
{WHILE}   { return WHILE; }
{PRINTT}  { return PRINTT; }
{PRINT}   { return PRINT; }
{SCAN}    { return SCAN; }
{SCANS}   { return SCANS; }
{DECL}    { return DECL; }

{VARS} {
	strcpy(yylval.str, yytext);
	return VARS;
}


{NUM} {
	yylval.flo = atof(yytext);
	return NUM;
}

[-+()=\/*^{};\[\]] { return *yytext; }

">"  { yylval.fn = 1; return CMP; }
"<"  { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"==" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }

[ \n\t]*    { /* ignorar espaços em branco */ }
.          { printf("(%s) Ignorado\n", yytext); }
%%
