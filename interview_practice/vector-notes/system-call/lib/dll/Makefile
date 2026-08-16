CC = cc
CFLAGS = -c -fPIC

OBJS = sum.o sub.o print.o
LIB = mylib.so

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) -shared -o $(LIB) $(OBJS)

sum.o: sum.c
	$(CC) $(CFLAGS) sum.c 

sub.o: sub.c
	$(CC) $(CFLAGS) sub.c

print.o: print.c
	$(CC) $(CFLAGS) print.c

clean:
	rm -rf $(LIB) $(OBJS)
