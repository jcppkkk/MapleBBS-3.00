#include "record.c"
#include "cache.c"
                                                                                  
#define fn_passwd BBSHOME"/.PASSWDS"
#define pwdfile fn_passwd
#define taxfile BBSHOME"/log/counter/tax_income"
                                                                                  
#define gold_lower_bound 100    // �C�� 100 ���̤����|
#define exp_lower_bound 1000
#define tax_rate_a 1000         // �|�v A  �d��������
#define tax_rate_b 100          // �|�v B  �ʤ������� (�Ӥ[�S�W����)
                                                                                  
#define CHECK_LEVEL(x,level)     ((x)?level&(x):1)
                                                                                  
userec auser;
userec xuser;
                                                                                  
int getuser(userid)
  char *userid;
{
  int uid;
  if (uid = searchuser(userid))
    rec_get(pwdfile, &xuser, sizeof(xuser), uid);
  return uid;
}
                                                                                  
int taxtax(char *tuser,int money, int exp)
{
  int unum;
  if (unum = getuser(tuser))
    {
      xuser.goldmoney -= money;
      xuser.exp -= exp;
      substitute_record(pwdfile, &xuser, sizeof(userec), unum);
      return xuser.goldmoney;
    }
  return -1;
}

int
bad_user_id(userid)
  char *userid;
{
  register char ch;
  if (strlen(userid) < 2)    return 1;
  if (not_alpha(*userid))    return 1;
  while (ch = *(++userid))
    if (not_alnum(ch))      return 1;
  return 0;
}
                                                                                  
int main(int argc, char *argv[])
{
  FILE *inf;
  time_t now;
  int i, j=0, t=0, expire, p_money, exp, total=0;
  char u='s', l='u', mode[10], name[20];
                                                                                  
  inf = fopen(pwdfile,"rb");
                                                                                  
  if (inf == NULL)
  {
    printf("Sorry, user data is not ready.\n");
    exit(0);
  }
                                                                                  
  if (argc != 2)
  {
     printf("\nusage: allgoldexpire mode \n\n");
     printf(" -l: list all users who have more than 100 golds\n");
     printf(" -e: list users who need to degold\n");
     printf(" -t: get tax from all users (only tax)\n");
     printf(" -g: get tax and get more gold from all users\n\n");
     exit(0);
  }
   else
     strcpy(mode,argv[1]);
  time(&now);
  for (i = 0; fread(&auser, sizeof(userec), 1, inf); i++)
  {
    if (bad_user_id(auser.userid) || strchr(auser.userid,'.') )
        i--;
    else
    {
       t=now-auser.lastlogin;
       exp=0; p_money=0; expire=0;
                                                                                  
       u='s';
       if(t>60) {          t=t/60;          u='m';       }
       if(t>60) {          t=t/60;          u='h';       }
       if(t>24)
       {
          t=t/24;
          u='d';
          if (t>=7)   expire=1;         // �W�L7�ѥ[��
          if (t>=14)  expire=2;         // �W�L28�ѦA�[��
          if (t>=28)  expire=3;         // �̦�����
          if (t>=56)  expire=4;
          if (t>=112) expire=5;
          if (t>=224) expire=7;
       }
       l='u';
       if CHECK_LEVEL(PERM_BM, auser.userlevel) l='b';
       if CHECK_LEVEL(PERM_BBSADM, auser.userlevel) l='a';
       if CHECK_LEVEL(PERM_SYSOP, auser.userlevel)
       {
          expire=0;      l='s';
       }
                                                                                  
       strcpy(name, auser.userid);
       if (expire==0 || (strcmp(mode, "-t")==0))
       {
//             exp=1 + (auser.exp/tax_rate_a);
             p_money=1 + (auser.goldmoney/tax_rate_a);
       }
       else
       {
//          exp=1+ (auser.exp/tax_rate_a);
//          exp=exp + (expire * (auser.exp/tax_rate_b));
          p_money=1 + (auser.goldmoney/tax_rate_a);
          p_money=p_money + (expire * (auser.goldmoney/tax_rate_b));
       }
//       if (auser.exp < exp_lower_bound) exp=0;
       if (auser.goldmoney < gold_lower_bound) p_money=0;
                                                                                  
       if (strcmp(mode, "-l")==0)
          printf("%04d. %12s (%c) gold:%10d expire:%5d%c p:%5dg\n",
             ++j, name, l, auser.goldmoney, t, u, p_money);
       else
       {
          if((expire>0) && (p_money>0))
          {
             printf("%04d. %12s (%c) %5d �ѨS�W���F�A�x���|���G%5d�T\n",
                ++j, name, l, t, p_money);
          }
          if((p_money>0) || (exp>0))
             if((strcmp(mode, "-g")==0) || (strcmp(mode, "-t")==0))
             {
                total = total + p_money;
                taxtax(name, p_money, exp);
             }
       }
    }
                                                                                  
  }
  fclose(inf);
                                                                                  
  t=0;
  inf = fopen(taxfile, "r");  /* �|���O����  �ֿn�|������ -> ��w */
                                                                                  
  if (inf != NULL)
  {
     fscanf(inf,"%d",&t);
     fclose(inf);
  }
  t = t + total;
                                                                                  
  inf = fopen(taxfile,"w+");
  fprintf(inf,"%d",t);
  fclose(inf);
                                                                                  
  return 1;
}


