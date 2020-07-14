gcc -c -o main.o main.c -g
gcc -c -o linkedlist.o linkedlist.c -g
gcc -c -o readSetFile.o readSetFile.c -g
gcc -c -o random.o random.c -g
gcc -c -o ann.o ann.c -g
gcc -c -o core.o core.c -g

gcc -o ai core.o ann.o random.o readSetFile.o linkedlist.o main.o -lm -lpthread