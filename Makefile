all:
	gcc src/server.c src/main.c src/core.c src/logger.c src/sql_parser.c src/utils.c  -o my