CC = gcc
CFLAGS = -Wall

all : filerequester multiserver

filerequester : filerequester.c
	$(CC) $(CFLAGS) -o filerequester filerequester.c -lpthread

multiserver : multiserver.c
	$(CC) $(CFLAGS) -o multiserver multiserver.c -lpthread

clean :
	$(RM) filerequester multiserver *.a *.o *.so *~
