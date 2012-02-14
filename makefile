OBJS = sc_eval.o sc_log.o sc_mem.o sc_reader.o sc_fixnum.o \
	sc_print.o sc_print.o sc_main.o sc_repl.o sc_init_obj.o \
	sc_boolean.o sc_character.o sc_sstream.o sc_string.o \
	sc_list.o sc_symbol.o sc_hashtbl.o sc_sform.o sc_env.o \
	sc_primitive.o sc_procdef.o sc_compound.o

CFLAG = -Wall -c -O2
ifeq ($(DEBUG), 1)
	CFLAG += -g
endif

scheme: $(OBJS)
	cc $^ -o scheme

%.o: %.c
	cc $(CFLAG) $< -o $@

.PHONY: clean
clean:
	rm *.o scheme
