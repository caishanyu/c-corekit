CC 		:= gcc
CFLAGS 	:= -std=c11 -pedantic -lpthread

atomic_counter : atomic_counter.c
	$(CC) atomic_counter.c $(CFLAGS) -o atomic_counter

clean:
	rm -rf atomic_counter