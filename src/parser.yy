%{
#include <cstdio>
#include "ast.h"

extern "C" int yylex();
void yyerror(const char *s) { std::printf("parser: %s\n", s); }

ast_program *main_program;

%}

%define parse.lac full
%define parse.error verbose

%union {
    ast_program *program;
    ast_alphabet *alphabet;
    ast_nfa *nfa;
    ast_state *state;
    ast_statelist *statelist;
    ast_onlist *onlist;
    ast_on *on;
    std::string *sval;
}

%token TALPHABET
%token TNFA TDFA
%token TLPAR TRPAR TSEMI
%token TACCEPT TSTATE TON
%token <sval> TCHAR TCHARRANGE TIDENTIFIER

%type <program> program
%type <alphabet> alphabetlist alphabetdecl
%type <nfa> nfa
%type <state> state
%type <statelist> statelist
%type <onlist> onlist
%type <on> on
%type <sval> ident char charrange

%start program

%%

program: alphabetdecl nfa { $$ = new ast_program($1, $2); main_program = $$; }
       ;

alphabetdecl: TALPHABET alphabetlist { $$ = $2; }
            ;

alphabetlist: char TSEMI { $$ = new ast_alphabet(); $$->add_front((*$1)[0]); delete $1; }
            | charrange TSEMI { $$ = new ast_alphabet(); $$->add_range_front((*$1)[0], (*$1)[2]); delete $1; }
            | char alphabetlist { $$ = $2; $$->add_front((*$1)[0]); delete $1; }
            | charrange alphabetlist { $$ = $2; $$->add_range_front((*$1)[0], (*$1)[2]); delete $1; }
            ;

nfa: TNFA ident TLPAR statelist TRPAR { $$ = new ast_nfa(*$2, *$4); delete $2; delete $4; }
   ;

statelist: state { $$ = new ast_statelist(); $$->push_back($1); }
         | state statelist { $$ = $2; $$->push_front($1); }
         ;

state: TSTATE ident TLPAR onlist TRPAR { $$ = new ast_state(*$2, false, *$4); delete $2; delete $4; }
     | TACCEPT TSTATE ident TLPAR onlist TRPAR { $$ = new ast_state(*$3, true, *$5); delete $3; delete $5; }
     ;

onlist: on { $$ = new ast_onlist(); $$->push_back($1); }
      | on onlist { $$ = $2; $$->push_front($1); }
      ;

on: TON ident ident TSEMI { $$ = new ast_on(*$2, *$3); delete $2; delete $3; }
  ;

ident: TIDENTIFIER { $$ = $1; }
     ;

char: TCHAR { $$ = $1; }
    ;

charrange: TCHARRANGE { $$ = $1; }
         ;

%%

