gcc -c -o main.o main.c -g
gcc -c -o linkedlist.o linkedlist.c -g
gcc -c -o readSetFile.o readSetFile.c -g


gcc -o ai readSetFile.o linkedlist.o main.o 