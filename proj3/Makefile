OBJS	:=	$(addsuffix	.o,	cachesim)

all:	$(OBJS)
	gcc	$(OBJS)	-g	main.c	-o	proj3

%.o:	%.c
	gcc	-c	$<

clean:
	rm	-f	*.o	proj3
