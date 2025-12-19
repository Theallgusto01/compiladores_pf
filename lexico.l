
%{
	#include <string.h>
	#include <stdlib.h>

%}

%x COMMENT


 
NUM [0-9]+\.?[0-9]*
VARS [a-z]+[0-9]*
IF "if"
ELSE "else"
WHILE "while"
PRINT "print"
PRINTT "printt"
SCAN "scan"
SCANS "scans"
DECL "var"



%%
{IF}      { return IF; }
{ELSE}    { return ELSE; }
{WHILE}   { return WHILE; }
{PRINTT}  { return PRINTT; }
{PRINT}   { return PRINT; }
{SCAN}    { return SCAN; }
{SCANS}   { return SCANS; }
{DECL}    { return DECL; }
"//".*          { /* ignora comentário de linha */ }
"/*"            { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>\n     { /* mantém contagem de linhas */ }
<COMMENT>.       { /* ignora conteúdo do bloco */ }
<COMMENT><<EOF>> { fprintf(stderr, "Unterminated block comment\n"); exit(1); }

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
