/*-------------------------------------------------------*/
/* util/topusr2.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : �ϥΪ� �W���O��/�峹�g�� �Ʀ�]              */
/* create : 97/03/27                                     */
/* update : 95/03/31                                     */
/*-------------------------------------------------------*/

#undef MONTH
#define REAL_INFO
#undef HIDE
#define ADMIN_REPORT
#define BUFSIZE 256

#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"
#include "record.c"
int mannum;

struct binfo
{
  char  boardname[18];
  char  expname[28];
  int times;
  int sum;
  int post;
  int num;
  int del;
  int pur;
  int tag;
} st[MAXBOARD];

int numboards=0;
int numgroups=0;
int numhide=0;
int numhideg=0;

int
rec_apply(fpath, fptr, size)
  char *fpath;
  int (*fptr) ();
int size;
{
  char abuf[BUFSIZE];
  FILE* fp;

  if (!(fp = fopen(fpath, "r")))
    return -1;

  while (fread(abuf, 1, size, fp) == size)
     if ((*fptr) (abuf) == QUIT) {
        fclose(fp);
        return QUIT;
     }
  fclose(fp);
  return 0;
}

int
brd_cmp(b, a)
struct binfo *a, *b;
{
    if(a->sum!=b->sum)
            return (a->sum - b->sum);
    return a->times - b->times;
}

/*
char    *Ctime(date)
time_t  *date;
{
        static char buf[80];

        strcpy(buf, (char *)ctime(date));
        buf[strlen(buf)-1] = '\0';
        return buf;
}
*/

int
record_data(board,sec)
char *board;
int sec;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].times++;
                        st[i].sum+=sec;
                        return;
                }
        }
        return ;
}

int
record_data2(board)
char *board;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].post++;
                        return;
                }
        }
        return ;
}

int
record_data3(board)
char *board;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].del++;
                        st[i].num++;
                        return;
                }
        }
        return ;
}

int
record_data4(board,sec)
char *board;
int sec;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].pur++;
                        st[i].num+=sec;
                        return;
                }
        }
        return ;
}

int
record_data5(board,sec)
char *board;
int sec;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].tag++;
                        st[i].num+=sec;
                        return;
                }
        }
        return ;
}






int
fillbcache(fptr)
    struct boardheader *fptr ;
{

    if( numboards >= MAXBOARD )
        return 0;

    /* Gene */
#ifdef HIDE
    if((fptr->level != 0) && !(fptr->level & PERM_POSTMASK))
        return;
#endif
    if((fptr->level != 0) && !(fptr->level & PERM_POSTMASK))
        numhide++;
    if ((strstr(fptr->title, "�["))||(strstr(fptr->title, "�U"))||
         (strstr(fptr->title, "�S")))
    {
    numgroups++;
    if((fptr->level != 0) && !(fptr->level & PERM_POSTMASK))
        numhideg++;
    return;}

    strcpy(st[numboards].boardname,fptr->brdname);
    strcpy(st[numboards].expname,fptr->title);
/*    printf("%s %s\n",st[numboards].boardname,st[numboards].expname); */
    st[numboards].times=0;
    st[numboards].sum=0;
    st[numboards].post=0;
    st[numboards].del=0;
    st[numboards].pur=0;
    st[numboards].num=0;
    st[numboards].tag=0;

    numboards++;
    return 0 ;
}

int
fillboard()
{
  rec_apply( BBSHOME"/.BOARDS", fillbcache, sizeof(boardheader));
}

/*
char *
timetostr(i)
int i;
{
        static char str[30];
        int minute,sec,hour;

        minute=(i/60);
        hour=minute/60;
        minute=minute%60;
        sec=i&60;
        sprintf(str,"%2d:%2d\'%2d\"",hour,minute,sec);
        return str;
}
*/

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  char userid2[IDLEN+1];
  usint userlevel;
  usint userlevel2;
  ushort numlogins;
  ushort numposts;
  ushort numloginsto;
  ushort numpoststo;
  ushort numloginsyes;
  ushort numpostsyes;
  ushort messto;
  ushort messfrom;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec aman;
int num;
FILE *fp,*fp2;



int
record_mess(name,type)
char *name;
int type;
{
  int i;

  for(i=0;i<mannum;i++)
  {
    if (!strcmp(name, allman[i].userid))
    {
      if (type == 0) allman[i].messfrom++;
      else allman[i].messto++;
      return;
    }
  }
  return ;
}


main(argc, argv)
  int argc;
  char **argv;
{
//  int j,k,num
//  FILE *inf3;
  FILE *inf2;
  FILE *inf;
  FILE *fpf;
  int i;
  int n;
  int numlog=0, numlog2=0;
  int numpo=0, numpo2=0;
  int maxlog=0, maxlog2=0;
  int maxpo=0, maxpo2=0;
  char maxlogid[IDLEN+1];
  char maxpoid[IDLEN+1];
  char maxlogid2[IDLEN+1];
  char maxpoid2[IDLEN+1];
  int userlog=0, userlog2=0;
  int userpo=0, userpo2=0;
  time_t now = time(0);
  struct tm *ptime;
//  char *progmode;
  FILE *fp1;
/*  FILE *op; */
  char buf[256], *p,bname[20];
/*  char date[80];
  int mode;
  int c[3]; */
  int max[3];
  unsigned int ave[3];
  int sec;
  char timesbname[20];
  char sumbname[20];
  char uname_from[20];
  char uname_to[20];
  int messnum;
  int max_from=0;
  int max_to=0;
  int user_from=0;
  char setby[13];
  char setto[13];
  int act[27];                  /* ����/�֭p�ɶ�/pointer */
  int user_to=0;
  int hour;
  int newreg=0;
  int numtalk=0;
  int numchat=0;
  int numnewb=0;
  int numnameb=0;
  int numdelb=0;
  int numdated=0;
  int numclean=0;
  int numsetb=0;
  int numkill=0;
  int numsuci=0;
  int numsetu=0;
  int numsetself=0;
  int alltime=0;
  int alltimes=0;
  int allpost=0;
  int allnum=0;
  int alldel=0;
  int allpur=0;
  int alltag=0;
  int maxtoday=0;
  int numsysop=0;
  int numboard=0;
  int numaccount=0;
  int numchatroom=0;
  int numbm=0;
  int numsee=0;
  int numcloak=0;
  int numloginok=0;
  int guestnum=0;

  setuid(BBSUID);
  setgid(BBSGID);
  chdir(BBSHOME);

#ifdef MONTH
  if ((fp1 = fopen(BBSHOME "/adm/board.read", "r")) == NULL)
#else
  if ((fp1 = fopen(BBSHOME"/usboard", "r")) == NULL)
#endif
  {
    printf("cann't open usboard\n");
    /* return 1 */;
  }

  fillboard();
  while (fgets(buf, 256, fp1))
  {
    if ( !strncmp(buf, "USE", 3))
    {
      p=strstr(buf,"USE");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    if ( p = (char *)strstr(buf+25, "Stay: "))
    {
      sec=atoi( p + 6);
    }
    else
        sec=0;
    record_data(bname,sec);
    }
    if ( !strncmp(buf, "DEL", 3))
    {
      p=strstr(buf,"DEL");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    record_data3(bname);
    }

    if ( !strncmp(buf, "PUR", 3))
    {
      p=strstr(buf,"PUR");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    if ( p = (char *)strstr(buf+25, "with: "))
    {
      sec=atoi( p + 6);
    }
    else
        sec=0;
    record_data4(bname,sec);
    }
    if ( !strncmp(buf, "TAG", 3))
    {
      p=strstr(buf,"TAG");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    if ( p = (char *)strstr(buf+25, "with: "))
    {
      sec=atoi( p + 6);
    }
    else
        sec=0;
    record_data5(bname,sec);
    }


    if ( !strncmp(buf, "POS", 3))
    {
      p=strstr(buf,"POS");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
      record_data2(bname);
    }


   }
   fclose(fp1);
   
   /* qsort */
   ave[0]=0;
   ave[1]=0;
   ave[2]=0;
   max[1]=0;
   max[0]=0;
   max[2]=0;
   for(i=0;i<numboards;i++)
   {
        ave[0]+=st[i].times;
        ave[1]+=st[i].sum;
        ave[2]+=st[i].times==0?0:st[i].sum/st[i].times;
        if(max[0]<st[i].times)
        {
                max[0]=st[i].times;
                strcpy(timesbname, st[i].boardname);
        }
        if(max[1]<st[i].sum)
        {
                max[1]=st[i].sum;
                strcpy(sumbname, st[i].boardname);
        }
        if(max[2]<(st[i].times==0?0:st[i].sum/st[i].times))
        {
                max[2]=(st[i].times==0?0:st[i].sum/st[i].times);
        }
        alltime+=st[i].sum;
        alltimes+=st[i].times;
        alldel+=st[i].del;
        allpur+=st[i].pur;
        alltag+=st[i].tag;
        allpost+=st[i].post;
        allnum+=st[i].num;
   }
   numboards++;
   st[numboards-1].times=ave[0]/numboards;
   st[numboards-1].sum=ave[1]/numboards;
   strcpy(st[numboards-1].boardname,"Total");
   strcpy(st[numboards-1].expname,"�`�X");
   qsort(st, numboards, sizeof( st[0] ), brd_cmp);


  now = time(NULL);
  ptime = localtime(&now);

#ifdef MONTH
  inf = fopen(BBSHOME "/.PASSWDS.yes", "rb");
#else
  inf = fopen(BBSHOME "/.PASSWDS", "rb");
#endif

  if (inf == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    /* exit(0) */;
  }

  for (i = 0; fread(&aman, sizeof(userec), 1, inf); i++)
  {
    strcpy(allman[i].userid, aman.userid);
    strncpy(allman[i].username, aman.username,23);
    allman[i].numloginsto = aman.numlogins;
    allman[i].numpoststo = aman.numposts;
    allman[i].userlevel = aman.userlevel;

#ifdef  HAVE_TIN
    allman[i].numposts += post_in_tin(allman[i].userid);
#endif
  }
  fclose (inf);

#ifdef MONTH
  inf2 = fopen(BBSHOME "/.PASSWDS.month", "rb");
#else
  inf2 = fopen(BBSHOME "/.PASSWDS.yes", "rb");
#endif
  if (inf2 == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    /* exit(0) */;
  }

  for (i = 0; fread(&aman, sizeof(userec), 1, inf2); i++)
  {
    strcpy(allman[i].userid2, aman.userid);
    allman[i].numloginsyes = aman.numlogins;
    allman[i].numpostsyes = aman.numposts;
    allman[i].userlevel2 = aman.userlevel;

#ifdef  HAVE_TIN
    allman[i].numposts += post_in_tin(allman[i].userid);
#endif
  }
  fclose(inf2);
  
  n=i-1;
  mannum=n;

  for (i = 0; i<=n; i++)
  {
    if (!strcmp(allman[i].userid, allman[i].userid2))
    {
      allman[i].numlogins = allman[i].numloginsto - allman[i].numloginsyes;
      allman[i].numposts = allman[i].numpoststo - allman[i].numpostsyes;
    }
    else
    {
      allman[i].numlogins = allman[i].numloginsto;
      allman[i].numposts = allman[i].numpoststo;
    }
    if (allman[i].numpoststo < allman[i].numpostsyes)
      allman[i].numposts = 0;
    if (allman[i].numloginsto < allman[i].numloginsyes)
      allman[i].numlogins = 0;
  }

#ifdef MONTH
    if ((fpf = fopen(BBSHOME "/adm/usies", "r")) == NULL)
#else
    if ((fpf = fopen(BBSHOME "/usies", "r")) == NULL)
#endif
  {
    printf("cann't open usies\n");
    /* return 1 */;
  }

    while (fgets(buf, 256, fpf))
  {
    hour = atoi(buf + 9);
    if (hour < 0 || hour > 23)
    {
      continue;
    }
    if (!(strncmp(buf +28 , "guest ", 6)))
      {
      guestnum++;
      }

    if (!(strncmp(buf +22, "APPLY", 5)))
      {
      newreg++;
      continue;
      }
    if (!(strncmp(buf +22, "DATED", 5)))
      {
      numdated++;
      continue;
      }
    if (!(strncmp(buf +22, "CLEAN", 5)))
      {
      numclean++;
      continue;
      }
    if (!(strncmp(buf +22, "SUCI", 4)))
      {
      numsuci++;
      continue;
      }
    if (!(strncmp(buf +22, "KILL", 4)))
      {
      numkill++;
      continue;
      }
    if (!(strncmp(buf +22, "NewBoard", 8)))
      {
      numnewb++;
      continue;
      }
    if (!(strncmp(buf +22, "DelBoard", 8)))
      {
      numdelb++;
      continue;
      }
    if (!(strncmp(buf +22, "SetBoard", 8)))
      {
      numsetb++;
      continue;
      }
    if (!(strncmp(buf +22, "NameBoard", 9)))
      {
      numnameb++;
      continue;
      }


    if (!(strncmp(buf +22, "CHAT ", 5)))
      {
      numchat++;
      continue;
      }
    if (!(strncmp(buf +22, "TALK ", 5)))
      {
      numtalk++;
      continue;
      }
    if (!strncmp(buf + 22, "ENTER", 5))
    {
      act[hour]++;
      continue;
    }
    if (!strncmp(buf + 22, "SetUser", 7))
    {
      p=strstr(buf,"SetUser");
      p+=8;
      p=strtok(p," ");
      strcpy(setby,p);
      p=strstr(buf, setby);
      p+=13;
      p=strtok(p," ");
      if (strstr(p,"\n"))
          p=strtok(p,"\n");
      strcpy(setto,p);
      if (strcmp(setto, setby))
        numsetu++;
      else
        numsetself++;
    }
    if (p = (char *) strstr(buf + 40, "Stay:"))
    {
      if (hour = atoi(p + 5))
      {
        act[24] += hour;
        act[25]++;
      }
      continue;
    }
  }
  fclose(fpf);

  if(fpf = fopen(".maxtoday", "r"))
  {
    fscanf(fpf, "%d", &maxtoday);
    fclose(fpf);
  }

  if ((fp = fopen(BBSHOME "/log/admin.log", "w")) == NULL)
  {
    printf("cann't open admin.log\n");
    /* return 0*/;
  }
  if((fp2 = fopen(BBSHOME "/log/board.log", "w")) == NULL)
  {
    printf("cann't open board.log\n");
    /* return 0*/;
  }

  for(i=0; i<=n; i++)
  {
    numlog+=allman[i].numloginsto;
    numpo+=allman[i].numpoststo;
    numlog2+=allman[i].numlogins;
    numpo2+=allman[i].numposts;
    if (allman[i].numloginsto>0) userlog++;
    if (allman[i].numlogins>0) userlog2++;
    if (allman[i].numpoststo>0) userpo++;
    if (allman[i].numposts>0) userpo2++;
    if (allman[i].numloginsto>maxlog) {
      maxlog=allman[i].numloginsto;
      strcpy(maxlogid, allman[i].userid2);
    }
    if (allman[i].numpoststo>maxpo) {
      maxpo=allman[i].numpoststo;
      strcpy(maxpoid, allman[i].userid2);
    }
    if (allman[i].numlogins>maxlog2) {
      maxlog2=allman[i].numlogins;
      strcpy(maxlogid2, allman[i].userid2);
    }
    if (allman[i].numposts>maxpo2) {
      maxpo2=allman[i].numposts;
      strcpy(maxpoid2, allman[i].userid2);
    }
    if (allman[i].messfrom>0) user_from++;
    if (allman[i].messto>0) user_to++;
    if (allman[i].messfrom>max_from) {
      max_from=allman[i].messfrom;
      strcpy(uname_from, allman[i].userid2);
    }
    if (allman[i].messto>max_to) {
      max_to=allman[i].messto;
      strcpy(uname_to, allman[i].userid2);
    }
    if (allman[i].userlevel & PERM_SYSOP) numsysop++;
    if (allman[i].userlevel & PERM_ACCOUNTS) numaccount++;
    if (allman[i].userlevel & PERM_BOARD) numboard++;
    if (allman[i].userlevel & PERM_CHATROOM) numchatroom++;
    if (allman[i].userlevel & PERM_BM) numbm++;
    if (allman[i].userlevel & PERM_SEECLOAK) numsee++;
    if (allman[i].userlevel & PERM_CLOAK) numcloak++;
    if (allman[i].userlevel & PERM_LOGINOK) numloginok++;
  }

  fprintf(fp, "    [1;31m%s[m %s ���i\n",
     BOARDNAME,Ctime(&now));
  fprintf(fp, "\n");
  fprintf(fp, "    ���� �w�� [1;33m%10d[m �H�W���L [1;33m%10d[m ��, �����C�H [1;33m%5d[m ��\n",
     userlog, numlog, numlog/userlog);
  fprintf(fp, "    ���� �w�� [1;33m%10d[m �H�o��L [1;33m%10d[m �g, �����C�H [1;33m%5d[m �g\n\n",
     userpo, numpo, numpo/userlog);
  fprintf(fp, "    ���� �w�� [1;33m%10d[m �H�W���L [1;33m%10d[m ��, �����C�H [1;33m%5d[m ��\n",
     userlog2, numlog2, numlog2/userlog2);
  fprintf(fp, "    ���� �w�� [1;33m%10d[m �H�o��L [1;33m%10d[m �g, �����C�H [1;33m%5d[m �g\n\n",
     userpo2, numpo2, numpo2/userlog2);
  fprintf(fp, "    ���� �W�� �̦h�����H�O [1;33m%13s[m �� [1;33m%10d[m ��\n",
     maxlogid2, maxlog2);
  fprintf(fp, "    ���� �o�� �̦h�����H�O [1;33m%13s[m �� [1;33m%10d[m �g\n\n",
     maxpoid2, maxpo2);
  fprintf(fp, "    ���� Ū�� [1;33m%8d[m �� �@ [1;33m%8d[m ��"
             " �����C�� [1;33m%5d[m �H�� �@ [1;33m%5d[m �� \n",
     ave[0], ave[1]/60, ave[0]/numboards, ave[1]/(numboards*60));
  fprintf(fp, "    ���� Ū�� ���Ƴ̰��O [1;33m%-13s[m �� �@ [1;33m%5d[m �� �@�목�ӼƬ� [1;33m%5d[m �� \n"
              "    ���� Ū�� �ɶ��̰��O [1;33m%-13s[m �� �@ [1;33m%5d[m �� �@��s�ռƬ� [1;33m%5d[m ��\n\n",
     timesbname, max[0], numboards-1, sumbname, max[1]/60, numgroups);
  fprintf(fp, "    ���� �`�@�� [1;33m%6d[m �ӰT�� �䤤 �� [1;33m%5d[m �ӤH�o �� [1;33m%5d[m �ӤH��\n"
              "    �o�̦h���O [1;33m%13s[m �� [1;33m%4d[m ��"
              " ���̦h���O [1;33m%13s[m �� [1;33m%4d[m ��\n\n",
     messnum, user_from, user_to, uname_from, max_from, uname_to, max_to);
  fprintf(fp, "    ���� �� [1;33m%5d[m �ӤH���U  �� [1;33m%5d[m �� guest �W�ӹL"
              " ������F [1;33m%8d[m ����\n"
              "    ���� �̰��� [1;33m%5d[m �P�ɤW�� ������ [1;33m%5d[m �H�W��",
     newreg, /* act[25]-numlog2 */  guestnum , act[24], maxtoday, act[24]/1440);
  fprintf(fp,"\n    ���� �� [1;33m%5d[m �� TALK �� [1;33m%5d[m �ӤH CHAT\n",
      numtalk, numchat);
  fprintf(fp, "    ���� �� [1;33m%5d[m �� id �Q�� �� [1;33m%5d[m �L��"
              " �� [1;33m%5d[m �Q�M �� [1;33m%5d[m �۱�\n",
     numkill, numdated, numclean, numsuci);
  fprintf(fp, "    ���� �� [1;33m%5d[m �ӤH���� �� [1;33m%5d[m �ӤH�Q����\n",
     numsetself, numsetu);

  fprintf(fp, "\n    �� [1;33m%5d[m �� ����� �� �� [1;33m%5d[m �� ����� �s�� \n",
      numhide-numhideg, numhideg);

  fprintf(fp, "\n    ���� �} [1;33m%5d[m �Ӫ� �� [1;33m%5d[m �Ӫ� "
              "�]�w [1;33m%5d[m �Ӫ� �R�W [1;33m%5d[m �Ӫ�\n",
      numnewb, numdelb, numsetb, numnameb);

  fprintf(fp, "\n    ������ [1;33m%3d[m �H, �b���`�ަ� [1;33m%3d[m �H, "
              "�ݪ��`�ަ� [1;33m%3d[m �H, ��ѫ��`�ަ� [1;33m%3d[m �H\n",
    numsysop, numaccount, numboard, numchatroom);
  fprintf(fp, "    ���D�� [1;33m%3d[m �H, �ݨ��Ԫ̦� [1;33m%3d[m �H, "
              "�������N�� [1;33m%3d[m �H, �������U�� [1;33m%5d[m �H\n",
    numbm, numsee, numcloak, numloginok);

/*------- wildcat : ������ɬ��� -------*/

  fprintf(fp2, "==>[1;32m �ݪO���p���i [33m%s[m\n",Ctime(&now));
  fprintf(fp2,"����:�ɶ�->���d�ɶ�(��) �H��->�i�O�H�� �R��->�Q�R���g��");

  fprintf(fp2,"\n[1;37;42m�W�� %-15.15s%-28.28s %6s %4s %4s %3s %3s %3s[m\n",
                "�Q�װϦW��","����ԭz","  �ɶ�","�H��","�R��","POST","TAG","DEL");

 for(i=0;i<MAXBOARD;i++)
 {
   if(st[i].sum)
     fprintf(fp2,"[1;33m%4d[m %-15.15s%-28.28s [1;32m%6d [31m%4d [m%4d [1;36m%3d[m %3d %3d\n",
     i+1,st[i].boardname,st[i].expname,st[i].sum,st[i].times,st[i].num
        , st[i].post,st[i].tag,st[i].del);
 }
     fprintf(fp2,"[1;37;42m     %-15.15s%-28.28s %6d %4d %4d %3d %3d %3d\n",
     "Total","�`�X",alltime,alltimes,allnum,allpost,alltag,alldel);

  printf("numlog = %d\n", numlog);
  printf("numlog2 = %d\n", numlog2);
  printf("numpo = %d\n", numpo);
  printf("numpo2 = %d\n", numpo2);
  printf("userlog = %d\n", userlog);
  printf("userlog2 = %d\n", userlog2);
  printf("userpo = %d\n", userpo);
  printf("userpo2 = %d\n", userpo2);
  printf("Maxpost %s = %d\n", maxpoid, maxpo);
  printf("Maxlogin %s = %d\n", maxlogid, maxlog);
  printf("Maxpost2 %s = %d\n", maxpoid2, maxpo2);
  printf("Maxlogin2 %s = %d\n", maxlogid2, maxlog2);
  fclose(fp);
  fclose(fp2);
}
