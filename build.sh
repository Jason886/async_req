gcc -I. -std=c99 -pedantic -Wall -Wextra -c async_task.c -o async_task.o
gcc -I. -std=c99 test.c async_task.o -o test -lpthread
