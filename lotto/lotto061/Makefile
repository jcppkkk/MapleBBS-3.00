# ------------------------------------------------------ #
#  Makefile	( WD-BBS Ver 2.3 )			 #
# ------------------------------------------------------ #
#  target : Makefile for WD-BBS main programs		 #
#  create : 95/03/29				 	 #
#  update : 98/12/21					 #
# ------------------------------------------------------ #
BBSHOME = /home/bbs

# ------------------------------------------------------
# FreeBSD 3.x, Linux
# ------------------------------------------------------
OS_DEF	= -DBSD44 -DTWO_LONGJMP_ARGS 
CC	= gcc
CFLAGS	= -O2 -Wunused -I../include -pipe -fomit-frame-pointer -g
LDFLAGS	= -lcrypt -ltermcap -lcompat -L../lib -ldao -export-dynamic -L/usr/local/lib/mysql -lmysqlclient

#SO =	railway.so pip.so bbcall.so tv.so nine.so dice.so gp.so \
#	pnote.so indict.so chat.so mine.so xyz.so song.so \
#	guessnum.so stock.so bj.so gagb.so gamble.so marie.so \
#	poker.so seven.so chessmj.so bet.so vote.so mn.so rpg.so buy.so \
#	fortune.so votebrd.so mime.so race.so bingo.so five.so

SO =	lotto.so

ETC =	Makefile $(HDR)

       
.SUFFIXES: .o .c .ln .so

.c.o:   ;   $(CC) $(CFLAGS) $(OS_DEF) -c $*.c
.c.ln:  ;   lint -abhi $*.c
.o.so:	;   ld -s -G $*.o -o $*.so -L../lib -ldao

all:$(SO)
so: $(SO)

install: $(SO)
	cp -f *.so ../../lotto

clean: /tmp
	rm -fr $(GARBAGE) *.o $(SO) $(LNFILES) *~ 
