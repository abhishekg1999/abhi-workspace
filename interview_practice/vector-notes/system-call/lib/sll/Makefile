CC = gcc
CFLAGS = -c 
AR = ar
ARFLAGS = -rcs

OBJ = sum.o sub.o print.o
LIB = mylib.a

all:$(LIB)
	
$(LIB):$(OBJ)
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)


sum.o: sum.c
	$(CC) $(CFLAGS) sum.c

sub.o: sub.c
	$(CC) $(CFLAGS) sub.c

print.o: print.c
	$(CC) $(CFLAGS) print.c

clean:
	rm -rf $(OBJ) $(LIB)
