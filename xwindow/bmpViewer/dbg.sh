gcc -g -c -o bmpViewer.o bmpViewer.c
gcc -g -c -o bmpinfo.o bmpinfo.c

gcc -o bmp bmpViewer.o bmpinfo.o -lX11
