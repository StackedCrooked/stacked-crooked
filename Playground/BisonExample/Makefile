# Makefile

FILES	= Lexer.c Parser.c Expression.c main.c
CC	= g++
CFLAGS	= -g -ansi -O0 -fno-inline

test:		$(FILES)
		$(CC) $(CFLAGS) $(FILES) -o test

Lexer.c:	Lexer.l 
		flex Lexer.l

Parser.c:	Parser.y Lexer.c
		PATH="/usr/local/opt/bison/bin:$PATH" bison Parser.y

clean:
		rm -f *.o *~ Lexer.c Lexer.h Parser.c Parser.h test
