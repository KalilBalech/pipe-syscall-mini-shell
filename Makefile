all: laboratorioCompiladinho
	./laboratorioCompiladinho

laboratorioCompiladinho: laboratorio-2.c
	gcc ./laboratorio-2.c -o laboratorioCompiladinho

clean:
	rm -f laboratorioCompiladinho
