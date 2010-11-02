CC = gcc
CFLAGS = -Wall

GC = gc.c gc.h
TYPE = type.c type.h
IO = io.c io.h
LISP = lisp.c lisp.h
FSUBR = fsubr.c
OBJ = gc.o type.o io.o lisp.o fsubr.o test.o eval.o
PROG = komlisp
TEST = test.c

all: $(PROG)

$(PROG):$(OBJ)
	gcc -pg -Wall $(OBJ) -o$(PROG)

eval.o:eval.c type.h
test.o:$(TEST)
fsubr.o:fsubr.c type.h
lisp.o:$(LISP)
io.o:$(IO) type.h
gc.o:$(GC) type.h
type.o:$(TYPE)

clean:
	-rm $(OBJ) $(PROG)
	-@rm  *~
