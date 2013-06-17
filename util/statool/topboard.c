/*-------------------------------------------------------*/
/* util/topboard.c        ( WDBBS Ver 0.6 )              */
/*-------------------------------------------------------*/
/* target : �ݪO�ϥβv�Ʀ�](��g��Ptt��topboard)        */
/*          ps.�אּŪ�� .BOARDS �ӫD usboard             */
/* create : 98/09/24                                     */
/* update : 98/09/26                                     */
/*-------------------------------------------------------*/

#include "bbs.h"
#define MAXBOARDS 512

typedef
struct boardrec
{
  char bid[20];
  long int time;
  long int user;
}
boardrec;

struct boardrec allboard[MAXBOARDS];
unsigned long int totaltime,totaluser;
boardheader aboard;
int num;
FILE *fp;

int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp1;
  int rc = 0;

  if (fp1 = fopen(filelist, "r"))
  {
    char buf1[150], *ptr;
    while (fgets(buf1, 150, fp1))
    {
      if (((ptr  = (char *) strtok(buf1," \t\n\r")) !=NULL ) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp1);
  }
  return rc;
}

int
stay_cmp(b, a)
 boardrec *a, *b;
{
 double temp= (a->time-b->time)*10000./ totaltime +
          (a->user-b->user)*10000./totaluser;
 if (temp>0) return 1;
 else if (temp<0) return -1;
 return 0;
}

void
top()
{
  char buf[200],board[30],id[20];
  int i = 0,rows = num;
  float a,b;

  if(!fp) return;
  fprintf(fp,"[1;46m�ƦW[44;37m  ��       ��     �ɶ�[32m(��)[31m[�����v][37m      �H�� \
[31m[�����v] [33m     �`��  [m\n");
  for (i = 0; i < rows; i++)
  {
    a=(float)allboard[i].time * 100 / totaltime;
    b=(float)allboard[i].user *100 /totaluser;
    fprintf(fp,"%3d.[1;36m%13s [32m%10d [%02.3f] %10d [%02.3f]    [1;33m   %2.2f[0m\n",i+1,
    allboard[i].bid,allboard[i].time,a,allboard[i].user,b,a+b +0.005);
  } 
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
    num = 100;

  inf = fopen(BBSHOME "/.BOARDS", "r");

  if (inf == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    exit(0);
  }

  for (i = 0; fread(&aboard, sizeof(boardheader), 1, inf); i++)
  {
    if(aboard.brdattr & BRD_NOCOUNT || !aboard.brdname[0])
        i--;
   else
    {
       strcpy(allboard[i].bid,aboard.brdname);
       allboard[i].time = aboard.totaltime;
       allboard[i].user = aboard.totalvisit;
       totaltime += allboard[i].time;
       totaluser += allboard[i].user;
    }
  }

  if ((fp = fopen(argv[2], "w")) == NULL)
  {
    printf("cann't open topboard\n");
    return 0;
  }

  qsort(allboard, i, sizeof(boardrec), stay_cmp);
  top();

  fclose(inf);
  fclose(fp);
}
