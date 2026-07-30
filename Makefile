CC = gcc
CFLAGS = -g -Wall -Wextra -D_POSIX_C_SOURCE=200809L -I./src -I./src/core -I./src/server -I./src/parser -I./src/utils -I./src/wrapper -I./src/db
LDFLAGS = -lrt
SRCS = src/main.c \
       src/server/server.c \
       src/core/core.c \
       src/core/logger.c \
       src/parser/lexer.c \
       src/parser/ast.c \
       src/parser/executor.c \
       src/parser/parser.c \
       src/core/utils.c \
       src/core/qoraLoop.c \
       src/core/timers.c \
       src/core/callback.c \
       src/core/memory.c \
       src/core/qNetwork.c \
       src/wrapper/wrappers.c \
       src/db/dict.c \
       src/db/qoraDb.c 

OBJS = $(SRCS:.c=.o)
TARGET = my

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
