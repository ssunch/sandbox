gcc -g -c -o viewer.o viewer.c
gcc -g -c -o bmp.o bmpprocess.c
gcc -g -c -o jpg.o jpgprocess.c
gcc -g -c -o util.o util.c

gcc -o imgViewer viewer.o bmp.o jpg.o util.o -lX11 -ljpeg
