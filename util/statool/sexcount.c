/*
 *�o�H�H: wsyfish.bbs@fpg.m4.ntu.edu.tw (���ݤ�����������), �ݪO: SobVersion
 *��  �D: [util]���W�P�y�έp horoscope.c
 *�o�H��: �p���������� (Fri Sep  5 12:52:13 1997)
 *��H��: sob!news.civil.ncku!fpg
 */

/*-------------------------------------------------------*/
/* util/sex.c           ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : ���W�ʧO�έp                                 */
/* create : 99/01/06                                     */
/* update : 99/01/06                                     */
/*-------------------------------------------------------*/

/* ���{���� wildcat (wildcat.bbs@wd.twbbs.org)
   �Ѧ� horoscope.c �ק粒�� */

#include "bbs.h"

#define DOTPASSWDS BBSHOME"/.PASSWDS"

struct userec cuser;

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

main()
{
  int i, j;
  FILE *fp;
  int max, item, maxsex,totalman=0;
  time_t now;
  int act[3];
  char *name[4] = {"�k��",
                   "�k��",
                   "����",
                   ""
                           };
  char    *blk[10] =
  {
      "  ","�j", "�k", "�l", "�m",
      "�n","�o", "�p", "�i", "�i",
  };


  now = time(0);
  chdir(BBSHOME);
  fp=fopen(DOTPASSWDS, "r");

  memset(act, 0, sizeof(act));
  while((fread(&cuser, sizeof(cuser), 1, fp)) >0 )
  {
    if(bad_user_id(cuser.userid)) continue;
    switch (cuser.sex)
    {
      case 0:
        act[0]++;
        break;
      case 1:
        act[1]++;
        break;
      case 2:
        act[0]++;
        break;
      case 3:
        act[1]++;
        break;
      case 4:
        act[0]++;
        break;
      case 5:
        act[1]++;
        break;
      case 6:
        act[2]++;
        break;
      case 7:
        act[2]++;
        break;
      default:
        act[2]++;
        break;
    }

  }
  fclose(fp);


  for (i = max  = maxsex = 0; i < 3; i++)
  {
    if (act[i] > max)
    {
      max = act[i];
      maxsex = i;
    }
  }

  item = max / 30 + 1;

  if ((fp = fopen("etc/sexcount", "w")) == NULL)
  {
    printf("cann't open etc/sexcount\n");
    return 1;
  }

  fprintf(fp,"           [1m[42;33m %s [m �ʧO�έp %s\n", BBSNAME,ctime(&now));

  for(i = 0;i < 3;i++)
  {
    fprintf(fp,"\n[1m[47;34m %s [m[32m", name[i]);
    for(j = 0; j < act[i]/item; j++)
    {
      fprintf(fp,"%2s",blk[9]);
    }
    fprintf(fp,"%2s [1;33m%d[m\n\n",blk[(act[i] % item) * 10 / item],
            act[i]);
    totalman += act[i];
  }
  fprintf(fp,"\n\n\n                          [1m[42;33m ���W�`�H�f [m %d �H", totalman);
  fclose(fp);
}
