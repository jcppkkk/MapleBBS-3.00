#include "bbs.h"
#define ba rpguser.race == 6 ? 10*rpguser.level : 5
char *betname[8] = {"疲倦野貓","沒力咩咩","遊魂小風","魔力小夢",
                    "勤奮大魚","碼頭阿倫","一堆螞蟻","嘰哩咕嚕"};

/* ----------------------------------- */
/* gamble.c 原作者: Ptt 改寫者: yychen */
/*                                     */
/* ----------------------------------- */
show_bet()
{
  FILE *fp = fopen(FN_TICKET_RECORD,"r");
  int i, total=0, ticket[8] = {0,0,0,0,0,0,0,0};

  if(fp)
        {
          fscanf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                 &ticket[0],&ticket[1],&ticket[2],&ticket[3],
                 &ticket[4],&ticket[5],&ticket[6],&ticket[7]);
          for(i = 0; i < 8; i++)
                total += ticket[i];
          fclose(fp);
        }
  prints("[1;33m1.%-8s:%-9d2.%-8s:%-9d3.%-8s:%-9d4.%-8s:%-9d[m\n"
         "[1;33m5.%-8s:%-9d6.%-8s:%-9d7.%-8s:%-9d8.%-8s:%-9d[m\n"
         "[1;37;44m 下注所有金額: [33m%d00[36m 金 [m",
         betname[0], ticket[0], betname[1], ticket[1],
         betname[2], ticket[2], betname[3], ticket[3],
         betname[4], ticket[4], betname[5], ticket[5],
         betname[6], ticket[6], betname[7], ticket[7], total);
}


show_ticket_data()
{
 clear();
 showtitle("對對樂賭盤", BoardName);
 move(2,0);
 outs(
        "[1;32m規則:[m 1.可購買八種不同類型的彩票。每張要花100元。\n"
        "      2.每十二小時開獎一次(6:30 18:30)。\n"
        "      3.開獎時會抽出一種彩票, 有購買該彩票者, 則可依購買的張數均分總賭金。\n"
        "      4.每筆獎金由系統抽取5%之稅金。\n\n"
        "[1;32m前幾次開獎結果:[m" );
 show_file(FN_TICKET, 8, 8,NO_RELOAD);
 move(15,0);
 outs("[1;36m目前下注狀況:[m\n");
 show_bet();
 move(20,0);
 prints(COLOR1"[1m你身上有： %-10d 元 [m           [m\n",
   cuser.silvermoney);
 prints("[1m請選擇要購買的種類(1~8)[Q:離開]:[m");
}

int
append_ticket_record(ch,n)
{
  FILE *fp;
  int ticket[8] = {0,0,0,0,0,0,0,0};
  if(fp = fopen(FN_TICKET_USER,"a"))
        {
          fprintf(fp,"%s %d %d\n",cuser.userid,ch,n);
          fclose(fp);
        }

  if(fp = fopen(FN_TICKET_RECORD,"r+"))
        {
          fscanf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    &ticket[0],&ticket[1],&ticket[2],&ticket[3],
                    &ticket[4],&ticket[5],&ticket[6],&ticket[7]);
          ticket[ch] += n;
          rewind(fp);
          fprintf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    ticket[0],ticket[1],ticket[2],ticket[3],
                    ticket[4],ticket[5],ticket[6],ticket[7]);
          fclose(fp);
        }
  else if(fp = fopen(FN_TICKET_RECORD,"w"))
        {
          ticket[ch] += n;
          fprintf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    ticket[0],ticket[1],ticket[2],ticket[3],
                    ticket[4],ticket[5],ticket[6],ticket[7]);
          fclose(fp);
        }
}

void
show_picture(char *filename)
{
 FILE *fp;
 char buf[256];
 move(5,0);
 if((fp=fopen(filename,"r")))
  {
    while(fgets(buf,256,fp))
        {
          outs(buf);
        }
    fclose(fp);
  }
}
void
clear_picture()
{
 char i;
 for (i=5;i<18;i++)
  {
        move(i,0);
        clrtoeol();
  }
}

void
ch_buyitem(money,picture,item)
int money;
char *picture;
int *item;
{
  int num=0;
  char buf[5];
  getdata(b_lines-1,0,"要買多少份呢:",buf,4,DOECHO,0);
  num = atoi(buf);
  if(num<0) num=0;
  if (cuser.silvermoney > money*num)
        {
          *item += num;
          demoney(money*num);
          clear_picture();
          show_picture(picture);
        }
  else
        {
          move(b_lines-1,0);
          clrtoeol();
          outs("現金不夠 !!!");
        }
  pressanykey(NULL);
}


int
ticket_main()
{
 int ch,n;

 if (lockutmpmode(TICKET))  return 0;

 setutmpmode(TICKET);
 log_usies("GAMBLE",NULL);
 while(1)
  {
   show_ticket_data();
   ch = igetch();
   if(ch=='q' || ch == 'Q') break;
   ch -= '1';
   if(ch > 7 || ch < 0) continue;
   n = 0;
   ch_buyitem(100,"etc/buyticket",&n);
   if(n>0)  append_ticket_record(ch,n);
  }
 unlockutmpmode();
 return 0;
}

int
query_ticket()
{
  FILE *fp;
  int tickets[8] = {0,0,0,0,0,0,0,0};
  int num1, num2, i=0;
  char userid[IDLEN+1];

  if (fp = fopen(FN_TICKET_USER, "r"))
  {
      while ((fscanf(fp, "%s %d %d", userid, &num1, &num2))!=EOF)
      {
        if (!strcasecmp(userid, cuser.userid))
        {
          if (!i)
            i = 1;
          tickets[num1] += num2;
        }
      }
      fclose(fp);
  }
  else
  {
    pressanykey("櫃台小姐: 並沒有任何一個人(包括您)押注.");
    return;
  }
  if (!i)
  {
    pressanykey("櫃台小姐: 抱歉，您並沒有押任何一項！");
    return;
  }
  clear();
  prints("[1;33m櫃台小姐給您一張清單：[m\n");
  prints("[32m森林賭盤：[1;32m%s[0;32m 下注一覽表[m\n\n",
    cuser.userid);
  for(i = 0; i < 8; i++)
  {
    prints("[1;3%dm您(%s) 押了 [%d. %-4s]：%d 張[m\n", 
      i+1 <= 7 ? i+1 : i-6,
      cuser.userid, i+1, betname[i],
      tickets[i]);
  }
  pressanykey("櫃台小姐: 以上是您的押注狀況。");
}
