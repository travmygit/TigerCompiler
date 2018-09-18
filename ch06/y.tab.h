/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LOW = 258,
     FUNCTION = 259,
     TYPE = 260,
     ID = 261,
     LBRACK = 262,
     OF = 263,
     DO = 264,
     THEN = 265,
     ELSE = 266,
     SEMICOLON = 267,
     ASSIGN = 268,
     OR = 269,
     AND = 270,
     GE = 271,
     GT = 272,
     LE = 273,
     LT = 274,
     NEQ = 275,
     EQ = 276,
     MINUS = 277,
     PLUS = 278,
     DIVIDE = 279,
     TIMES = 280,
     UMINUS = 281,
     STRING = 282,
     INT = 283,
     COMMA = 284,
     COLON = 285,
     LPAREN = 286,
     RPAREN = 287,
     RBRACK = 288,
     LBRACE = 289,
     RBRACE = 290,
     DOT = 291,
     ARRAY = 292,
     IF = 293,
     WHILE = 294,
     FOR = 295,
     TO = 296,
     LET = 297,
     IN = 298,
     END = 299,
     BREAK = 300,
     NIL = 301,
     VAR = 302
   };
#endif
/* Tokens.  */
#define LOW 258
#define FUNCTION 259
#define TYPE 260
#define ID 261
#define LBRACK 262
#define OF 263
#define DO 264
#define THEN 265
#define ELSE 266
#define SEMICOLON 267
#define ASSIGN 268
#define OR 269
#define AND 270
#define GE 271
#define GT 272
#define LE 273
#define LT 274
#define NEQ 275
#define EQ 276
#define MINUS 277
#define PLUS 278
#define DIVIDE 279
#define TIMES 280
#define UMINUS 281
#define STRING 282
#define INT 283
#define COMMA 284
#define COLON 285
#define LPAREN 286
#define RPAREN 287
#define RBRACK 288
#define LBRACE 289
#define RBRACE 290
#define DOT 291
#define ARRAY 292
#define IF 293
#define WHILE 294
#define FOR 295
#define TO 296
#define LET 297
#define IN 298
#define END 299
#define BREAK 300
#define NIL 301
#define VAR 302




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 18 "tiger.y"
{
	int pos;
	int ival;
	string sval;
  A_var var;
  A_exp exp;
  A_dec dec;
  A_ty ty;
  A_decList declist;
  A_expList explist;
  A_field field;
  A_fieldList fieldlist;
  A_fundec fundec;
  A_fundecList fundeclist;
  A_namety namety;
  A_nametyList nametylist;
  A_efield efield;
  A_efieldList efieldlist;
  }
/* Line 1529 of yacc.c.  */
#line 163 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

