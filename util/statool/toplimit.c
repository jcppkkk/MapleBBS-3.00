/*-------------------------------------------------------*/
/* util/topusr.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : �ϥΪ� �W���O��/�峹�g�� �Ʀ�]              */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"
#define REAL_INFO

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  unsigned long int limit;
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
limit_cmp(b, a)
  struct manrec *a, *b;
{
  return (a->limit - b->limit);
}


void
top()
{
  int i, j, rows = (num + 1) / 2;
  char buf1[80], buf2[80];

  fprintf(fp, "\
[1;33m���w�w�w�w�w�w�w�w�w�w�s�� [44;33m       �ȹ��Ʀ�]      [33;40m ���s�w�w�w�w�w�w�w�w�w�w��[m\n\
[1;37m�W��[33m�w[37m�N��[33m�w�w�w[37m�ʺ�[33m�w�r�w�w�w�w�w[37m�]��   �W��[33m�w[37m�N��[33m�w�r�w[37m�ʺ�[33m�w�w�w�w�w�w[37m�]��[m\
");
  for (i = 0; i < rows; i++)
  {
    sprintf(buf1, "[%2d] %-11.11s%-16.16s%5d%c",
      i + 1, allman[i].userid, allman[i].username,
      allman[i].limit > 100000 ? allman[i].limit/1000 : allman[i].limit,
      (allman[i].limit > 100000) ? 'K' : ' ');
    j = i + rows;
    sprintf(buf2, "[%2d] %-11.11s%-16.16s%4d%c",
      j + 1, allman[j].userid, allman[j].username,
      allman[j].limit > 100000 ? allman[j].limit/1000 : allman[j].limit,
      (allman[j].limit > 100000) ? 'K' : ' ');
    if (i < 3)
      fprintf(fp, "\n [1;%dm%-40s[0;37m%s", 31 + i, buf1, buf2);
    else
      fprintf(fp, "\n %-40s%s", buf1, buf2);
  }
}
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
     allman[i].limit = (aman.totaltime*10) + (aman.numlogins*100) + (aman.numposts*1000);

    }
  }

  if ((fp = fopen(argv[2], "w")) == NULL)
  {
    printf("cann't open toplimit\n");
    return 0;
  }

  qsort(allman, i, sizeof(manrec), limit_cmp);
  top();

  fclose(inf);
  fclose(fp);
}
