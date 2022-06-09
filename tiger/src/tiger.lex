%{
#include <string.h>
#include <stdlib.h> // @see https://stackoverflow.com/questions/24704468/malloc-returning-address-that-i-cannot-access
#include "util.h"
#include "y.tab.h"
#include "errormsg.h"

// TODO:
//   handle comments
//   handle strings
//   error handling
//   EOF handling
//   Detect unclosed comments (at end of file) and unclosed strings
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

#define STRING_BUFFER_SIZE 4096
char string_buffer[STRING_BUFFER_SIZE];
int  string_buffer_index;

void yy_extend_string(char c)
{
	if (string_buffer_index < STRING_BUFFER_SIZE)
	{
		string_buffer[string_buffer_index++] = c;
	}
	else
	{
		EM_error(EM_tokPos, "string too long (> 4095)");
	}
}

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
%x COMMENT STRINGS
%%
[ \t]+                 { adjust(); continue; }
\n                     { adjust(); EM_newline(); continue; }
","                    { adjust(); return COMMA; }
":"                    { adjust(); return COLON; }
";"                    { adjust(); return SEMICOLON; }
"("                    { adjust(); return LPAREN; }
")"                    { adjust(); return RPAREN; }
"["                    { adjust(); return LBRACK; }
"]"                    { adjust(); return RBRACK; }
"{"                    { adjust(); return LBRACE; }
"}"                    { adjust(); return RBRACE; }
"."                    { adjust(); return DOT; }
"+"                    { adjust(); return PLUS; }
"-"                    { adjust(); return MINUS; }
"*"                    { adjust(); return TIMES; }
"/"                    { adjust(); return DIVIDE; }
"="                    { adjust(); return EQ; }
"<>"                   { adjust(); return NEQ; }
"<"                    { adjust(); return LT; }
"<="                   { adjust(); return LE; }
">"                    { adjust(); return GT; }
">="                   { adjust(); return GE; }
"&"                    { adjust(); return AND; }
"|"                    { adjust(); return OR; }
":="                   { adjust(); return ASSIGN; }
array                  { adjust(); return ARRAY; }
if                     { adjust(); return IF; }
then                   { adjust(); return THEN; }
else                   { adjust(); return ELSE; }
while                  { adjust(); return WHILE; }
for                    { adjust(); return FOR; }
to                     { adjust(); return TO; }
do                     { adjust(); return DO; }
let                    { adjust(); return LET; }
in                     { adjust(); return IN; }
end                    { adjust(); return END; }
of                     { adjust(); return OF; }
break                  { adjust(); return BREAK; }
nil                    { adjust(); return NIL; }
function               { adjust(); return FUNCTION; }
var                    { adjust(); return VAR; }
type                   { adjust(); return TYPE; }
[a-zA-Z][a-zA-Z0-9_]*  { adjust(); yylval.sval = String(yytext); return ID; }
[0-9]+                 { adjust(); yylval.ival = atoi(yytext); return INT; }
"/*"                   { adjust(); BEGIN(COMMENT); }
\"                     { adjust(); string_buffer_index = 0; BEGIN(STRINGS); }
.                      { adjust(); EM_error(EM_tokPos, "illegal token"); yyterminate(); }
<COMMENT>[^*\n]*                { adjust(); continue; }
<COMMENT>"*"+[^*/\n]*           { adjust(); continue; }
<COMMENT>\n                     { adjust(); EM_newline(); continue; }
<COMMENT>"*"+"/"                { adjust(); BEGIN(INITIAL); }
<COMMENT><<EOF>>                { EM_error(EM_tokPos, "unclosed comment"); yyterminate(); }
<STRINGS>\"                     { adjust(); yy_extend_string('\0'); yylval.sval = String(string_buffer); BEGIN(INITIAL); return STRING; }
<STRINGS>\n                     { adjust(); EM_error(EM_tokPos, "illegal newline in literal string"); yyterminate(); }
<STRINGS>\\[0-7]{1,3}           { adjust(); int result; sscanf(yytext + 1, "%o", &result); if (result > 0xff) { EM_error(EM_tokPos, "illegal octal escape sequence in literal string"); yyterminate(); } yy_extend_string(result); }
<STRINGS>\\[0-9]+               { adjust(); EM_error(EM_tokPos, "illegal octal escape sequence in literal string"); yyterminate(); }
<STRINGS>\\n                    { adjust(); yy_extend_string('\n'); }
<STRINGS>\\t                    { adjust(); yy_extend_string('\t'); }
<STRINGS>\\r                    { adjust(); yy_extend_string('\r'); }
<STRINGS>\\b                    { adjust(); yy_extend_string('\b'); }
<STRINGS>\\f                    { adjust(); yy_extend_string('\f'); }
<STRINGS>\\(.|\n)               { adjust(); yy_extend_string(yytext[1]); }
<STRINGS>[^\\\n\"]+             { adjust(); char* yptr = yytext; while (*yptr) yy_extend_string(*yptr++); }
<STRINGS><<EOF>>                { EM_error(EM_tokPos, "unclosed string"); yyterminate(); }
