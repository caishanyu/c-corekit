CC 		:= gcc
CFLAGS 	:= -std=c11 -pedantic -lpthread
INC		:= -Icommon -Iatomic

main : atomic/atomic_counter.c atomic/atomic_spin_lock.c main.c
	$(CC) atomic/atomic_counter.c atomic/atomic_spin_lock.c  main.c $(CFLAGS) $(INC) -o bin/main

clean:
	rm -rf bin/*