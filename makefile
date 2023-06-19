CFLAGS= -Wall -g
OBJS= main.o print.o search.o delete.o register.o 
all: main

%.o: %.c
	gcc -c -o $@ $(CFLAGS) $<
main: $(OBJS)
	gcc -o main $(OBJS)

clean:
	rm -rf main $(OBJS)

