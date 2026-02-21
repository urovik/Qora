all:
	gcc server.c main.c core.c logger.c sql_parser.c utils.c  -o my