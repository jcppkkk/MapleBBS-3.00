#!/usr/bin/perl
open(BBSPOST, "> /home/bbs/game/stock_now1");
open(STOCK, "lynx -source http://www.otc.org.tw/statis/real/LISTall.html |");

while (<STOCK>)
{
  print BBSPOST if($_ ne "\n") ;
}
close STOCK;

print BBSPOST "\n--\n";
print BBSPOST "�� Origin: ^[[31;1m�G^[[33;1m�T^[[32;1m�l^[[0m (140.113.124.116)�� �ڨӦ�:[����Ҩ�����]   ";

close BBSPOST;
