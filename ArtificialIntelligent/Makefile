CC=gcc
CFLAGS=-g -Wall
TARGET=ai
LDFLAGS=-lm -lpthread
OBJS=main.o linkedlist.o readSetFile.o random.o ann.o core.o
 
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)
