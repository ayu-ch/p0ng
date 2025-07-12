CC = gcc
CFLAGS = -Wall -Wextra -lncurses

SRCS = pong.c host.c joiner.c shared.c
OBJS = $(SRCS:.c=.o)

TARGET = pong

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
