CC=gcc
CFLAGS= -Wall -g
TARGET= w4118_sh
SRC= $(TARGET).c

$(TARGET): $(SRC)
	$(CC) -o $@ $< $(CFLAGS)

.phony: clean run gdb edit
run: $(TARGET)
	./$<

clean: 
	@rm -vf $(TARGET) *.o
 
gdb: $(TARGET)
	gdb $<

edit: 
	vi $(SRC) 
