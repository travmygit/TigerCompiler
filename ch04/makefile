# variable
OBJECTS = lex.yy.o y.tab.o errormsg.o util.o

# executable
all: lextest parsetest
.PHONY: all

lextest: lextest.o $(OBJECTS)
	cc -g -o $@ lextest.o $(OBJECTS)

parsetest: parsetest.o $(OBJECTS)
	cc -g -o $@ parsetest.o $(OBJECTS)

# objects
errormsg.o: errormsg.c errormsg.h util.h
util.o: util.c util.h

# lex
lextest.o: lextest.c y.tab.h errormsg.h util.h
lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
lex.yy.c: tiger.lex
	lex tiger.lex

# parse
parsetest.o: parsetest.c errormsg.h util.h
y.tab.o: y.tab.c
y.tab.c: tiger.y
	yacc -dv tiger.y
y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

.PHONY: clean
clean:
	rm -f *.o a.out lextest parsetest lex.yy.c y.output y.tab.c y.tab.h
