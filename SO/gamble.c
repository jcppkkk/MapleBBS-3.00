#include "bbs.h"
#define ba rpguser.race == 6 ? 10*rpguser.level : 5
char *betname[8] = {"�h�³���","�S�O����","�C��p��","�]�O�p��",
                    "�ԾĤj��","�X�Y����","�@�����","�T���B�P"};

/* ----------------------------------- */
/* gamble.c ��@��: Ptt ��g��: yychen */
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
         "[1;37;44m �U�`�Ҧ����B: [33m%d00[36m �� [m",
         betname[0], ticket[0], betname[1], ticket[1],
         betname[2], ticket[2], betname[3], ticket[3],
         betname[4], ticket[4], betname[5], ticket[5],
         betname[6], ticket[6], betname[7], ticket[7], total);
}


show_ticket_data()
{
 clear();
 showtitle("���ֽ�L", BoardName);
 move(2,0);
 outs(
        "[1;32m�W�h:[m 1.�i�ʶR�K�ؤ��P�������m���C�C�i�n��100���C\n"
        "      2.�C�Q�G�p�ɶ}���@��(6:30 18:30)�C\n"
        "      3.�}���ɷ|��X�@�رm��, ���ʶR�ӱm����, �h�i���ʶR���i�Ƨ����`����C\n"
        "      4.�C�������Ѩt�Ω��5%���|���C\n\n"
        "[1;32m�e�X���}�����G:[m" );
 show_file(FN_TICKET, 8, 8,NO_RELOAD);
 move(15,0);
 outs("[1;36m�ثe�U�`���p:[m\n");
 show_bet();
 move(20,0);
 prints(COLOR1"[1m�A���W���G %-10d �� [m           [m\n",
   cuser.silvermoney);
 prints("[1m�п�ܭn�ʶR������(1~8)[Q:���}]:[m");
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
  getdata(b_lines-1,0,"�n�R�h�֥��O:",buf,4,DOECHO,0);
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
          outs("�{������ !!!");
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
    pressanykey("�d�x�p�j: �èS������@�ӤH(�]�A�z)��`.");
    return;
  }
  if (!i)
  {
    pressanykey("�d�x�p�j: ��p�A�z�èS�������@���I");
    return;
  }
  clear();
  prints("[1;33m�d�x�p�j���z�@�i�M��G[m\n");
  prints("[32m�˪L��L�G[1;32m%s[0;32m �U�`�@����[m\n\n",
    cuser.userid);
  for(i = 0; i < 8; i++)
  {
    prints("[1;3%dm�z(%s) ��F [%d. %-4s]�G%d �i[m\n", 
      i+1 <= 7 ? i+1 : i-6,
      cuser.userid, i+1, betname[i],
      tickets[i]);
  }
  pressanykey("�d�x�p�j: �H�W�O�z����`���p�C");
}
