# ------------------------------------------------------- #
#  util/Makefile	( NTHU CS MapleBBS Ver 2.36 )   	  #
# ------------------------------------------------------- #
#  target : Makefile for ALL				  #
# ------------------------------------------------------- #
BBSHOME=/home/2012/jethro/trunk/fastBBS/build

# freebsd , linux , solaris ... etc. (use by innbbsd)
CC=gcc
RANLIB	= ranlib

OSTYPE := $(shell uname -s)
ifeq ($(OSTYPE),FreeBSD)
	CPROTO	= cproto -E"gcc -pipe -E" # -s -v
	override CFLAGS	+= -O3 -s -pipe -fomit-frame-pointer -Wall -Werror -I../include -D FREEBSD
endif
ifeq ($(OSTYPE),Linux)
	CPROTO	:= cproto -E"gcc -pipe -E" # -s -v
	override CFLAGS	= -O3 -s -pipe -fomit-frame-pointer -Wall -Werror -I../include -D LINUX
endif
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	CCFLAGS += -D AMD64
endif
ifneq ($(filter %86,$(UNAME_P)),)
	CCFLAGS += -D IA32
endif
ifneq ($(filter arm%,$(UNAME_P)),)
	CCFLAGS += -D ARM
endif
