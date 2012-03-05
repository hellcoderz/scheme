OBJS = eval.o  mem.o reader.o fixnum.o charcache.o \
	write.o main.o repl.o init_obj.o flonum.o \
	boolean.o character.o sstream.o string.o \
	list.o symbol.o hashtbl.o sform.o env.o \
	primitive.o procdef.o compound.o eof.o \
	port.o ioproc.o gc.o stack.o intcache.o \
	mathproc.o strproc.o

CFLAG = -Wall -c
ifeq ($(DEBUG), 1)
	CFLAG += -g
else
	CFLAG += -O2
endif

scheme: $(OBJS)
	cc $^ -lm -o scheme

%.o: %.c
	cc $(CFLAG) $< -o $@

.PHONY: clean
clean:
	rm *.o scheme
