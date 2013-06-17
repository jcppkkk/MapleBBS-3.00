#!/bin/sh
#請以bbsadm的身份run此script
cd
mkdir /home/bbs/lotto
mkdir /home/bbs/lotto/history
mkdir /home/bbs/lotto/tmp
cp get_lotto_now.sh /home/bbs/lotto
cp lotto_open.sh /home/bbs/lotto
cp post_open.c /home/bbs/src/util/statool
cp lotto_rule.txt /homt/bbs/lotto
cd /home/bbs/src/lotto
make clean install clean
gcc -o /home/bbs/bin/get_lotto_now get_lotto_now.c
gcc -o /home/bbs/bin/lotto_open lotto_open.c

