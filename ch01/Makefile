a.out: main.o prog.o slp.o util.o maxargs.o interp.o
	cc -g main.o prog.o slp.o util.o maxargs.o interp.o

main.o: main.c slp.h util.h
	cc -g -c main.c

prog.o: prog.c slp.h util.h
	cc -g -c prog.c

slp.o: slp.c slp.h util.h
	cc -g -c slp.c

util.o: util.c util.h
	cc -g -c util.c

interp.o: interp.c interp.h
	cc -g -c interp.c

maxargs.o: maxargs.c maxargs.h
	cc -g -c maxargs.c

clean:
	rm -f util.o prog.o slp.o main.o maxargs.o interp.o
