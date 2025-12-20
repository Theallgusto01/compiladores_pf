%{
	
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>	

// 
/*
    Tipos -> nodetype:
        1 - número (double)
        2 - string (texto)
        3 - vetor (array de double)
*/

typedef struct var_entry{ /* Estrutura para entrada na tabela de símbolos */ 
    int nodetype;           /* tipo do valor armazenado */
    char identifier[50];    /* nome da variável */
    double value;           /* valor numérico (se for número) */
    char string_value[50];  /* valor textual (se for string) */
    double *array;          /* ponteiro para array (se for vetor) */
    struct var_entry *next; /* próximo na lista ligada */
} VarEntry;

// Insere uma nova variável (escala) no início da lista
VarEntry *insert_variable_entry(VarEntry *list, char name[]){
	VarEntry *new_entry = (VarEntry*)malloc(sizeof(VarEntry));
	strcpy(new_entry->identifier, name);
	new_entry->next = list;
	new_entry->nodetype = 1; /* por padrão, trata como número */
	return new_entry;
}

// Insere um array (vetor) como variável na lista
VarEntry *insert_array_entry(VarEntry *list, char name[], int size){
	VarEntry *new_entry = (VarEntry*)malloc(sizeof(VarEntry));
	strcpy(new_entry->identifier, name);
	new_entry->array = (double*)malloc(size * sizeof(double));
	new_entry->next = list;
	new_entry->nodetype = 3; /* vetor */
	return new_entry;
}

// Busca uma variável por nome na lista
VarEntry *search_variable_entry(VarEntry *list, char name[]){
	VarEntry *it = list;
	while(it != NULL){
		if(strcmp(name, it->identifier) == 0)
			return it;
		it = it->next;
	}
	return it; /* NULL se não encontrar */
}
	
		
		/*O nodetype serve para indicar 
		o tipo de nó que está na árvore. 
		Isso serve para a função eval()
		entender o que realizar naquele nó*/

/*************** Estrutura da AST ******************/

typedef struct ast { /*Estrutura de um nó*/
	int nodetype; /*Tipo do nó*/
	struct ast *left; /*Esquerda*/
	struct ast *right; /*Direita*/
}Ast; 

typedef struct numval { /*Estrutura de um número*/
	int nodetype;
	double number;
}Numval;

typedef struct varval { /* Estrutura de uma variável*/
	int nodetype;
	char var[50];
	int size;
}Varval;


typedef struct text_node { /* Estrutura de um texto*/
	int nodetype;
	char text[500];
}TXT;

	
typedef struct flow { /*Estrutura de um desvio (if/else/while)*/
	int nodetype;
	Ast *condition;	/*condição*/
	Ast *then_list;	/*then*/
	Ast *else_list;	/*else*/
}Flow;

typedef struct assign { /*Estrutura para um nó de atribuição. Para atrubior o valor de v em s*/
	int nodetype; 		// tipo do nó
	char var_name[50]; // nome da variável
	Ast *value_node; // nó que representa o valor a ser atribuído
	int position;    // posição no array, se aplicável
}Assig;

/* **************************************************** */


/* Lista global de variáveis (início e auxiliar) */

VarEntry *var_list; /* início da lista de variáveis */



/* Funções para criação de nós na AST */

Ast * newast(int nodetype, Ast *l, Ast *r){ /*Função de que cria um novo nó na AST*/

	Ast *a = (Ast*) malloc(sizeof(Ast)); // Novo nó
	if(!a) {							// Verifica se a alocação foi bem sucedida
		printf("out of space"); 
		exit(0);
	}
	a->nodetype = nodetype;
	a->left = l;
	a->right = r;
	return a;
}
 
Ast * newvarnode(int nodetype, char name[50]) { /*Função de que cria uma nova variável*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->var,name);
	return (Ast*)a;
}

	Ast * newarray(int nodetype, char name[50], int length) {/*Função de que cria um novo array*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->var,name);
	a->size = length;
	return (Ast*)a;
}	
	
Ast * newtext(int nodetype, char text_value[500]) {/*Função de que cria um nó do tipo texto*/
	TXT *a = (TXT*) malloc(sizeof(TXT));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->text,text_value);
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
	a->nodetype = nodetype;
	a->condition = cond;
	a->then_list = tl;
	a->else_list = el;
	return (Ast *)a;
}

Ast * newcmp(int cmptype, Ast *l, Ast *r){ /*Função que cria um nó para testes*/
	Ast *a = (Ast*)malloc(sizeof(Ast));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '0' + cmptype; /*Para pegar o tipe de teste, definido no arquivo.l e utilizar na função eval()*/
	a->left = l;
	a->right = r;
	return a;
}

Ast * newasgn(char s[50], Ast *v) { /*Função para um nó de atribuição*/
	Assig *a = (Assig*)malloc(sizeof(Assig));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->var_name,s);
	a->value_node = v; /*Valor*/
	return (Ast *)a;
}

Ast * newasgn_a(char s[50], Ast *v, int index) { /*Função para um nó de atribuição*/
	Assig *a = (Assig*)malloc(sizeof(Assig));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->var_name,s);
	a->value_node = v; /*Valor*/
	a->position = index;
	return (Ast *)a;
}
	
	

Ast * new_value_reference(char s[]) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'N';
	strcpy(a->var,s);
	return (Ast*)a;
}
	
Ast * new_array_value_reference(char s[], int index) { /*Função que recupera o nome/referência de um nó tipo array*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'n';
	strcpy(a->var,s);
	a->size = index;
	
	return (Ast*)a;
}	

Ast * new_string_value_reference(char s[50]) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'Q';
	strcpy(a->var,s);
	return (Ast*)a;
	
}

	

VarEntry *srch(VarEntry *list, char name[]){
	VarEntry *it = list;
	while(it){
		if(strcmp(it->identifier, name) == 0) return it;
		it = it->next;
	}
	return NULL;
}

VarEntry *ins(VarEntry *list, char name[]){
	return insert_variable_entry(list, name);
}

VarEntry *ins_a(VarEntry *list, char name[], int size){
	return insert_array_entry(list, name, size);
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
double eval(Ast * a) {
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

		case '+': v = eval(a->left) + eval(a->right); break;
		case '-': v = eval(a->left) - eval(a->right); break;
		case '*': v = eval(a->left) * eval(a->right); break;
		case '/': v = eval(a->left) / eval(a->right); break;
		case 'M': v = -eval(a->left); break; /* negação */

		case '1': v = (eval(a->left) > eval(a->right))? 1 : 0; break;
		case '2': v = (eval(a->left) < eval(a->right))? 1 : 0; break;
		case '3': v = (eval(a->left) != eval(a->right))? 1 : 0; break;
		case '4': v = (eval(a->left) == eval(a->right))? 1 : 0; break;
		case '5': v = (eval(a->left) >= eval(a->right))? 1 : 0; break;
		case '6': v = (eval(a->left) <= eval(a->right))? 1 : 0; break;

		case '=': {
			double val = eval(((Assig *)a)->value_node);
			VarEntry *target = srch(var_list, ((Assig *)a)->var_name);
			if(target){
				if(target->nodetype == 1) target->value = val;
				else if(target->array) target->array[((Assig *)a)->position] = val;
			}
		} break;

		case 'I': /* if */
			if (eval(((Flow *)a)->condition) != 0) {
				if (((Flow *)a)->then_list) v = eval(((Flow *)a)->then_list); else v = 0.0;
			} else {
				if (((Flow *)a)->else_list) v = eval(((Flow *)a)->else_list); else v = 0.0;
			}
			break;

		case 'W': /* while */
			v = 0.0;
			if(((Flow *)a)->then_list) {
				while(eval(((Flow *)a)->condition) != 0) v = eval(((Flow *)a)->then_list);
			}
			break;

		case 'L': eval(a->left); v = eval(a->right); break; /* lista */

		case 'P': v = eval(a->left); printf("%.2f\n", v); break; /* print */

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
				/* print a string. a->left can be either a string-variable reference (nodetype 'Q')
				   or a literal text node (we'll use nodetype 'X' for text literals). */
				if(a->left) {
					if(a->left->nodetype == 'Q'){
						eval_str_result = eval2(a->left);
						if(eval_str_result) printf("%s\n", eval_str_result);
					} else if(a->left->nodetype == 'X'){
						printf("%s\n", ((TXT *)a->left)->text);
					} else {
						/* unexpected node type for string print */
						printf("(error) cannot print node type %c\n", a->left->nodetype);
					}
				}
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

val: prog
	;

prog: stmt 		{eval($1);}  
	| prog stmt {eval($2);}	 
	;
	
/*Funções para análise sintática e criação dos nós na AST*/	
	
stmt: IF '(' exp ')' '{' list '}' %prec IFX {$$ = newflow('I', $3, $6, NULL);}
	| IF '(' exp ')' '{' list '}' ELSE '{' list '}' {$$ = newflow('I', $3, $6, $10);}
	| WHILE '(' exp ')' '{' list '}' {$$ = newflow('W', $3, $6, NULL);}

	| VARS '=' exp %prec VARPREC { $$ = newasgn($1,$3);}
	| VARS '['NUM']' '=' exp {$$ = newasgn_a($1,$6,$3);}

	| DECL VARS	 %prec DECLPREC { $$ = newvarnode('V',$2);}
	| DECL VARS '['NUM']'	{ $$ = newarray('a',$2,$4);}
	
	| PRINT '(' exp ')' 	{$$ = newast('P',$3,NULL);}
	| PRINT '(' exp1 ')' 	{$$ = newast('Y',$3,NULL);} 
	| PRINTT '(' exp1 ')' 	{$$ = newast('Y',$3,NULL);}
	| SCAN '('VARS')'		{$$ = newvarnode('S',$3);}
	| SCANS '('VARS')'		{$$ = newvarnode('T',$3);}
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
	
	|VARS 	%prec VET {$$ = new_value_reference($1);} 		/*token de uma variável*/
	|VARS '['NUM']' {$$ = new_array_value_reference($1,$3);} 			/*token de um elemento de um array*/
	;

// a = 4 + b[5]

exp1: 
	VARS {$$ = new_string_value_reference($1);} 			
	| TEXTO { $$ = newtext('X', $1); }
	;
%%

#include "lex.yy.c"

int main(int argc, char **argv){
	const char *filename = NULL;
	FILE *input = NULL;

	if(argc > 1){
		filename = argv[1];
		input = fopen(filename, "r");
	} else {
		filename = "entrada.d";
		input = fopen(filename, "r");
		if(!input){
			filename = "entrada.txt";
			input = fopen(filename, "r");
		}
	}

	if(!input){
		fprintf(stderr, "Unable to open input file: %s\n", filename);
		return 1;
	}

	yyin = input;
	yyparse();
	yylex();
	fclose(input);
	return 0;
}

/* Implementação de yyerror exigida pelo parser gerado pelo Bison */
void yyerror(char *s){
	/* show the offending token for easier debugging */
	extern char *yytext;
	fprintf(stderr, "%s at '%s'\n", s, yytext ? yytext : "(null)");
}

