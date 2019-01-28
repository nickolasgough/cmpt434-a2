# Nickolas Gough, nvg081, 11181823

GCC = gcc
FLAGS = -g -std=gnu90 -Wall -pedantic


all: sender-a receiver-a


sender-a: sender-a.o common.o
	$(GCC) -o $@ $(FLAGS) $^

sender-a.o: sender-a.c
	$(CC) -o $@ -c $(FLAGS) $<


receiver-a: receiver-a.o common.o
	$(GCC) -o $@ $(FLAGS) $^

receiver-a.o: receiver-a.c
	$(CC) -o $@ -c $(FLAGS) $<


common.o: common.c common.h
	$(CC) -o $@ -c $(FLAGS) $<


clean:
	rm -rf sender-a receiver-a *.o
