all:    lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	a.out < test > junk
#	egrep 'nextToken|processed|reduce' junk
        

lex.cc: lex.l Scanner.ih
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih;

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  --construction parse.y; 
	./sedscript
     
clean:
	rm -f parse.cc Parser.ih Parser.h
Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
