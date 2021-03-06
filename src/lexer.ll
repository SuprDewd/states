%{
#include <string>
#include "ast.h"
#include "parser.hh"
extern "C" int yylex();

#define SAVE_SVAL (yylval.sval = new std::string(yytext, yyleng))

int lexer_line = 1;

%}

%option noyywrap

ALPHABET     alphabet
NFA          nfa
DFA          dfa
REGEX        regex
STATE        state
ACCEPT       accept
ON           on
LCBRACK      "{"
RCBRACK      "}"
SEMI         ";"

IDENTIFIER   [-_a-zA-Z0-9]+
CHAR         [a-zA-Z0-9]
CHARRANGE    [a-zA-Z0-9]-[a-zA-Z0-9]
EPSILON      "epsilon"

NL           \r?\n
WS           [\r\n\t ]+
NONSEP       [^\r\n\t ;{}]+

COMSTART     "/*"
COMEND       "*/"
STAR         "*"
PLUS         "+"
QUES         "?"
PIPE         "|"
LPAR         "("
RPAR         ")"


%x alphabet
%x state
%x on
%x nfa
%x dfa
%x regex
%x comment

%%

<INITIAL>{
    {COMSTART}     BEGIN(comment);

    {ALPHABET}     BEGIN(alphabet); return TALPHABET;
    {ACCEPT}       return TACCEPT;
    {STATE}        BEGIN(state); return TSTATE;
    {ON}           BEGIN(on); return TON;
    {NFA}          BEGIN(nfa); return TNFA;
    {DFA}          BEGIN(dfa); return TDFA;
    {REGEX}        BEGIN(regex); return TREGEX;
    {LCBRACK}      return TLCBRACK;
    {RCBRACK}      return TRCBRACK;
}

<comment>{
    {COMEND}       BEGIN(INITIAL);

    [^*\n]+        ;
    {STAR}         ;
    {NL}           ++lexer_line;
}

<alphabet>{
    {SEMI}         BEGIN(INITIAL); return TSEMI;

    {CHAR}         SAVE_SVAL; return TCHAR;
    {CHARRANGE}    SAVE_SVAL; return TCHARRANGE;
}

<state>{
    {IDENTIFIER}   SAVE_SVAL; BEGIN(INITIAL); return TIDENTIFIER;
}

<on>{
    {SEMI}         BEGIN(INITIAL); return TSEMI;

    {IDENTIFIER}   SAVE_SVAL; return TIDENTIFIER;
}

<nfa,dfa>{
    {IDENTIFIER}   SAVE_SVAL; BEGIN(INITIAL); return TIDENTIFIER;
}

<regex>{
    [ ]+           return TWS;
    {LPAR}         return TLPAR;
    {RPAR}         return TRPAR;
    {STAR}         return TSTAR;
    {PLUS}         return TPLUS;
    {QUES}         return TQUES;
    {PIPE}         return TPIPE;
    {EPSILON}      return TEPSILON;
    {CHAR}         SAVE_SVAL; return TCHAR;

    {NL}           ++lexer_line;

    .              printf("lexer: unrecognized token '%s' on line %d\n", yytext, lexer_line); exit(1);
}

<INITIAL,alphabet,state,on,nfa,dfa>{

    {NL}           ++lexer_line;

    /* Ignore whitespace */
    {WS}           ;

    /* Catch all unrecognized things */
    {NONSEP}       printf("lexer: unrecognized token '%s' on line %d\n", yytext, lexer_line); exit(1);
}


%%

