all : ppc3ac

ppc3ac.tab.c ppc3ac.tab.h : ppc3ac.y
	bison -d ppc3ac.y
	
ppc3ac.lex.yy.c : ppc3ac.l ppc3ac.tab.h
	flex -o ppc3ac.lex.yy.c ppc3ac.l
	
ppc3ac : ppc3ac.lex.yy.c ppc3ac.tab.c ppc3ac.tab.h ppc3ac.c
	gcc -o ppc3ac -g ppc3ac.tab.c ppc3ac.lex.yy.c ppc3ac.c -lfl

ppc3ac.c:;
	
clean:
	rm ppc3ac *.tab.c *.lex.yy.c *.tab.h