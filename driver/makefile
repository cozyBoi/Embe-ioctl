CC=arm-none-linux-gnueabi-gcc
.SUFFIXES: .c .h .o

DEPS= fpga_dot_font.h #input_process.h output_process.h
CFLAGS= #-W

TARGET = 20171677_hw2
OBJDIR = build/

_OBJS = main.o #input.o output.o
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
