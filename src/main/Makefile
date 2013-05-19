PKGFLAG	=
EXTHDRS	= 

include ../Makefile.in

BINDIR	= ../../bin
TARGET	= $(BINDIR)/$(EXEC)

target: $(TARGET)

$(TARGET): $(COBJS) $(LIBDEP)
	@echo "> building $(EXEC)..."
	@$(CC) $(CFLAGS) -I$(INCDIR) $(COBJS) -L$(LIBDIR) $(INCLIB) -o $@
