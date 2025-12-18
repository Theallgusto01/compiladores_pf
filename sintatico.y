%{
	
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>	

// Representação de uma entrada na tabela de variáveis (lista ligada)
/*
    nodetype:
        1 - número (double)
        2 - string (texto)
        3 - vetor (array de double)
*/
typedef struct var_entry{
    int nodetype;         /* tipo do valor armazenado */
    char identifier[50];  /* nome da variável */
    double value;         /* valor numérico (se for número) */
    char string_value[50];/* valor textual (se for string) */
    double *array;        /* ponteiro para array (se for vetor) */
    struct var_entry *next; /* próximo na lista ligada */
} VarEntry;

// Insere uma nova variável (escala) no início da lista
VarEntry *insere_variavel(VarEntry *list, char name[]){
    VarEntry *new = (VarEntry*)malloc(sizeof(VarEntry));
    strcpy(new->identifier, name);
    new->next = list;
    new->nodetype = 1; /* por padrão, trata como número */
    return new;
}

// Insere um array (vetor) como variável na lista
VarEntry *insere_variavel_array(VarEntry *list, char name[], int tamanho){
    VarEntry *new = (VarEntry*)malloc(sizeof(VarEntry));
    strcpy(new->identifier, name);
    new->array = (double*)malloc(tamanho * sizeof(double));
    new->next = list;
    new->nodetype = 3; /* vetor */
    return new;
}

// Busca uma variável por nome na lista ligada
VarEntry *busca_variavel(VarEntry *list, char name[]){
    VarEntry *it = list;
    while(it != NULL){
        if(strcmp(name, it->identifier) == 0)
            return it;
        it = it->next;
    }
    return it; /* NULL se não encontrar */
}
	
		
		/*O node type serve para indicar 
		o tipo de nó que está na árvore. 
		Isso serve para a função eval()
		entender o que realizar naquele nó*/
		
typedef struct ast { /*Estrutura de um nó*/
	int nodetype;
	struct ast *l; /*Esquerda*/
	struct ast *r; /*Direita*/
}Ast; 

typedef struct numval { /*Estrutura de um número*/
	int nodetype;
	double number;
}Numval;

typedef struct varval { /*Estrutura de um nome de variável*/
	int nodetype;
	char var[50];
	int size;
}Varval;


typedef struct texto { 
	int nodetype;
	char txt[50];
}TXT;

	
typedef struct flow { /*Estrutura de um desvio (if/else/while)*/
	int nodetype;
	Ast *cond;		/*condição*/
	Ast *tl;		/*then, ou seja, verdade*/
	Ast *el;		/*else*/
}Flow;

typedef struct symasgn { /*Estrutura para um nó de atribuição. Para atrubior o valor de v em s*/
	int nodetype;
	char s[50];
	Ast *v;
	int pos;
}Symasgn;


/* Lista global de variáveis (início e auxiliar) */
VarEntry *var_list; /* início da lista de variáveis */
VarEntry *var_aux;  /* ponteiro auxiliar para operações sobre variáveis */

Ast * newast(int nodetype, Ast *l, Ast *r){ /*Função para criar um nó*/

	Ast *a = (Ast*) malloc(sizeof(Ast));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}
 
Ast * newvari(int nodetype, char nome[50]) {/*Função de que cria uma nova variável*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->var,nome);
	return (Ast*)a;
}

Ast * newarray(int nodetype, char nome[50], int tam) {/*Função de que cria um novo array*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->var,nome);
	a->size = tam;
	return (Ast*)a;
}	
	
Ast * newtext(int nodetype, char txt[500]) {/*Função de que cria um nó do tipo texto*/
	TXT *a = (TXT*) malloc(sizeof(TXT));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->txt,txt);
	return (Ast*)a;
}	
	
Ast * newnum(double d) {			/*Função de que cria um novo número*/
	Numval *a = (Numval*) malloc(sizeof(Numval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'K';
	a->number = d;
	return (Ast*)a;
}	
	
Ast * newflow(int nodetype, Ast *cond, Ast *tl, Ast *el){ /*Função que cria um nó de if/else/while*/
	Flow *a = (Flow*)malloc(sizeof(Flow));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = nodetype;
	a->cond = cond;
	a->tl = tl;
	a->el = el;
	return (Ast *)a;
}

Ast * newcmp(int cmptype, Ast *l, Ast *r){ /*Função que cria um nó para testes*/
	Ast *a = (Ast*)malloc(sizeof(Ast));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '0' + cmptype; /*Para pegar o tipe de teste, definido no arquivo.l e utilizar na função eval()*/
	a->l = l;
	a->r = r;
	return a;
}

Ast * newasgn(char s[50], Ast *v) { /*Função para um nó de atribuição*/
	Symasgn *a = (Symasgn*)malloc(sizeof(Symasgn));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->s,s);
	a->v = v; /*Valor*/
	return (Ast *)a;
}

Ast * newasgn_a(char s[50], Ast *v, int indice) { /*Função para um nó de atribuição*/
	Symasgn *a = (Symasgn*)malloc(sizeof(Symasgn));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->s,s);
	a->v = v; /*Valor*/
	a->pos = indice;
	return (Ast *)a;
}
	
	
Ast * newValorVal(char s[]) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'N';
	strcpy(a->var,s);
	return (Ast*)a;
}
	
Ast * newValorVal_a(char s[], int indice) { /*Função que recupera o nome/referência de um nó tipo array*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'n';
	strcpy(a->var,s);
	a->size = indice;
	
	return (Ast*)a;
}	

Ast * newValorValS(char s[50]) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'Q';
	strcpy(a->var,s);
	return (Ast*)a;
	
}

	
/* Wrappers e helpers para manter compatibilidade com chamadas externas
   (o projeto original usava nomes como srch, ins, ins_a). */
VarEntry *srch(VarEntry *list, char name[]){
	VarEntry *it = list;
	while(it){
		if(strcmp(it->identifier, name) == 0) return it;
		it = it->next;
	}
	return NULL;
}

VarEntry *ins(VarEntry *list, char name[]){
	return insere_variavel(list, name);
}

VarEntry *ins_a(VarEntry *list, char name[], int tamanho){
	return insere_variavel_array(list, name, tamanho);
}

/* Recupera string a partir de um nó (usado por PRINTT) */
char * eval2(Ast *a) {
	if(!a) return NULL;
	if(a->nodetype != 'Q'){
		printf("internal error: bad node %c\n", a->nodetype);
		return NULL;
	}
	VarEntry *f = srch(var_list, ((Varval *)a)->var);
	return f ? f->string_value : NULL;
}

/* Avalia a AST e retorna um valor numérico (double). */
double eval(Ast *a) {
	double v = 0.0;
	char input_str_buf[50];
	char *eval_str_result = NULL;
	VarEntry *found_var;

	if(!a) { printf("internal error, null eval\n"); return 0.0; }

	switch(a->nodetype) {
		case 'K': /* número literal */
			v = ((Numval *)a)->number;
			break;

		case 'N': /* variável escalar */
			found_var = srch(var_list, ((Varval *)a)->var);
			if(found_var) v = found_var->value; else v = 0.0;
			break;

		case 'n': /* elemento de vetor */
			found_var = srch(var_list, ((Varval *)a)->var);
			if(found_var && found_var->array) v = found_var->array[((Varval *)a)->size]; else v = 0.0;
			break;

		case '+': v = eval(a->l) + eval(a->r); break;
		case '-': v = eval(a->l) - eval(a->r); break;
		case '*': v = eval(a->l) * eval(a->r); break;
		case '/': v = eval(a->l) / eval(a->r); break;
		case 'M': v = -eval(a->l); break; /* negação */

		case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
		case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
		case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
		case '4': v = (eval(a->l) == eval(a->r))? 1 : 0; break;
		case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
		case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;

		case '=': {
			double val = eval(((Symasgn *)a)->v);
			VarEntry *target = srch(var_list, ((Symasgn *)a)->s);
			if(target){
				if(target->nodetype == 1) target->value = val;
				else if(target->array) target->array[((Symasgn *)a)->pos] = val;
			}
		} break;

		case 'I': /* if */
			if (eval(((Flow *)a)->cond) != 0) {
				if (((Flow *)a)->tl) v = eval(((Flow *)a)->tl); else v = 0.0;
			} else {
				if (((Flow *)a)->el) v = eval(((Flow *)a)->el); else v = 0.0;
			}
			break;

		case 'W': /* while */
			v = 0.0;
			if(((Flow *)a)->tl) {
				while(eval(((Flow *)a)->cond) != 0) v = eval(((Flow *)a)->tl);
			}
			break;

		case 'L': eval(a->l); v = eval(a->r); break; /* lista */

		case 'P': v = eval(a->l); printf("%.2f\n", v); break; /* print */

		case 'S': /* scan numero */
			scanf("%lf", &v);
			found_var = srch(var_list, ((Varval *)a)->var);
			if(found_var) found_var->value = v;
			break;

		case 'T': /* scan string */
			scanf("%s", input_str_buf);
			found_var = srch(var_list, ((Varval *)a)->var);
			if(found_var) strcpy(found_var->string_value, input_str_buf);
			break;

		case 'Y': /* print string */
			eval_str_result = eval2(a->l);
			if(eval_str_result) printf("%s\n", eval_str_result);
			break;

		case 'V': /* declare var */
			var_list = ins(var_list, ((Varval*)a)->var);
			break;

		case 'a': /* declare array */
			var_list = ins_a(var_list, ((Varval*)a)->var, ((Varval*)a)->size);
			break;

		default:
			printf("internal error: bad node %c\n", a->nodetype);
			break;
	}
	return v;
}

	/* protótipos e funções de auxílio para o parser/lexer */
	int yylex(void);
	void yyerror(char *s);

	%}

%union{
	float flo;
	int fn;
	int inter;
	char str[50];
	Ast *a;
}

%token <flo>NUM
%token <str>VARS
%token <str>TEXTO
%token FIM IF ELSE WHILE PRINT DECL SCAN PRINTT SCANS
%token <fn> CMP

%right '='
%left '+' '-'
%left '*' '/'
%left CMP

%type <a> exp list stmt prog exp1

%nonassoc IFX VARPREC DECLPREC NEG VET 

%%

val: prog FIM
	;

prog: stmt 		{eval($1);}  /*Inicia e execução da árvore de derivação*/
	| prog stmt {eval($2);}	 /*Inicia e execução da árvore de derivação*/
	;
	
/*Funções para análise sintática e criação dos nós na AST*/	
/*Verifique q nenhuma operação é realizada na ação semântica, apenas são criados nós na árvore de derivação com suas respectivas operações*/
	
stmt: IF '(' exp ')' '{' list '}' %prec IFX {$$ = newflow('I', $3, $6, NULL);}
	| IF '(' exp ')' '{' list '}' ELSE '{' list '}' {$$ = newflow('I', $3, $6, $10);}
	| WHILE '(' exp ')' '{' list '}' {$$ = newflow('W', $3, $6, NULL);}

	| VARS '=' exp %prec VARPREC { $$ = newasgn($1,$3);}
	| VARS '['NUM']' '=' exp {$$ = newasgn_a($1,$6,$3);}

	| DECL VARS	 %prec DECLPREC { $$ = newvari('V',$2);}
	| DECL VARS '['NUM']'	{ $$ = newarray('a',$2,$4);}
	
	| PRINT '(' exp ')' 	{$$ = newast('P',$3,NULL);}
	| PRINTT '(' exp1 ')' 	{$$ = newast('Y',$3,NULL);}
	| SCAN '('VARS')'		{$$ = newvari('S',$3);}
	| SCANS '('VARS')'		{$$ = newvari('T',$3);}
	;

list: stmt {$$ = $1;}
		| list stmt { $$ = newast('L', $1, $2);	}
		;
	
exp: 
	 exp '+' exp {$$ = newast('+',$1,$3);}		/*Expressões matemáticas*/
	|exp '-' exp {$$ = newast('-',$1,$3);}
	|exp '*' exp {$$ = newast('*',$1,$3);}
	|exp '/' exp {$$ = newast('/',$1,$3);}
	|exp CMP exp {$$ = newcmp($2,$1,$3);}		/*Testes condicionais*/
	|'(' exp ')' {$$ = $2;}
	|'-' exp %prec NEG {$$ = newast('M',$2,NULL);}
	|NUM 	{$$ = newnum($1);}						/*token de um número*/
	
	|VARS 	%prec VET {$$ = newValorVal($1);}		/*token de uma variável*/
	|VARS '['NUM']' {$$ = newValorVal_a($1,$3);}				/*token de um elemento de um array*/
	;

// a = 4 + b[5]

exp1: 
	VARS {$$ = newValorValS($1);}				
	;
%%

#include "lex.yy.c"

int main(){
	
	yyin=fopen("entrada.txt","r");
	yyparse();
	yylex();
	fclose(yyin);
return 0;
}

/* Implementação de yyerror exigida pelo parser gerado pelo Bison */
void yyerror(char *s){
    fprintf(stderr, "%s\n", s);
}

