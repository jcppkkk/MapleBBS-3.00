#!/bin/sh
#�ХHbbsadm��������lotto src�Brun��script
#�Ybbs�t�Ϊ��a�ؿ����O/home/bbs,�Ф�ʭק�or��ʰ���
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
