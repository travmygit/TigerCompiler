#include "tokens.h"

string toknames[] =
{
	"ID",
	"STRING",
	"INT",
	"COMMA",
	"COLON",
	"SEMICOLON",
	"LPAREN",
	"RPAREN",
	"LBRACK",
	"RBRACK",
	"LBRACE",
	"RBRACE",
	"DOT",
	"PLUS",
	"MINUS",
	"TIMES",
	"DIVIDE",
	"EQ",
	"NEQ",
	"LT",
	"LE",
	"GT",
	"GE",
	"AND",
	"OR",
	"ASSIGN",
	"ARRAY",
	"IF",
	"THEN",
	"ELSE",
	"WHILE",
	"FOR",
	"TO",
	"DO",
	"LET",
	"IN",
	"END",
	"OF",
	"BREAK",
	"NIL",
	"FUNCTION",
	"VAR",
	"TYPE"
};

string tokname(int tok) {
	return (tok < ID || tok > TYPE) ? "BAD_TOKEN" : toknames[tok - 257];
}