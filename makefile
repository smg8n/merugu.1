CC = gcc
CFLAGS = -I. -g
TARGET = dt
OBJS = main.o Arguments.o Print.o
.SUFFIXES: .c .o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)


.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
