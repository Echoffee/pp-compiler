CC=gcc
#GNU99 pour l'usage de snprintf, strdup et retrait de certains warnings de c99.
CFLAGS:= -std=gnu99
LDFLAGS = -lfl
LEX = flex
all: c3ai

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

c3ai.yy.c: c3ai.l
	$(LEX) -o $@ $<

c3ai: c3ai.yy.c bilquad.o environ.o
	$(CC) -g $(CFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	rm c3ai.yy.c bilquad.o environ.o c3ai
