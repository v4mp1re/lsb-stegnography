SRC:= $(wildcard *.c)
OBJ:= $(patsubst %.c,%.o,$(SRC))
Target: $(OBJ)
	gcc -o $@ $(OBJ) -Wall
clean:
	rm *.o lsb_steg
