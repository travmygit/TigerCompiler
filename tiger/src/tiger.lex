%{
#include <string.h>
#include <stdlib.h> // @see https://stackoverflow.com/questions/24704468/malloc-returning-address-that-i-cannot-access
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

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
