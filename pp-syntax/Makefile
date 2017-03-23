all : ppsyna

ppsyna.tab.c ppsyna.tab.h : ppsyna.y
	bison -d ppsyna.y
	
ppsyna.lex.yy.c : ppsyna.l ppsyna.tab.h
	flex -o ppsyna.lex.yy.c ppsyna.l
	
ppsyna : ppsyna.lex.yy.c ppsyna.tab.c ppsyna.tab.h ppsyna.c
	gcc -o ppsyna ppsyna.tab.c ppsyna.lex.yy.c ppsyna.c -lfl

ppsyna.c:;
	
clean:
	rm ppsyna *.tab.c *.lex.yy.c *.tab.h