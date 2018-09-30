# variable
OBJECTS = lex.yy.o y.tab.o errormsg.o util.o absyn.o prabsyn.o symbol.o \
          table.o parse.o env.o semant.o types.o mipsframe.o temp.o \
		  translate.o escape.o tree.o printtree.o

# executable
all: lextest parsetest absyntest typechecktest translatetest
.PHONY: all

lextest: lextest.o $(OBJECTS)
	cc -g -o $@ lextest.o $(OBJECTS)

parsetest: parsetest.o $(OBJECTS)
	cc -g -o $@ parsetest.o $(OBJECTS)

absyntest: absyntest.o $(OBJECTS)
	cc -g -o $@ absyntest.o $(OBJECTS)

typechecktest: typechecktest.o $(OBJECTS)
	cc -o $@ typechecktest.o $(OBJECTS)

translatetest: translatetest.o $(OBJECTS)
	cc -o $@ translatetest.o $(OBJECTS)

# objects
errormsg.o: errormsg.c errormsg.h util.h
util.o: util.c util.h
absyn.o: absyn.c absyn.h util.h symbol.h
prabsyn.o: prabsyn.c prabsyn.h util.h absyn.h
symbol.o: symbol.c symbol.h util.h table.h
table.o: table.c table.h util.h
parse.o: parse.c parse.h util.h errormsg.h symbol.h absyn.h

# lex
lextest.o: lextest.c absyn.h symbol.h y.tab.h errormsg.h util.h
lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
lex.yy.c: tiger.lex
	lex tiger.lex

# parse
parsetest.o: parsetest.c errormsg.h util.h absyn.h symbol.h
y.tab.o: y.tab.c
y.tab.c: tiger.y
	yacc -dv tiger.y
y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

# absyn
absyntest.o: absyntest.c errormsg.h util.h absyn.h symbol.h parse.h prabsyn.h

# type-check
typechecktest.o: typechecktest.c errormsg.h util.h absyn.h symbol.h
env.o: env.c env.h
semant.o: semant.c semant.h
types.o: types.c types.h

# frame
mipsframe.o: frame.h
temp.o: temp.h
escape.o: escape.h

# translate
translate.o: translate.h
tree.o: tree.h
printtree.o: printtree.h
translatetest.o: translatetest.c errormsg.h util.h absyn.h symbol.h frame.h

.PHONY: clean
clean:
	rm -f *.o a.out lextest parsetest absyntest typechecktest translatetest lex.yy.c y.output y.tab.c y.tab.h
