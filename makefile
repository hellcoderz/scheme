OBJS = sc_eval.o sc_log.o sc_mem.o sc_reader.o sc_fixnum.o
OBJS += sc_print.o sc_print.o sc_main.o sc_repl.o
OBJS += sc_init_obj.o sc_boolean.o

scheme: $(OBJS)
	cc $^ -o scheme

%.o: %.c
	cc -Wall -ansi -c -O2 $< -o $@

.PHONY: clean
clean:
	rm *.o scheme
