OBJECTS=main.o
CC=gcc
ARGS=-Wall
TARGET=testprogram

$(TARGET): $(OBJECTS)
	$(CC) $(ARGS) -o $(TARGET) $(OBJECTS)

main.o: main.c
	$(CC) $(ARGS) -c -o main.o main.c

clean:
	rm -f *.o

cleanall:
	rm -f *.o $(TARGET)
