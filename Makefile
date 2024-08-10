main: auxiliary.c child.c;
	gcc -o main main.c child.c auxiliary.c -lpthread;

clean:
	rm main;