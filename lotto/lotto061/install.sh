#!/bin/sh
#請以bbsadm的身份於lotto src處run此script
#若bbs系統的家目錄不是/home/bbs,請手動修改or手動執行
mkdir /home/bbs/lotto
mkdir /home/bbs/lotto/history
mkdir /home/bbs/lotto/tmp
cp get_lotto_now.sh /home/bbs/lotto
cp lotto_open.sh /home/bbs/lotto
cp lotto_rule.txt /home/bbs/lotto
cp lotto_on_off.txt /home/bbs/lotto
cp post_open.c /home/bbs/src/util/statool
cp lotto_run.sh /home/bbs/lotto
cp lotto_close.sh /home/bbs/lotto
cd /home/bbs/src/util/statool
make install
cd /home/bbs/src/lotto
make clean install 
make clean
gcc -o /home/bbs/lotto/get_lotto_now get_lotto_now.c
gcc -o /home/bbs/lotto/lotto_open lotto_open.c
