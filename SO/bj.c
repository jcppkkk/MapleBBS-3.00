#include "bbs.h"

#define ba rpguser.race == 6 ? 10*rpguser.level : 5
/* �³ǧJ�C�� */

int print_card(int card,int x,int y)
{
  char *flower[4]={"��","��","��","��"};
  char *poker[52]={"��","��","��","��","��","��","��","��","��","��","��","��",
                   "��","��","��","��","��","��","��","��","��","��","��","��",
                   "��","��","��","��","��","��","��","��","��","��","��","��",
                   "10","10","10","10","��","��","��","��","��","��","��","��",
                   "��","��","��","��"};

move(x,y);   prints("�~�w�w�w��");
move(x+1,y); prints("�x%s    �x",poker[card]);
move(x+2,y); prints("�x%s    �x",flower[card%4]);
move(x+3,y); prints("�x      �x");
move(x+4,y); prints("�x      �x");
move(x+5,y); prints("�x      �x");
move(x+6,y); prints("���w�w�w��");
return 0;
}


void
BlackJack()
{
  char buf[256];
  int    num[52]={11,11,11,11,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,
                   7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,10,10,10,
                  10,10,10,10,10,10,10,10};
  int cardlist[52]={0};
  int i,j,m,tmp=0,tmp2,ch;
  int win=2,win_jack=5; /* win ��Ĺ�ɪ����v, win_jack ���e��i�N 21 �I���v */
  int six=10, seven=20, aj=10, super_jack=20; /* 777, A+J, spade A+J �����v */
  int host_count=2, guest_count=1, card_count=3, A_count=0, AA_count=0;
  int host_point=0, guest_point=0, mov_y=4;
  int host_card[12]={0}, guest_card[12]={0};
  long int money;

  int CHEAT=0; /* �����Ѽ�, 1 �N�@��, 0 �N���@ */

  time_t now = time(0);
  time(&now);

  clear();
  setutmpmode(BLACKJACK);
  do{
  move(0,0);prints("�z���W�٦� [1;44;33m%d[m ��",cuser.silvermoney);
  getdata(1, 0, "�n��`�h�ֿ�(1 - 250000)? ", buf, 7, DOECHO,0);
  money=atoi(buf);
  if(!buf[0])return;
  if(check_money(money,SILVER)) return;
  }while((money<1)||(money>250000));
  demoney(money);
  move(2,0);prints("(�� y ��P, n ����P, d double)");
  move(0,0);clrtoeol();prints("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
  for(i=1;i<=52;i++){
    m=0;
    do{
    j=(time(0)+cuser.silvermoney+rand())%52;
    if (cardlist[j]==0){cardlist[j]=i;m=1;}
    }while(m==0);
  };
  for(i=0;i<52;i++)cardlist[i]--; /* �~�P */

  if(money>=20000)CHEAT=1;
  if(CHEAT==1){
    if(cardlist[1]<=3){
      tmp2=cardlist[50];
      cardlist[50]=cardlist[1];
      cardlist[1]=tmp2;
    }
  }                             /* �@���X */

  host_card[0]=cardlist[0];
  if(host_card[0]<4)AA_count++;
  guest_card[0]=cardlist[1];

  if(guest_card[0]<4)A_count++;
  host_card[1]=cardlist[2];
  if(host_card[1]<4)AA_count++; /* �o�e�T�i�P */

  move(5,0);  prints("�~�w�w�w��");
  move(6,0);  prints("�x      �x");
  move(7,0);  prints("�x      �x");
  move(8,0);  prints("�x      �x");
  move(9,0);  prints("�x      �x");
  move(10,0); prints("�x      �x");
  move(11,0); prints("���w�w�w��");
  print_card(host_card[1],5,4);
  print_card(guest_card[0],15,0);  /* �L�X�e�T�i�P */

  host_point=num[host_card[1]];
  guest_point=num[guest_card[0]];

  do{
    m=1;
    guest_card[guest_count]=cardlist[card_count];
    if(guest_card[guest_count]<4)A_count++;
    print_card(guest_card[guest_count],15,mov_y);
    guest_point+=num[guest_card[guest_count]];

    if((guest_card[0]>=24&&guest_card[0]<=27)&&(guest_card[1]>=24&&guest_card[1]<=27)&&(guest_card[2]>=24&&guest_card[2]<=27)){
      move(18,3);prints("[1;41;33m     ������     [m");
      move(3,0);prints("[1;41;33m������ !!! �o���� %d �Ȩ�[m",money*seven);
      inmoney(money*seven);
//      inexp(ba*7);
      game_log(BLACKJACK,"���F [1;33m%7d[m ���� "COLOR1"[1m  ������   [m"
        ,money*seven);
      pressanykey("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
      return;
    }

    if((guest_card[0]==40&&guest_card[1]==0)||(guest_card[0]==0&&guest_card[1]==40)){
      move(18,3);prints("[1;41;33m �W�ť��� BLACK JACK  [m");
      move(3,0);prints("[1;41;33m�W�ť��� BLACK JACK !!! �o���� %d �Ȩ�[m",money*super_jack);
      inmoney(money*super_jack);
//      inexp(ba*5);
      game_log(BLACKJACK,"���F [1;33m%7d[m ���� [1;41;33m ���� �Ϣ� [m"
        ,money*super_jack);
      pressanykey("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
      return;
    }

    if((guest_card[0]<=3&&guest_card[0]>=0)&&(guest_card[1]<=43&&guest_card[1]>=40))tmp=1;

if((tmp==1)||((guest_card[1]<=3&&guest_card[1]>=0)&&(guest_card[0]<=43&&guest_card[0]>=40))){
      move(18,3);prints("[1;41;33m SUPER BLACK JACK  [m");
      move(3,0);prints("[1;41;33mSUPER BLACK JACK !!! �o���� %d �Ȩ�[m",money*aj);
      inmoney(money*aj);
//      inexp(ba*5);
      game_log(BLACKJACK,"���F [1;33m%7d[m ���� [1;44;33m Super�Ϣ� [m",money*aj);
      pressanykey("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
      return;
    }

    if(guest_point==21&&guest_count==1){
      move(18,3);prints("[1;41;33m  BLACK JACK  [m");
      move(3,0);prints("[1;41;33mBLACK JACK !!![44m �o���� %d �Ȩ�[m",money*win_jack);
      inmoney(money*win_jack);
//      inexp(ba*3);
      move(0,0);clrtoeol();prints("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
    if(money*win_jack>=500000){
      game_log(BLACKJACK,"���F [1;33m%7d[m ���� [1;47;30m BlackJack [m %s ",money*win_jack);
    }

      pressanykey(NULL);
      return;
    }                        /* �e��i�N 21 �I */

    if(guest_point>21){
      if(A_count>0){guest_point-=10;A_count--;};
    }
    move(12,0); clrtoeol();prints("[1;32m�I��: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m�I��: [33m%d[m",guest_point);
    if(guest_point>21){
      pressanykey("  �z����~~~  ");
      return;
    }

    if(guest_count==5){
      move(18,3);prints("[1;41;33m            �L����            [m");
      move(3,0);prints("[1;41;33m�L���� !!! �o���� %d �Ȩ�[m",money*six);
      inmoney(money*six);
//      inexp(ba*5);
      game_log(BLACKJACK,"���F [1;33m%7d[m ���� [1;44;33m  �L����   [m",money*six);
      pressanykey("�z�٦� %d �ȹ�",cuser.silvermoney);
      return;
    }

    guest_count++;
    card_count++;
    mov_y+=4;

    do{
      if(ch=='d')m=0;
      if(m!=0)ch=igetkey();
    }while(ch!='y'&&ch!='n'&&ch!='d'&&m!=0); /* �� key */

    if(ch=='d'&&m!=0&&guest_count==2){
      if(cuser.silvermoney>=money){
        demoney(money);
        money*=2;
      }
      else ch='n';
      move(0,0);clrtoeol();prints("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
    }                                      /* double */

    if(ch=='d'&&guest_count>2)ch='n';
    if(guest_point==21)ch='n';
  }while(ch!='n'&&m!=0);

  mov_y=8;

  print_card(host_card[0],5,0);
  print_card(host_card[1],5,4);
  host_point+=num[host_card[0]];

  do{

    if(host_point<guest_point){
      host_card[host_count]=cardlist[card_count];
      print_card(host_card[host_count],5,mov_y);
      if(host_card[host_count]<4)AA_count++;
      host_point+=num[host_card[host_count]];
    }
    if(host_point>21){
      if(AA_count>0){host_point-=10;AA_count--;};
    }
    move(12,0); clrtoeol();prints("[1;32m�I��: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m�I��: [33m%d[m",guest_point);
    if(host_point>21){
      move(14,0); clrtoeol(); prints("[1;32m�I��: [33m%d [1;41;33m WINNER [m",guest_point);

      move(3,0);prints("[1;44;33m�AĹ�F~~~~ �o���� %d �Ȩ�[m",money*win);
      inmoney(money*win);
//      inexp(ba);
      move(0,0);clrtoeol();prints("�z�٦� [1;44;33m%d[m �Ȩ�",cuser.silvermoney);
      pressanykey(NULL);
      return;
    }
    host_count++;
    card_count++;
    mov_y+=4;
  }while(host_point<guest_point);

  pressanykey("�A��F~~~~ �Ȩ�S��!");
  return;
}
