/*    Ptt   �}�� ��utility  */

#include "bbs.h"
#include "record.c"
#include "cache.c"

#define MAX_DES 8 /* �̤j�O�d���� */

#define DOTPASSWDS BBSHOME"/.PASSWDS"
userec xuser;
char *betname[8] = {"�h�³���","�S�O����","�C��p��","�]�O�p��",
                    "�ԾĤj��","�X�Y����","�@�����","�T���B�P"};

int getuser(userid)
  char *userid;
{
  int uid;
  if (uid = searchuser(userid))
   {
    rec_get(DOTPASSWDS, &xuser, sizeof(xuser), uid);
   }
  return uid;
}

int inumoney(char *tuser,int money)
{
  int unum;

  if (unum = getuser(tuser))
    {
      xuser.silvermoney += money;
      substitute_record(DOTPASSWDS, &xuser, sizeof(userec), unum);
      return xuser.silvermoney;
    }
  return -1;
}

char *
Cdatelite(clock)
  time_t *clock;
{
  static char foo[18];
  struct tm *mytm = localtime(clock);

  strftime(foo, 18, "%D %T", mytm);
  return (foo);
}


main()
{
 int money, bet, n, total=0, ticket[8] = {0,0,0,0,0,0,0,0};
 FILE *fp;
 time_t  now = time(0);
 char des[MAX_DES][200]={"","","",""};

 f_mv(BBSHOME "/" FN_TICKET_RECORD,BBSHOME "/" FN_TICKET_RECORD".tmp");
 f_mv(BBSHOME "/" FN_TICKET_USER,BBSHOME "/" FN_TICKET_USER".tmp");

 if(!(fp = fopen(BBSHOME "/" FN_TICKET_RECORD ".tmp" ,"r")))  return;         
 fscanf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                 &ticket[0],&ticket[1],&ticket[2],&ticket[3],
                 &ticket[4],&ticket[5],&ticket[6],&ticket[7]);         
 for(n = 0; n < 8; n++)
                total += ticket[n];
 fclose(fp);

 if(!total) return;
 
 if(fp = fopen(BBSHOME "/" FN_TICKET,"r"))
  {
   for(n=0; n < MAX_DES && fgets(des[n],200,fp); n++);
   fclose(fp);
  }

 srandom(now + 11 );
 srandom(random()/(RAND_MAX+1.0)*1000.0);
 bet=(int) (8.0*random()/(RAND_MAX+1.0));

 money = ticket[bet] ? total * 95 / ticket[bet] : 9999999;

 if(fp = fopen(BBSHOME "/" FN_TICKET,"w"))
  {
   if(des[MAX_DES-1][0]) n = 1;
   else n = 0;
   for(; n < MAX_DES && des[n][0] != 0; n++)
        {
         fprintf(fp,des[n]);
        }
   fprintf(fp,"%s ��L�}�X:%s �`���B:%d00 �� ����/�i:%d �� ���v:%1.2f\n",
         Cdatelite(&now), betname[bet], total, money,
         (float)ticket[bet]/total); 
   fclose(fp);
  }

 if(ticket[bet] && (fp = fopen(BBSHOME "/" FN_TICKET_USER".tmp","r"))) 
                        /* ���H�㤤 */
        {
          int mybet, num;
          char userid[20], genbuf[200];
          fileheader mymail;

          while (fscanf(fp,"%s %d %d\n", userid, &mybet, &num) != EOF)
                {
                  if(mybet == bet)
                        {       
                          printf("%s ���F %d �i [%s] �o %d ��\n",userid, num, betname[mybet], money * num);
                          if(inumoney(userid, money * num) == -1) continue;
                          sprintf(genbuf,BBSHOME"/home/%s", userid);
                          stampfile(genbuf, &mymail);
                          strcpy(mymail.owner, "���ֽ�L");
                          sprintf(mymail.title, "[%s] �����o! $ %d", Cdatelite(&now), money * num);
                          mymail.savemode = 0;
                          unlink(genbuf);
                          f_cp(BBSHOME "/" FN_TICKET, genbuf,O_TRUNC);
                          sprintf(genbuf,BBSHOME"/home/%s/.DIR",userid);
                          rec_add(genbuf, &mymail, sizeof(mymail));
//                          keeplog("etc/ticket","Record","���ֶ}��");

                        }
                }
        }
 unlink(BBSHOME "/" FN_TICKET_RECORD".tmp");
 unlink(BBSHOME "/" FN_TICKET_USER".tmp");
}
