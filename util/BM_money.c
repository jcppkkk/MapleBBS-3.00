/*        �����D�����{��          */

#include "bbs.h"
#include "record.c"
#include "cache.c"

#define FUNCTION    (300 - (c*5))
#define BM_Money	BBSHOME"/log/BM_money"

extern int numboards;
extern boardheader *bcache;
extern struct UCACHE *uidshm;
char *fn_board=".BOARDS";
char *fn_passwd=".PASSWDS";
int c,n;
userec xuser;

int
getuser(userid)
  char *userid;
{
  int uid;

  if (uid = searchuser(userid))
  {
    rec_get(fn_passwd, &xuser, sizeof(xuser), uid);
  }
  return uid;
}


int
inumoney(char *tuser, int money)
{
  int unum;

  if (unum = getuser(tuser))
  {
    xuser.goldmoney += money;
    substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
    return xuser.goldmoney;
  }
  return -1;
}


void
main()
{
  FILE *fp=fopen(BBSHOME "/log/topboardman","r");
  FILE *fp2=fopen(BM_Money,"w");
  char buf[201],bname[20],BM[90],*ch;
  boardheader *bptr;
  int money,nBM;

  resolve_boards();
  if(!fp) return ;

  c=0;
  fgets(buf,200,fp); /* �Ĥ@�殳�� */

  fprintf(fp2,
"              [1;44m  ���y�u�}���D �C�g���~ �̺�ذϱƦW���t  [m\n\n"
"[33m                 (�ƦW�ӫ᭱�δX�G�S����ذϪ̤��C�J)[m\n"
"  �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w\n"
"\n\n");

  while(fgets(buf, 200, fp) != NULL)
  {
    buf[24] = 0;
    sscanf(&buf[9], "%s", bname);
    for (n = 0; n < numboards; n++)
    {
      bptr = &bcache[n];
      if (!strcmp(bptr->brdname,bname)) 
        break;
    }
    if (n == numboards)
      continue;

    if (FUNCTION <= 0) 
      break;
    
    strcpy(BM, bptr->BM);
    fprintf(fp2,"        (%d) %-15.15s %s \n",c+1,bptr->brdname,bptr->title);

    if (BM[0] == 0 || BM[0] == ' ' || strstr(BM, "�x�D")) 
      continue;
    else if(bptr->brdattr & BRD_GROUPBOARD || bptr->brdattr & BRD_CLASS) 
      continue;

    ch = BM;
    for (nBM = 1; (ch = strchr(ch, '/')) != NULL; nBM++) 
    {
      ch++;
    }
    ch = BM;


    fprintf(fp2,"             ���� [32m%6d [m     ���� [33m%s[m \n",
      FUNCTION,bptr->BM);

#if 1
    for (n = 0; n <nBM; n++)
    {
      fileheader mymail;
      char *ch1;
      if (ch1 = strchr(ch,'/'))
        *ch1 = 0;
      if(inumoney(ch, FUNCTION ) != -1)
      {
        char genbuf[200];
        sprintf(genbuf,BBSHOME"/home/%s", ch);
        stampfile(genbuf, &mymail);

        strcpy(mymail.owner, "[�~���U]");
//        sprintf(mymail.title, "���զӤw");
        sprintf(mymail.title,
          "[32m %s [m�����~�� �C[33m%d[m",bptr->brdname,FUNCTION);
        mymail.savemode = 0;
        f_rm(genbuf);
        f_ln(BM_Money, genbuf);
        sprintf(genbuf,BBSHOME"/home/%s/.DIR", ch);
        rec_add(genbuf, &mymail, sizeof(mymail));
      }
      ch=ch1+1;
    }
#endif
    
    c++;
  }
  fclose(fp2);
  fclose(fp);
}
