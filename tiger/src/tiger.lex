%{
#include <string.h>
#include <stdlib.h> // @see https://stackoverflow.com/questions/24704468/malloc-returning-address-that-i-cannot-access
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

// TODO:
//   handle comments
//   handle strings
//   error handling
//   EOF handling
//   ...
// NOTE:
//   Identifiers
//   Comments
//   Integer literal
//   String literal
// Reserved words:
//   while, for, to, break, let, in, end,
//   function, var, type, array, if, then,
//   else, do, of, nil
// Punctuation symbols:
//   , : ; ( ) [ ] { } . + - * / = <> < <= > >= & | :=

// extern int   yyleng; // the length of the matched string
// extern char* yytext; // the literal string matched by regular expression

int charPos = 1;

int yywrap()
{
	charPos = 1;
	return 1;
}

void adjust()
{
	EM_tokPos = charPos;
	charPos += yyleng;
}
%}

%%
" "            { adjust(); continue; }
\n             { adjust(); EM_newline(); continue; }
","            { adjust(); return COMMA; }
for            { adjust(); return FOR; }
[0-9]+         { adjust(); yylval.ival = atoi(yytext); return INT; }
.              { adjust(); EM_error(EM_tokPos, "illegal token"); }
