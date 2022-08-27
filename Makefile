all: pesquisa.o validacao.o
	gcc pesquisa.o validacao.o main.c -o main
	
pesquisa.o: pesquisa.h
	gcc -c pesquisa.c
	
validacao.o: validacao.h
	gcc -c validacao.c

clean:
	rm -rf *.o main
