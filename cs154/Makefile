OBJS	:=	$(addsuffix	.o,	functions	printP2)

all:	$(OBJS)
	gcc	$(OBJS)	-g	main.c	-o	proj1

%.o:	%.c
	gcc	-c	$<

clean:
	rm	-f	*.o	proj1
