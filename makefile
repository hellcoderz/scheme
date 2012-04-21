OBJS = eval.o  mem.o reader.o fixnum.o charcache.o \
	write.o main.o repl.o initobj.o flonum.o \
	boolean.o character.o sstream.o string.o \
	list.o symbol.o hashtbl.o sform.o env.o \
	primitive.o procdef.o compound.o eof.o \
	port.o ioproc.o gc.o stack.o intcache.o \
	mathproc.o strproc.o objstream.o vector.o \
	vecproc.o frame.o cont.o macro.o

CFLAG = -Wall -c
LFLAG = -lm -lrt
ifeq ($(DEBUG), 1)
	CFLAG += -g
else
	CFLAG += -O3
endif

ifeq ($(PROFILE), 1)
	CFLAG += -pg
	LFLAG += -pg
endif

asc: $(OBJS)
	cc $^ $(LFLAG) -o asc

%.o: %.c
	cc $(CFLAG) $< -o $@

INSTALL_BIN = /usr/bin
INSTALL_TOP = /usr/share/asc
INSTALL_LIB = $(INSTALL_TOP)/lib
MKDIR = mkdir -p
INSTALL = install -p
INSTALL_EXEC = $(INSTALL) -m 0755
INSTALL_DATA = $(INSTALL) -m 0644
UNINSTALL = rm -f
UNINSTALL_DIR = $(UNINSTALL) -r
BIN = asc
LIB = lib/core.scm
install:
	$(MKDIR) $(INSTALL_BIN) $(INSTALL_LIB)
	$(INSTALL) $(BIN) $(INSTALL_BIN)
	$(INSTALL) $(LIB) $(INSTALL_LIB)

uninstall:
	cd $(INSTALL_BIN) && $(UNINSTALL) $(BIN)
	$(UNINSTALL_DIR) $(INSTALL_TOP)

RM = rm -f
.PHONY: clean
clean:
	$(RM) *.o asc
