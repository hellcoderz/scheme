OBJS = sc_eval.o sc_log.o sc_mem.o sc_reader.o sc_fixnum.o
OBJS += sc_print.o sc_print.o sc_main.o sc_repl.o
OBJS += sc_init_obj.o sc_boolean.o sc_character.o
OBJS += sc_sstream.o sc_string.o sc_list.o sc_symbol.o sc_symtbl.o
OBJS += sc_sform.o

scheme: $(OBJS)
	cc $^ -o scheme

%.o: %.c
	cc -Wall -c -O2 $< -o $@

.PHONY: clean
clean:
	rm *.o scheme
