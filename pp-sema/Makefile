all : ppsema

ppsema.tab.c ppsema.tab.h : ppsema.y
	bison -d ppsema.y
	
ppsema.lex.yy.c : ppsema.l ppsema.tab.h
	flex -o ppsema.lex.yy.c ppsema.l
	
ppsema : ppsema.lex.yy.c ppsema.tab.c ppsema.tab.h ppsema.c
	gcc -o ppsema ppsema.tab.c ppsema.lex.yy.c ppsema.c -lfl

ppsema.c:;
	
clean:
	rm ppsema *.tab.c *.lex.yy.c *.tab.h