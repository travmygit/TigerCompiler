# variable
OBJECTS = lex.yy.o y.tab.o errormsg.o util.o absyn.o prabsyn.o symbol.o \
          table.o parse.o env.o semant.o types.o

# executable
all: lextest parsetest absyntest typechecktest
.PHONY: all

lextest: lextest.o $(OBJECTS)
	cc -g -o $@ lextest.o $(OBJECTS)

parsetest: parsetest.o $(OBJECTS)
	cc -g -o $@ parsetest.o $(OBJECTS)

absyntest: absyntest.o $(OBJECTS)
	cc -g -o $@ absyntest.o $(OBJECTS)

typechecktest: typechecktest.o $(OBJECTS)
	cc -g -o $@ typechecktest.o $(OBJECTS)

# objects
errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c $<
util.o: util.c util.h
	cc -g -c $<
absyn.o: absyn.c absyn.h util.h symbol.h
	cc -g -c $<
prabsyn.o: prabsyn.c prabsyn.h util.h absyn.h
	cc -g -c $<
symbol.o: symbol.c symbol.h util.h table.h
	cc -g -c $<
table.o: table.c table.h util.h
	cc -g -c $<
parse.o: parse.c parse.h util.h errormsg.h symbol.h absyn.h
	cc -g -c $<

# lex
lextest.o: lextest.c absyn.h symbol.h y.tab.h errormsg.h util.h
	cc -g -c $<
lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c $<
lex.yy.c: tiger.lex
	lex tiger.lex

# parse
parsetest.o: parsetest.c errormsg.h util.h absyn.h symbol.h
	cc -g -c $<
y.tab.o: y.tab.c
	cc -g -c $<
y.tab.c: tiger.y
	yacc -dv tiger.y
y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

# absyn
absyntest.o: absyntest.c errormsg.h util.h absyn.h symbol.h parse.h prabsyn.h

# type-check
typechecktest.o: typechecktest.c errormsg.h util.h absyn.h symbol.h
	cc -g -c $<
env.o: env.c env.h
	cc -g -c $<
semant.o: semant.c semant.h
	cc -g -c $<
types.o: types.c types.h
	cc -g -c $<

.PHONY: clean
clean:
	rm -f *.o a.out lextest parsetest absyntest typechecktest lex.yy.c y.output y.tab.c y.tab.h
