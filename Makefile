CC = gcc
CFLAGS = -g -Wall -Wextra -I./src -I./src/core -I./src/server -I./src/parser -I./src/utils -I./src/wrapper
SRCS = src/main.c \
       src/server/server.c \
       src/core/core.c \
       src/core/logger.c \
       src/parser/sql_parser.c \
       src/parser/lexer.c \
       src/parser/ast.c \
       src/parser/executor.c \
       src/parser/parser.c \
       src/core/utils.c \
       src/core/qoraLoop.c \
       src/core/memory.c \
       src/core/qNetwork.c \
       src/wrapper/wrappers.c 
OBJS = $(SRCS:.c=.o)
TARGET = my

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
