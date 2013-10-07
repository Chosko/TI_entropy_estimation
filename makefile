compile:
	gcc -o entropia entropia.c

clean:
	rm entropia

recompile:
	rm entropia
	gcc -o entropia entropia.c

exec:
	./entropia entropia.c
