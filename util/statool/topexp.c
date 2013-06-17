/*-------------------------------------------------------*/
/* util/topusr.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : ¨Ï¥ÎªÌ ¤W¯¸°O¿ý/¤å³¹½g¼Æ ±Æ¦æº]              */
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
  char *explv[14] = {"¥ô¤H¨Ï³êªº¥£Áõ" , "¤TÀ\\¤£Ä~ªº¤^¤¢" 
      , "¥´«÷ÁÈ¿úªº¥­¥Á" , "¼ö¦åªmÄËªº°«¤h" , "«i©¹ª½«eªº¤h§L"
      , "¨F³õ°­¯«ªº¾Ô¤h" , "Å»«iµ½¾Ôªº±N­x" , "´I­p´¼Ånªº¿Ñ¤h" 
      , "¤@«°¤§¥Dªº¤Ó¦u" , "¯«¾÷§®ºâªº­x®v" , "¤@¤è¤§ÅQªº¦{ªª" 
      , "«Â¾_»·Ã¦ªº¤¸«Ó" , "¸U¤H¤§¤Wªº¥à¬Û" , "¦Ü´LµL¤Wªº¬Ó«Ò" };

//  fprintf(fp, "\
//[1;33m¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢s¡÷ [44;33m   ¸g   Åç   ­È ±Æ¦æ   [33;40m ¡ö¢s¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³[m\n\
//[1;37m¦W¦¸[33m¢w[37m¥N¸¹[33m¢w¢w¢w[37m¼ÊºÙ[33m¢w¢r¢w¢w¢w¢w¢w[37m¼Æ¥Ø   ¦W¦¸[33m¢w[37m¥N¸¹[33m¢w¢r¢w[37m¼ÊºÙ[33m¢w¢w¢w¢w¢w¢w[37m¼Æ¥Ø[m\
//");
  fprintf(fp, "\
[1;33m¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢s¡÷ [41m     ¸gÅç­È ±Æ¦æº]     [40m ¡ö¢s¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³[m
[1;33m¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢r¢w¢w¢w¢w¢w¢w¢w ¢w¢w¢w¢w¢w¢w¢w¢r¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³[m
[1;37;41m¦W¦¸ [44m ¶¥          ¯Å   [37mUSERID       [37m ¼Ê    ºÙ           [37m¸g  Åç  ­È           [m");
  
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
