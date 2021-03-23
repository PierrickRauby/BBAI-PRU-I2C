user_space : user_space.o
	gcc -o user_space user_space.o
	rm -rf *.o
user_space.o : user_space.c
	gcc -o user_space.o -c user_space.c 

clean:
	rm -rf *.o
	rm -rf user_space
