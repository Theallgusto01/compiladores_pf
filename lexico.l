
%{
	#include <string.h>
	#include <stdlib.h>

%}

%x COMMENT


 
NUM [0-9]+\.?[0-9]*
/* Identifier: allow letters, underscores and digits (start with letter or underscore) */
VARS [a-zA-Z_][a-zA-Z0-9_]*
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

\"[^\"]*\" {
	/* string literal: remove surrounding quotes and return TEXTO */
	int len = strlen(yytext);
	if(len >= 2) {
		/* copy without the starting and ending quote */
		int copy_len = len - 2;
		if(copy_len > sizeof(yylval.str)-1) copy_len = sizeof(yylval.str)-1;
		strncpy(yylval.str, yytext+1, copy_len);
		yylval.str[copy_len] = '\0';
	} else {
		yylval.str[0] = '\0';
	}
	return TEXTO;
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
