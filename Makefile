GCC=clang
GXX=clang++ -v
DEBUG=-g
PARSER_TRACE=1


lex.yy.cc: lang.l
	flex lang.l

lang.tab.cc: lang.yy
	bison -Dparse.trace --debug -t lang.yy

lang.tab.hh: lang.tab.cc

lexer: lex.yy.cc lang.tab.cc lang.tab.hh ast.o
	$(GXX) $(DEBUG) -std=c++11 -D_LEXER_STANDALONE_ -o lexer lex.yy.cc lang.tab.cc ast.o

parser: lang.tab.cc lang.tab.hh lex.yy.cc ast.o
	$(GXX) -std=c++11 $(DEBUG) -D_PARSER_STANDALONE_ -DYYDEBUG=$(PARSER_TRACE) -o parser lang.tab.cc lex.yy.cc ast.o

%.o: %.c
	$(GCC) $(DEBUG) -c -o $@ $<

%.o: %.cpp
	$(GXX) -std=c++11 $(DEBUG) -c -o $@ $<

clean:
	rm -f *.o
	rm -f lex.yy.*
	rm -f lang.tab.* stack.hh
	rm -f lexer parser

