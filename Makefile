GCC=clang
GXX=clang++ -std=c++11
DEBUG=-g

lex.yy.c: lang.l
	flex lang.l

lang.tab.cc: lang.yy
	bison lang.yy

lexer: lex.yy.c lang.tab.cc
	$(GXX) $(DEBUG) -D_LEXER_STANDALONE_ -o lexer lex.yy.c

parser: lang.tab.cc lex.yy.c ast.o
	$(GXX) $(DEBUG) -D_PARSER_STANDALONE_ -DYYDEBUG=1 -o parser lang.tab.cc lex.yy.c ast.o

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) $(DEBUG) -c -o $@ $<

clean:
	rm -f *.o lex.yy.* lang.tab.* lexer parser

