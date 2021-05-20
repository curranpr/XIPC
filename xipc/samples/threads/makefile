RM=/bin/rm
CC=/usr/bin/gcc
SYSINC=	-I/usr/include
CFLAGS=	-c -g $(SYSINC) -I../../include
COMLIB= -L../../lib/threads -lxipc_r -lnxipctcp_r -lpthread

.PRECIOUS:
.IGNORE:

.c.o:
	$(CC) $(CFLAGS) $% $<

all:	progs 

PROGRAMS =	qdemo aqdemo

progs: $(PROGRAMS)

qdemo:	qdemo.o   
		$(CC) qdemo.o -o qdemo $(COMLIB) 
		@echo "*** $(@F) up to date ***"

aqdemo:	aqdemo.o   
		$(CC) aqdemo.o -o aqdemo $(COMLIB) 
		@echo "*** $(@F) up to date ***"

touch:
	/bin/touch $(CFILES)

clean:
	$(RM) -f nohup.out core  *.o $(PROGRAMS)

new:	clean all
