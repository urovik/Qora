CC = gcc
CFLAGS = -Wall -Wextra -I./src -I./src/core -I./src/server -I./src/parser -I./src/utils 
SRCS = src/main.c \
       src/server/server.c \
       src/core/core.c \
       src/core/logger.c \
       src/parser/sql_parser.c \
       src/core/utils.c \
       src/server/handlers/client_handlers.c
OBJS = $(SRCS:.c=.o)
TARGET = my

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
