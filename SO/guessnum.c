/*-------------------------------------------------------*/
/* guessnum.c   ( NTHU CS MapleBBS Ver 3.10 )            */
/*-------------------------------------------------------*/
/* author : thor.bbs@bbs.cs.nthu.edu.tw			 */
/* target : Guess Number tool dynamic link module        */
/* create : 99/02/16                                     */
/* update :   /  /                                       */
/*-------------------------------------------------------*/

#if 0
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <sys/types.h>
#include <time.h>
#endif
#include "bbs.h"

#define ba rpguser.race == 6 ? 10*rpguser.level : 5

typedef char Num[4];

typedef struct{
  Num n;
  int A, B;
} His;

static int hisNum;
static His *hisList;

static int numNum;
static char *numSet;

#if 1
void num_init() 
{
  hisList = (His *)malloc(sizeof(His)); /* pseudo */
  numSet = (char *)malloc(10*9*8*7*sizeof(char));
}

void num_fini() 
{
  free(hisList);
  free(numSet);
}
#endif

static void AB(Num p, Num q, int *A, int *B)
{ /* compare p and q, return ?A?B */
  int i, j;
  *A = *B = 0;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      if(p[i]==q[j])
        if(i==j)
          ++*A;
        else
          ++*B;
}

static int getth(int o, char a[])
{ /* return "o"th element index in a[], base 0 */
  int i = -1;
  o++;
  while(o)
    if(a[++i])
      continue;
    else
      o--;
  return i;
}

static void ord2Num(int o, Num p)
{ /* return "o"th filtered number */
  char digit[10];
  int i,j,k;
  
  memset(digit,0,sizeof digit);

  for(j = 0, k = 10; j < 4; j++, k--)
  {
    i = o % k; o /= k;
    i = getth(i, digit);
    p[j] = i; digit[i] = 1;
  }
}

static int matchHis(Num n)
{
  int i, A, B;
  for(i=0;i<hisNum;i++)
  {
    AB(n,hisList[i].n,&A,&B);
    if(A != hisList[i].A || B != hisList[i].B)
      return 0;
  }
  return 1;
}

int mainNum(int fighting /* Thor.990317: 對戰模式 */)
{
  Num myNumber;
  char buf[80];
  int money;
  /* 倍率        0  1   2   3   4   5   6   7   8   9   10 */
  float bet[11]={0,100, 50, 10, 3 ,1.5,1.2,0.9, 0.8 ,0.5,0.1}; 
  /* initialize variables */
#if 1
  if(!hisList) hisList = (His *)malloc(sizeof(His)); /* pseudo */
  if(!numSet) numSet = (char *)malloc(10*9*8*7*sizeof(char));
#endif

  hisNum = 0;       

  numNum = 10*9*8*7;
  memset(numSet, 0, numNum*sizeof(char));

  srand(time(NULL));

  /* Thor.990317:對戰模式 */
  stand_title(fighting ? "猜數字大戰" :"傻瓜猜數字"); /* clear(); */

  do{
    move(0,0);
    prints("您身上還有 [1;44;33m%d[m 元",cuser.silvermoney);
    getdata(1, 0, "要押注多少錢(1 - 250000)? ", buf, 7, DOECHO,0);
    money=atoi(buf);
    if(!buf[0])return;
    if(check_money(money,SILVER)) return;
  }while((money<1)||(money>250000));

  /* Thor.990221: 有人反應離不開 */
  if(answer("想好您的數字了嗎?[y/N]")!='y')
  {
    pressanykey("不玩了啊? 下次再來哦! ^_^");
    return 0;
  }

  if(fighting)
    ord2Num(rand() % numNum, myNumber); /* Thor.990317:對戰模式 */

  demoney(money);
  /* while there is possibility */
  for(;;)
  {
    Num myGuess, yourGuess;
    int youA,youB, myA, myB;

    if(fighting)  /* Thor.990317:對戰模式 */
    {
      int i , a , b;
      char tmp[50];
     do{
      getdata(b_lines-3, 0, "您猜我的數字是[????]:", tmp, 5, DOECHO,0);
      /* Thor.990317: 為簡化, 不作checking */
      if(!tmp[0]) goto abort_game;
      if(tmp[0] == '!' && HAS_PERM(PERM_SYSOP))
        pressanykey("%d%d%d%d",myNumber[0],myNumber[1],myNumber[2],myNumber[3]);
      for (a=0;a<3;a++)
      for (b=a+1;b<4;b++)
      if(tmp[a]==tmp[b])
        tmp[0]=0;
      if(!tmp[0])
      {
        pressanykey("輸入數字有問題!!"); 
      }
      
      }while  (!tmp[0]);    
      for(i=0;i<4;i++) 
        yourGuess[i] = tmp[i] - '0';
      AB(myNumber,yourGuess, &myA, &myB);
      move(b_lines-2,0); clrtoeol();
      move(b_lines-2,0);
      prints("我說 \033[1m%dA%dB \033[m", myA, myB);
     
      if(myA==4)
      {
        int m = money ,e = ba ;
        /* you win  */
#if 1
        game_log(NumFight,"猜了%d次,贏了%d元,EXP %d 點"
          , hisNum,(int)bet[hisNum]*money,(int)bet[hisNum]*ba);
#endif
        if(hisNum > 10) hisNum = 10;
        m *= bet[hisNum]; e *= bet[hisNum];
        inmoney(m);
//        inexp(e); 
//        pressanykey("您贏了! 好崇拜 ^O^,獎金 %d 元,經驗值 %d 點!",m,e);
        pressanykey("您贏了! 好崇拜 ^O^,獎金 %d 元!",m);
        return 0;
      }  
    }

    /* pickup a candidate number */
    for(;;)
    {
      int i;
      /* pickup by random */
      if(numNum<=0) 
        goto foolme;
      i = rand() % numNum;
      i = getth(i, numSet); /* i-th ordering num */
      numSet[i] = 1; numNum--; /* filtered out */
      ord2Num(i, myGuess); /* convert ordering num to Num */

      /* check history */
      if(matchHis(myGuess))
        break;
    }

    /* show the picked number */
    move(b_lines - 1, 0); clrtoeol();
    move(b_lines - 1, 0); 
    prints("我猜您的數字是 \033[1;37m%d%d%d%d\033[m",myGuess[0], myGuess[1], myGuess[2], myGuess[3]);

    /* get ?A?B */
    for(;;)
    {
      char buf[5];
      /* get response */
      getdata(b_lines, 0, "您的回答[?A?B]:", buf, 5, DOECHO,0);

      if(!buf[0])
      {
abort_game:
        /* abort */
        pressanykey("不玩了啊? 下次再來哦! ^_^");
#if 1
        game_log(NumFight,"猜了%d次,不想玩了!", hisNum);
#endif
        return 0;
      }
      if(isdigit(buf[0]) && (buf[1]|0x20) == 'a'
       &&isdigit(buf[2]) && (buf[3]|0x20) == 'b' )
      {
        youA = buf[0] - '0';
        youB = buf[2] - '0';
        /* check legimate */
        if(youA >= 0 && youA <=4
          && youB >=0 && youB <= 4
          && youA + youB <= 4)
        {
          /* if 4A, end the game */
          if(youA == 4)
          {
           int m=money,e=ba;   
#if 1
           game_log(NumFight,"猜了%d次,輸給電腦!", hisNum);
#endif
           /* I win  */
           if(hisNum > 10) hisNum = 10;  
           m *= bet[10 - hisNum]*0.1; e *= bet[10 - hisNum];
//           pressanykey("我贏了! 厲害吧 ^O^,賠你 %d 元,得到 %d 點經驗值",m,e);
	   pressanykey("我贏了! 厲息a ^O^,賠你 %d 元!",m);
           inmoney(m); 
//           inexp(e); 
           return 0;
          }
          else
            break;
        }
      }
      /* err A B */
      outmsg("輸入格式有誤");
    }
    /* put in history */
    hisNum ++;
    hisList = (His *) realloc(hisList, hisNum * sizeof(His)); /* assume must succeeded */
    memcpy(hisList[hisNum - 1].n, myGuess, sizeof(Num));
    hisList[hisNum - 1].A=youA;
    hisList[hisNum - 1].B=youB;

    move(2+hisNum,0);
    if(fighting) /* Thor.990317: 對戰模式 */
      prints("第 \033[1;37m%d\033[m 次, 你猜 \033[1;36m%d%d%d%d\033[m, 我說 \033[1;33m%dA%dB\033[m; 我猜 \033[1;33m%d%d%d%d\033[m, 你說 \033[1;36m%dA%dB\033[m", hisNum, yourGuess[0], yourGuess[1], yourGuess[2], yourGuess[3], myA, myB, myGuess[0], myGuess[1], myGuess[2], myGuess[3], youA, youB);
    else
      prints("第 \033[1;37m%d\033[m 次, 我猜 \033[1;33m%d%d%d%d\033[m, 你說 \033[1;36m%dA%dB\033[m", hisNum, myGuess[0], myGuess[1], myGuess[2], myGuess[3], youA, youB);
  }
foolme:
  /* there is no posibility, show "you fool me" */
  pressanykey("你騙我! 不跟你玩了 ~~~>_<~~~");
#if 1
      game_log(NumFight,"猜了%d次,耍電腦,被趕出去!", hisNum);
#endif
  return 0;
}

int guessNum()
{
  mainNum(0);
}

int fightNum()
{
  setutmpmode(NumFight);
  mainNum(1);
}
