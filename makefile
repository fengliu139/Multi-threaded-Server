CC = gcc
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread

all: web_server_http

util.o: util.c util.h
	${CC} -c -o util.o util.c

web_server_http: server.o util.o
	${CC} ${LDFLAGS} -o web_server_http server.o util.o

server.o: server.c

clean:
	rm *.o web_server_http 

