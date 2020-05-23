CC=gcc
.SUFFIXES: .c .h .o

#DEPS= main.h #input_process.h output_process.h
CFLAGS= #-W

TARGET = app
OBJDIR = build/

_OBJS = app.o #input.o output.o
OBJS = $(addprefix $(OBJDIR),$(_OBJS))

$(OBJDIR)$(TARGET): $(OBJS)
	$(CC) -static -o $(OBJDIR)$(TARGET) $(OBJS)

$(OBJDIR)%.o: %.c $(OBJDIR)
	$(CC) -static -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

.PHONY: push
push:
	make
	adb push $(OBJDIR)$(TARGET) /data/local/tmp
