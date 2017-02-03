GCC=clang
GXX=clang++ -std=c++11
DEBUG=-g

lex.yy.c: lang.l
	flex lang.l

lang.tab.cc: lang.yy
	bison lang.yy

lexer: lex.yy.c lang.tab.cc
	$(GXX) $(DEBUG) -D_LEXER_STANDALONE_ -o lexer lex.yy.c

Lex: lex.yy.c lang.tab.cc Lexer.o Lex.o
	$(GXX) $(DEBUG) -o Lex lex.yy.c Lexer.o Lex.o

parser: lang.tab.cc lex.yy.c ast.o
	$(GXX) $(DEBUG) -D_PARSER_STANDALONE_ -DYYDEBUG=1 -o parser lang.tab.cc lex.yy.c ast.o

Parse: lang.tab.cc lex.yy.c Parse.o Lexer.o ast.o
	$(GXX) $(DEBUG) -o Parse lang.tab.cc lex.yy.c Parse.o Lexer.o ast.o

Compile: lang.tab.cc lex.yy.c Compile.o Lexer.o ast.o sema.o codegen.o
	$(GXX) $(DEBUG) -o Compile lang.tab.cc lex.yy.c Compile.o Lexer.o ast.o sema.o codegen.o

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) $(DEBUG) -c -o $@ $<

clean:
	rm -f *.o lex.yy.* lang.tab.* lexer parser

