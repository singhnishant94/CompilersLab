all: clean lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
#	./sedscript
	g++   --std=c++0x lex.cc parse.cc main.cc;
	./a.out < ./test-cpp > ./code.asm;

exe: code.asm
	cp ./code.asm ./Machine/code.asm;
	cd Machine && \
	make && \
	cp ./a.out ../exe
#	egrep 'nextToken|reduce' junk;
#	dot -Tps graph.gv -o graph.ps;
#	gnome-open graph.ps
        

lex.cc: lex.l Scanner.ih 
	./cond_remove_scannerih.sh;
	flexc++ lex.l;
#	sed -i '/int lex()/r enums' Scanner.h

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	./cond_remove_parserh.sh;
	bisonc++ parse.y;
#	sed -i '/insert preincludes/a #include "headers.cc"' Parserbase.h;
#	sed -i '/include "Parser.h"/a #include "definitions.cc"' Parser.ih;	
	
	sed -i '/include/a using namespace std;' Parser.h
	sed -i '/int parse()/r ParsehFile' Parser.h
#	sed -i '/$insert tokens/a static int counterme;' Parserbase.h;
#	bisonc++   --construction -V parse.y; 
#	sed -i '/ifndef/a #include "tree_util.hh"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
	./sedscript
     

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -rf lex.cc Scanner.h Scanner.ih Parser.h Parser.ih parse.cc
