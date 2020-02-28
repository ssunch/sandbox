gcc -c -o main.o main.c -g
gcc -c -o linkedlist.o linkedlist.c -g
gcc -o ai linkedlist.o main.o 