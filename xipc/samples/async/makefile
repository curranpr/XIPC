CC = /usr/bin/gcc
CFLAGS = -I../../include  
LDFLAGS = -s -L../../lib -lxipc -lnxipctcp
RM = /bin/rm

all: demasync demo_aio

demasync:	demasync.o
		$(CC) -o $@ demasync.o  $(LDFLAGS)

demo_aio:	demo_aio.o
		$(CC) -o $@ demo_aio.o  $(LDFLAGS)

clean:
		$(RM) -f demasync demo_aio *.o

new: clean all 


