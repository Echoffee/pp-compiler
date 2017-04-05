all : ppina

ppina.tab.c ppina.tab.h : ppina.y
	bison -d ppina.y
	
ppina.lex.yy.c : ppina.l ppina.tab.h
	flex -o ppina.lex.yy.c ppina.l
	
ppina : ppina.lex.yy.c ppina.tab.c ppina.tab.h ppina.c
	gcc -o ppina ppina.tab.c ppina.lex.yy.c ppina.c -lfl

ppina.c:;
	
clean:
	rm ppina *.tab.c *.lex.yy.c *.tab.h