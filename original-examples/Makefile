all: uaf ptr-0 ptr-1 ptr-2 ptr-3 ptr-5 ptr-6 ptr-7 ptr-8 ptr-9 expl-1

uaf: uaf.c
	gcc uaf.c -o uaf

ptr-0: ptr-0.c
	gcc ptr-0.c -lpthread -o ptr-0

ptr-1: ptr-1.c
	gcc ptr-1.c -o ptr-1

ptr-2: ptr-2.c
	gcc ptr-2.c -lpthread -o ptr-2

ptr-3: ptr-3.c
	gcc ptr-3.c -o ptr-3

ptr-5: ptr-5.c
	gcc ptr-5.c -o ptr-5

ptr-6: ptr-6.c
	gcc ptr-6.c -o ptr-6

ptr-7: ptr-7.c
	gcc ptr-7.c -o ptr-7

ptr-8: ptr-8.c
	gcc ptr-8.c -o ptr-8

ptr-9: ptr-9.c
	gcc ptr-9.c -o ptr-9

expl-1: expl-1.c
	gcc expl-1.c -o expl-1

not-fooled: ptr-9
	MALLOC_CHECK_=1
	ptr-9

fooled: ptr-9
	MALLOC_CHECK_=0
	ptr-9

clean:
	rm -f uaf ptr-? expl-1

rmall:
	rmall uaf 'ptr-?' expl-1
