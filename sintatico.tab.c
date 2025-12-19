/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "sintatico.y"

	
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
			eval_str_result = eval2(a->left);
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

	

#line 460 "sintatico.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUM = 258,                     /* NUM  */
    VARS = 259,                    /* VARS  */
    TEXTO = 260,                   /* TEXTO  */
    FIM = 261,                     /* FIM  */
    IF = 262,                      /* IF  */
    ELSE = 263,                    /* ELSE  */
    WHILE = 264,                   /* WHILE  */
    PRINT = 265,                   /* PRINT  */
    DECL = 266,                    /* DECL  */
    SCAN = 267,                    /* SCAN  */
    PRINTT = 268,                  /* PRINTT  */
    SCANS = 269,                   /* SCANS  */
    CMP = 270,                     /* CMP  */
    IFX = 271,                     /* IFX  */
    VARPREC = 272,                 /* VARPREC  */
    DECLPREC = 273,                /* DECLPREC  */
    NEG = 274,                     /* NEG  */
    VET = 275                      /* VET  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 389 "sintatico.y"

	float flo;
	int fn;
	int inter;
	char str[50];
	Ast *a;

#line 535 "sintatico.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUM = 3,                        /* NUM  */
  YYSYMBOL_VARS = 4,                       /* VARS  */
  YYSYMBOL_TEXTO = 5,                      /* TEXTO  */
  YYSYMBOL_FIM = 6,                        /* FIM  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_ELSE = 8,                       /* ELSE  */
  YYSYMBOL_WHILE = 9,                      /* WHILE  */
  YYSYMBOL_PRINT = 10,                     /* PRINT  */
  YYSYMBOL_DECL = 11,                      /* DECL  */
  YYSYMBOL_SCAN = 12,                      /* SCAN  */
  YYSYMBOL_PRINTT = 13,                    /* PRINTT  */
  YYSYMBOL_SCANS = 14,                     /* SCANS  */
  YYSYMBOL_CMP = 15,                       /* CMP  */
  YYSYMBOL_16_ = 16,                       /* '='  */
  YYSYMBOL_17_ = 17,                       /* '+'  */
  YYSYMBOL_18_ = 18,                       /* '-'  */
  YYSYMBOL_19_ = 19,                       /* '*'  */
  YYSYMBOL_20_ = 20,                       /* '/'  */
  YYSYMBOL_IFX = 21,                       /* IFX  */
  YYSYMBOL_VARPREC = 22,                   /* VARPREC  */
  YYSYMBOL_DECLPREC = 23,                  /* DECLPREC  */
  YYSYMBOL_NEG = 24,                       /* NEG  */
  YYSYMBOL_VET = 25,                       /* VET  */
  YYSYMBOL_26_ = 26,                       /* '('  */
  YYSYMBOL_27_ = 27,                       /* ')'  */
  YYSYMBOL_28_ = 28,                       /* '{'  */
  YYSYMBOL_29_ = 29,                       /* '}'  */
  YYSYMBOL_30_ = 30,                       /* '['  */
  YYSYMBOL_31_ = 31,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_val = 33,                       /* val  */
  YYSYMBOL_prog = 34,                      /* prog  */
  YYSYMBOL_stmt = 35,                      /* stmt  */
  YYSYMBOL_list = 36,                      /* list  */
  YYSYMBOL_exp = 37,                       /* exp  */
  YYSYMBOL_exp1 = 38                       /* exp1  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   121

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  7
/* YYNRULES -- Number of rules.  */
#define YYNRULES  28
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  76

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   275


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      26,    27,    19,    17,     2,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    16,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    30,     2,    31,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,    29,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    21,    22,    23,    24,    25
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   414,   414,   417,   418,   423,   424,   425,   427,   428,
     430,   431,   433,   434,   435,   436,   439,   440,   444,   445,
     446,   447,   448,   449,   450,   451,   453,   454,   460
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUM", "VARS", "TEXTO",
  "FIM", "IF", "ELSE", "WHILE", "PRINT", "DECL", "SCAN", "PRINTT", "SCANS",
  "CMP", "'='", "'+'", "'-'", "'*'", "'/'", "IFX", "VARPREC", "DECLPREC",
  "NEG", "VET", "'('", "')'", "'{'", "'}'", "'['", "']'", "$accept", "val",
  "prog", "stmt", "list", "exp", "exp1", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-59)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      94,    -8,   -21,   -20,   -19,     5,    -3,    -2,    -1,    26,
      94,   -59,    40,    30,    40,    40,    40,     4,    42,    45,
      52,   -59,   -59,   -59,    35,    40,    40,    95,    29,    44,
      58,    64,    66,    53,   -59,    59,    60,    69,   -59,    75,
      40,    40,    40,    40,    40,    72,    61,    68,   -59,    78,
     -59,   -59,   -59,    80,   -59,   -59,   101,   101,    82,    82,
      40,    94,    94,   -59,   -59,    95,   -59,     7,    28,    91,
     -59,   -59,    89,    94,    41,   -59
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,     3,     0,     0,     0,     0,     0,    10,     0,     0,
       0,     1,     4,    25,    26,     0,     0,     8,     0,     0,
       0,     0,     0,     0,    28,     0,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
      14,    13,    15,     0,    23,    22,    18,    19,    20,    21,
       0,     0,     0,    11,    27,     9,    16,     0,     0,     5,
      17,     7,     0,     0,     0,     6
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -59,   -59,   -59,     0,   -58,   -13,   -59
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     9,    10,    66,    67,    27,    35
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      11,    29,    30,    31,    68,    14,    15,    16,    12,    17,
      22,     1,    38,    39,     2,    74,     3,     4,     5,     6,
       7,     8,    13,    18,    19,    20,    21,    55,    56,    57,
      58,    59,     1,    28,    32,     2,    69,     3,     4,     5,
       6,     7,     8,    23,    24,     1,    33,    65,     2,    34,
       3,     4,     5,     6,     7,     8,    36,    71,    25,    40,
      45,    41,    42,    43,    44,    37,    26,    70,    70,    49,
      75,    46,    53,    40,    70,    41,    42,    43,    44,    40,
      50,    41,    42,    43,    44,    47,    51,    52,    60,    61,
      40,    48,    41,    42,    43,    44,    62,    40,     1,    72,
       0,     2,    54,     3,     4,     5,     6,     7,     8,    63,
      40,    64,    41,    42,    43,    44,    40,    73,     0,     0,
      43,    44
};

static const yytype_int8 yycheck[] =
{
       0,    14,    15,    16,    62,    26,    26,    26,    16,     4,
      10,     4,    25,    26,     7,    73,     9,    10,    11,    12,
      13,    14,    30,    26,    26,    26,     0,    40,    41,    42,
      43,    44,     4,     3,    30,     7,    29,     9,    10,    11,
      12,    13,    14,     3,     4,     4,     4,    60,     7,     4,
       9,    10,    11,    12,    13,    14,     4,    29,    18,    15,
      31,    17,    18,    19,    20,    30,    26,    67,    68,     3,
      29,    27,     3,    15,    74,    17,    18,    19,    20,    15,
      27,    17,    18,    19,    20,    27,    27,    27,    16,    28,
      15,    27,    17,    18,    19,    20,    28,    15,     4,     8,
      -1,     7,    27,     9,    10,    11,    12,    13,    14,    31,
      15,    31,    17,    18,    19,    20,    15,    28,    -1,    -1,
      19,    20
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     4,     7,     9,    10,    11,    12,    13,    14,    33,
      34,    35,    16,    30,    26,    26,    26,     4,    26,    26,
      26,     0,    35,     3,     4,    18,    26,    37,     3,    37,
      37,    37,    30,     4,     4,    38,     4,    30,    37,    37,
      15,    17,    18,    19,    20,    31,    27,    27,    27,     3,
      27,    27,    27,     3,    27,    37,    37,    37,    37,    37,
      16,    28,    28,    31,    31,    37,    35,    36,    36,    29,
      35,    29,     8,    28,    36,    29
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    36,    36,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    38
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     7,    11,     7,     3,     6,
       2,     5,     4,     4,     4,     4,     1,     2,     3,     3,
       3,     3,     3,     3,     2,     1,     1,     4,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* prog: stmt  */
#line 417 "sintatico.y"
                        {eval((yyvsp[0].a));}
#line 1594 "sintatico.tab.c"
    break;

  case 4: /* prog: prog stmt  */
#line 418 "sintatico.y"
                    {eval((yyvsp[0].a));}
#line 1600 "sintatico.tab.c"
    break;

  case 5: /* stmt: IF '(' exp ')' '{' list '}'  */
#line 423 "sintatico.y"
                                            {(yyval.a) = newflow('I', (yyvsp[-4].a), (yyvsp[-1].a), NULL);}
#line 1606 "sintatico.tab.c"
    break;

  case 6: /* stmt: IF '(' exp ')' '{' list '}' ELSE '{' list '}'  */
#line 424 "sintatico.y"
                                                        {(yyval.a) = newflow('I', (yyvsp[-8].a), (yyvsp[-5].a), (yyvsp[-1].a));}
#line 1612 "sintatico.tab.c"
    break;

  case 7: /* stmt: WHILE '(' exp ')' '{' list '}'  */
#line 425 "sintatico.y"
                                         {(yyval.a) = newflow('W', (yyvsp[-4].a), (yyvsp[-1].a), NULL);}
#line 1618 "sintatico.tab.c"
    break;

  case 8: /* stmt: VARS '=' exp  */
#line 427 "sintatico.y"
                                     { (yyval.a) = newasgn((yyvsp[-2].str),(yyvsp[0].a));}
#line 1624 "sintatico.tab.c"
    break;

  case 9: /* stmt: VARS '[' NUM ']' '=' exp  */
#line 428 "sintatico.y"
                                 {(yyval.a) = newasgn_a((yyvsp[-5].str),(yyvsp[0].a),(yyvsp[-3].flo));}
#line 1630 "sintatico.tab.c"
    break;

  case 10: /* stmt: DECL VARS  */
#line 430 "sintatico.y"
                                        { (yyval.a) = newvarnode('V',(yyvsp[0].str));}
#line 1636 "sintatico.tab.c"
    break;

  case 11: /* stmt: DECL VARS '[' NUM ']'  */
#line 431 "sintatico.y"
                                { (yyval.a) = newarray('a',(yyvsp[-3].str),(yyvsp[-1].flo));}
#line 1642 "sintatico.tab.c"
    break;

  case 12: /* stmt: PRINT '(' exp ')'  */
#line 433 "sintatico.y"
                                {(yyval.a) = newast('P',(yyvsp[-1].a),NULL);}
#line 1648 "sintatico.tab.c"
    break;

  case 13: /* stmt: PRINTT '(' exp1 ')'  */
#line 434 "sintatico.y"
                                {(yyval.a) = newast('Y',(yyvsp[-1].a),NULL);}
#line 1654 "sintatico.tab.c"
    break;

  case 14: /* stmt: SCAN '(' VARS ')'  */
#line 435 "sintatico.y"
                                        {(yyval.a) = newvarnode('S',(yyvsp[-1].str));}
#line 1660 "sintatico.tab.c"
    break;

  case 15: /* stmt: SCANS '(' VARS ')'  */
#line 436 "sintatico.y"
                                        {(yyval.a) = newvarnode('T',(yyvsp[-1].str));}
#line 1666 "sintatico.tab.c"
    break;

  case 16: /* list: stmt  */
#line 439 "sintatico.y"
           {(yyval.a) = (yyvsp[0].a);}
#line 1672 "sintatico.tab.c"
    break;

  case 17: /* list: list stmt  */
#line 440 "sintatico.y"
                            { (yyval.a) = newast('L', (yyvsp[-1].a), (yyvsp[0].a));	}
#line 1678 "sintatico.tab.c"
    break;

  case 18: /* exp: exp '+' exp  */
#line 444 "sintatico.y"
                     {(yyval.a) = newast('+',(yyvsp[-2].a),(yyvsp[0].a));}
#line 1684 "sintatico.tab.c"
    break;

  case 19: /* exp: exp '-' exp  */
#line 445 "sintatico.y"
                     {(yyval.a) = newast('-',(yyvsp[-2].a),(yyvsp[0].a));}
#line 1690 "sintatico.tab.c"
    break;

  case 20: /* exp: exp '*' exp  */
#line 446 "sintatico.y"
                     {(yyval.a) = newast('*',(yyvsp[-2].a),(yyvsp[0].a));}
#line 1696 "sintatico.tab.c"
    break;

  case 21: /* exp: exp '/' exp  */
#line 447 "sintatico.y"
                     {(yyval.a) = newast('/',(yyvsp[-2].a),(yyvsp[0].a));}
#line 1702 "sintatico.tab.c"
    break;

  case 22: /* exp: exp CMP exp  */
#line 448 "sintatico.y"
                     {(yyval.a) = newcmp((yyvsp[-1].fn),(yyvsp[-2].a),(yyvsp[0].a));}
#line 1708 "sintatico.tab.c"
    break;

  case 23: /* exp: '(' exp ')'  */
#line 449 "sintatico.y"
                     {(yyval.a) = (yyvsp[-1].a);}
#line 1714 "sintatico.tab.c"
    break;

  case 24: /* exp: '-' exp  */
#line 450 "sintatico.y"
                           {(yyval.a) = newast('M',(yyvsp[0].a),NULL);}
#line 1720 "sintatico.tab.c"
    break;

  case 25: /* exp: NUM  */
#line 451 "sintatico.y"
                {(yyval.a) = newnum((yyvsp[0].flo));}
#line 1726 "sintatico.tab.c"
    break;

  case 26: /* exp: VARS  */
#line 453 "sintatico.y"
                          {(yyval.a) = new_value_reference((yyvsp[0].str));}
#line 1732 "sintatico.tab.c"
    break;

  case 27: /* exp: VARS '[' NUM ']'  */
#line 454 "sintatico.y"
                        {(yyval.a) = new_array_value_reference((yyvsp[-3].str),(yyvsp[-1].flo));}
#line 1738 "sintatico.tab.c"
    break;

  case 28: /* exp1: VARS  */
#line 460 "sintatico.y"
             {(yyval.a) = new_string_value_reference((yyvsp[0].str));}
#line 1744 "sintatico.tab.c"
    break;


#line 1748 "sintatico.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 462 "sintatico.y"


#include "lex.yy.c"

int main(int argc, char **argv){
	const char *filename = NULL;
	FILE *input = NULL;

	if(argc > 1){
		filename = argv[1];
		input = fopen(filename, "r");
	} else {
		filename = "entrada.ipl";
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
    fprintf(stderr, "%s\n", s);
}

