GCC=clang
GXX=clang++
DEBUG=-g

lex.yy.c: lang.l
	flex lang.l

lang.tab.c: lang.y
	bison lang.y

lexer: lex.yy.c
	$(GCC) $(DEBUG) -D_LEXER_STANDALONE_ -o lexer lex.yy.c

parser: lang.tab.c lex.yy.c ast_c.o ast.o
	$(GXX) -std=c++11 $(DEBUG) -D_PARSER_STANDALONE_ -DYYDEBUG=1 -o parser lang.tab.c lex.yy.c ast_c.o ast.o

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) -std=c++11 $(DEBUG) -c -o $@ $<

clean:
	rm -f *.o lex.yy.c lang.tab.c lang.tab.h lexer parser

