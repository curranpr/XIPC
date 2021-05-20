CC=/usr/bin/gcc
CFLAGS	=	-I../../include -O

# One of following three definitions for LDFLAGS must be uncommented; other two must be commented.
# use following line for linking with combined (stand-alone and network capable) libraries
LDFLAGS =	-s -L../../lib -lxipc -lnxipctcp 
# use following line for linking with network (only) libraries
#LDFLAGS	=	-s -L../../lib -lnxipc -lnxipctcp 
# use following line for linking with stand-alone (no network capablities) libraries
#LDFLAGS	=	-s -L../../lib -lsxipc 

PROGRAMS = demrtest demmsg demqtest demwatch demmix sample \
	demburst momprod momcons momsrvr momclnt

all:		$(PROGRAMS) install

demburst:	demburst.o
		$(CC) -o $@ demburst.o	 $(LDFLAGS)

demrtest:	demrtest.o
		$(CC) -o $@ demrtest.o	 $(LDFLAGS)

demmsg: 	demmsg.o
		$(CC) -o $@ demmsg.o   $(LDFLAGS)

demqtest:	demqtest.o
		$(CC) -o $@ demqtest.o	 $(LDFLAGS)

demwatch:	demwatch.o
		$(CC) -o $@ demwatch.o	 $(LDFLAGS)

demmix: 	demmix.o
		$(CC) -o $@ demmix.o  $(LDFLAGS)

sample: 	sample.o
		$(CC) -o $@ sample.o  $(LDFLAGS)

momprod:	momprod.o
		$(CC) -o $@ momprod.o  $(LDFLAGS)

momcons:	momcons.o
		$(CC) -o $@ momcons.o  $(LDFLAGS)

momsrvr:	momsrvr.o
		$(CC) -o $@ momsrvr.o  $(LDFLAGS)

momclnt:	momclnt.o
		$(CC) -o $@ momclnt.o  $(LDFLAGS)

install:
		@echo "Installing $(PROGRAMS) in ../bin"
		@mv $(PROGRAMS) ../bin

clean:
		rm -f $(PROGRAMS) *.o

new: clean all


