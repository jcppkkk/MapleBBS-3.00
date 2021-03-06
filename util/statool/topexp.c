/*-------------------------------------------------------*/
/* util/topusr.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 使用者 上站記錄/文章篇數 排行榜              */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"
#define REAL_INFO

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  unsigned long int exp;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec aman;
int num;
FILE *fp;

int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp))
    {
      if ((ptr = strtok(buf, " \t\n\r")) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
  }
  return rc;
}

int
post_cmp(b, a)
  struct manrec *a, *b;
{
  return (a->exp - b->exp);
}

void
top()
{
//  int i, j, rows = (num + 1) / 2;
  int i, j, rows = num , q=0 ;
  char buf1[80], buf2[80];
  char *explv[14] = {"任人使喚的奴隸" , "三餐\不繼的乞丐" 
      , "打拼賺錢的平民" , "熱血沸騰的鬥士" , "勇往直前的士兵"
      , "沙場鬼神的戰士" , "驍勇善戰的將軍" , "富計智囊的謀士" 
      , "一城之主的太守" , "神機妙算的軍師" , "一方之霸的州牧" 
      , "威震遠疆的元帥" , "萬人之上的丞相" , "至尊無上的皇帝" };

//  fprintf(fp, "\
//[1;33m○──────────┬→ [44;33m   經   驗   值 排行   [33;40m ←┬──────────○[m\n\
//[1;37m名次[33m─[37m代號[33m───[37m暱稱[33m─┴─────[37m數目   名次[33m─[37m代號[33m─┴─[37m暱稱[33m──────[37m數目[m\
//");
  fprintf(fp, "\
[1;33m○──────────┬→ [41m     經驗值 排行榜     [40m ←┬──────────○[m
[1;33m○──────────┴─────── ───────┴──────────○[m
[1;37;41m名次 [44m 階          級   [37mUSERID       [37m 暱    稱           [37m經  驗  值           [m");
  
  for (i = 0; i < rows; i++)
  {
    if (allman[i].exp < 50)              q=0;
    else if (allman[i].exp < 100)        q=1;
    else if (allman[i].exp < 300)        q=2;
    else if (allman[i].exp < 600)        q=3;
    else if (allman[i].exp < 1000)       q=4;
    else if (allman[i].exp < 1500)       q=5;
    else if (allman[i].exp < 3000)       q=6;
    else if (allman[i].exp < 5000)       q=7;
    else if (allman[i].exp < 9000)       q=8;
    else if (allman[i].exp < 15000)      q=9;
    else if (allman[i].exp < 30000)      q=10;
    else if (allman[i].exp < 60000)      q=11;
    else if (allman[i].exp < 100000)     q=12;
    else
      q=13;
             
    sprintf(buf1, "%3d [[1;36m%s[m]  [1;35m%-13.13s [m%-16.16s  %5d ",
      i + 1,explv[q], allman[i].userid, allman[i].username,allman[i].exp);
//    j = i + rows;
//    sprintf(buf2, "[%2d] %-11.11s%-16.16s%4d ",
//      j + 1, allman[j].userid, allman[j].username,allman[j].exp);
//    if (i < 3)
//      fprintf(fp, "\n [1;%dm%-40s[0;37m%s", 31 + i, buf1, buf2);
//    if (q > 12)
//      fprintf(fp, "\n [1;%dm%-40s[0;37m", 20 + q, buf1);
//    else if (q > 11)
//      fprintf(fp, "\n %-40s%s", buf1, buf2);
//      fprintf(fp, "\n [1;%dm%-40s[0;37m", 20 + q, buf1);
//    else if (q>10)
//      fprintf(fp, "\n [1;%dm%-40s[0;37m", 25 + q, buf1);
//    else
      fprintf(fp, "\n %-40s", buf1);
  }
}


#ifdef  HAVE_TIN
int
post_in_tin(char *name)
{
  char buf[256];
  FILE *fh;
  int counter = 0;
  sprintf(buf, "%s/home/%s/.tin/posted", BBSHOME, name);
  fh = fopen(buf, "r");
  if (fh == NULL)
    return 0;
  else
  {
    while (fgets(buf, 255, fh) != NULL)
      counter++;
    fclose(fh);
    return counter;
  }
}
#endif                          /* HAVE_TIN */
int
not_alpha(ch)
  register char ch;
{
  return (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z');
}

int
not_alnum(ch)
  register char ch;
{
  return (ch < '0' || (ch > '9' && ch < 'A') ||
    (ch > 'Z' && ch < 'a') || ch > 'z');
}

int
bad_user_id(userid)
  char *userid;
{
  register char ch;
  if (strlen(userid) < 2)
    return 1;
  if (not_alpha(*userid))
    return 1;
  while (ch = *(++userid))
  {
    if (not_alnum(ch))
      return 1;
  }
  return 0;
}

main(argc, argv)
  int argc;
  char **argv;
{
  FILE *inf;
  int i;

  if (argc < 3)
  {
    printf("Usage: %s <num_top> <out-file>\n", argv[0]);
    exit(1);
  }

  num = atoi(argv[1]);
  if (num == 0)
    num = 30;

  inf = fopen(BBSHOME "/.PASSWDS", "rb");

  if (inf == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    exit(0);
  }

  for (i = 0; fread(&aman, sizeof(userec), 1, inf); i++)
  {
   if(belong("etc/nontop",aman.userid) || bad_user_id(aman.userid)
       || strchr(aman.userid,'.'))
   {
     i--;
   }
   else
   {
     strcpy(allman[i].userid, aman.userid);
     strncpy(allman[i].username, aman.username,23);
     allman[i].exp = aman.exp;
   }
  }

  if ((fp = fopen(argv[2], "w")) == NULL)
  {
    printf("cann't open toppost\n");
    return 0;
  }
  qsort(allman, i, sizeof(manrec), post_cmp);
  top();
  fclose(inf);
  fclose(fp);
}
