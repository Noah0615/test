CFLAGS= -Wall -g
OBJS = sort.o main.o add.o
all: main

%.o: %.c
	gcc -c -o $@ $(CFLAGS) $<

main: $(OBJS)
	gcc -o main $(OBJS)

clean:
	rm -rf main $(OBJS)
