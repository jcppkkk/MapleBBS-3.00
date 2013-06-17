/*-------------------------------------------------------*/
/* mn.c    ( WD-BBS Ver 2.3 )				 */
/*-------------------------------------------------------*/
/* author : wildcat@wd.twbbs.org			 */
/* target : �O�b�� , �O���ͬ��������J��X		 */
/* create : 99/12/18                                     */
/* update :   /  /                                       */
/*-------------------------------------------------------*/

#define	FN_MN	".MN"	// MoneyNote file

#include "bbs.h"

static char genbuf[256];

static void
add_mn()
{
  int i = 5,mode;
  MN add;

  memset(&add, 0, sizeof(MN));
  stand_title("�s�W���");
  getdata(i++ , 0 , " (1)���J   (2)��X       : ", genbuf , 2 , DOECHO , 0);
  mode = atoi(genbuf)-1;
  if(mode) add.flag = MODE_OUT;
  else add.flag = MODE_IN;
  getdata(i++ , 0 , "�ɶ� (�~�� �褸 xxxx   ) : ", genbuf , 5 , DOECHO , 0);
  add.year = atoi(genbuf);
  while(add.month <= 0 || add.month > 12)
  {
    getdata(i , 0 , "     (��� 01 - 12     ) : ", genbuf , 3 , DOECHO , 0);
    add.month = atoi(genbuf);
  }
  i++;
  while(add.day <= 0 || add.day > 31)
  {
    getdata(i , 0 , "     (��� 01 - 31     ) : ", genbuf , 3 , DOECHO , 0);
    add.day = atoi(genbuf);
  }
  i++;
  getdata(i++ , 0 , "             ��  �B      : ", genbuf , 8 , DOECHO , 0);
  add.money = atoi(genbuf);
  if(add.flag == MODE_OUT)
  {
    move(i++,0);
    outs("���O : 1.��  2.��  3.��  4.��");
    getdata(i++ , 0 , "       5.�|  6.��  7.��L: ", genbuf , 2 , DOECHO , 0);
    add.use_way = atoi(genbuf)-1;
  }
  getdata(i++ , 0 , "             ��  ��      : ", add.desc , 51 , DOECHO , 0);

  setuserfile(genbuf, FN_MN);
  rec_add(genbuf, &add, sizeof(MN));
  pressanykey("�s�W����");
  return;
}  

static void
del_mn()
{
  int i;

  getdata(b_lines, 0, "�n�R���ĴX�����", genbuf, 3, DOECHO, 0);
  i = atoi(genbuf);

  setuserfile(genbuf, FN_MN);
  if(dashs(genbuf)/sizeof(MN) < i)
  {
    pressanykey("�A�d���o , �S���o�����");
    return;
  }
  rec_del(genbuf, sizeof(MN), i);
}

static void
count_mn()
{
  int fd,totalin=0,totalout=0,i=0,j=0,
      way1=0,way2=0,way3=0,way4=0,way5=0,way6=0,way7=0;
  MN mncount;

  setuserfile(genbuf, FN_MN);
//  i = dashs(genbuf)/sizeof(MN);
  if ( (i = dashs(genbuf)) != -1 )
    i /= sizeof(MN);
  
  for(j=0;j<=i;j++)
  {
    rec_get(genbuf,&mncount,sizeof(mncount),j);  
    if(mncount.flag == MODE_OUT) 
    {
      totalout += mncount.money;
      switch(mncount.use_way)
      {
        case 0:
          way1+=mncount.money;
          break;
        case 1:
          way2+=mncount.money;
          break;
        case 2:
          way3+=mncount.money;
          break;
        case 3:
          way4+=mncount.money;
          break;
        case 4:
          way5+=mncount.money;
          break;
        case 5:
          way6+=mncount.money;
          break;
        default:
          way7+=mncount.money;
          break;
      };
    }
    else 
      totalin += mncount.money;
    close(fd);
  }
  clear();
  prints("\
�ثe[1;31m�`���J[m : [1;33m%-12d[m ��
�ثe[1;32m�`��X[m : [1;33m%-12d[m ��

��b[1;36m[��] %-12d ��[m    [1;32m[��] %-12d ��[m
    [1;31m[��] %-12d ��[m    [1;33m[��] %-12d ��[m
    [1;35m[�|] %-12d ��[m    [1;37m[��] %-12d ��[m
    [1;34m��L %-12d ��[m",
    totalin,totalout,way1,way2,way3,way4,way5,way6,way7);
  pressanykey(NULL);
}

static void
load_mn(file,page)
  char *file;
  int page;
{
  int totalin, totalout , totalpage;
  int i,j;
  char *way[] = {"��", "��", "��", "��", "�|", "��", "��L",NULL};
  MN show;

  if(dashs(file) > 0)
  {
    i = (dashs(file)/sizeof(MN)) - (page*15);
    totalpage = i/15;
    totalin = totalout = 0;

    for(j=1;j<=i;j++)
    {    
      rec_get(file, &show , sizeof(MN), j+(page*15));
      move(j+5,0);
      if(show.use_way > 6) show.use_way = 6;
      prints("%02d. %-4d/%02d/%02d  %s    %-8d    %4s     %-35.35s\n",
        j+(page*15), show.year, show.month, show.day, 
        show.flag == MODE_OUT ? "��X" : "���J", show.money, 
        show.flag == MODE_OUT ? way[show.use_way] : "    ",
        show.desc);
      if(show.flag == MODE_OUT) totalout += show.money;
      else totalin += show.money;
      if(j >= 15)break;
    }
  }
  else return;
  move(b_lines - 2,0);
  prints(COLOR1"[1m (%02d/%02d��)�������J %10d �� , ��X %10d �� , �`�p %10d ��    [m",
    page+1,totalpage+1,totalin , totalout, totalin-totalout);
}

int op = 0;

void
show_mn()
{
  clear();
  showtitle("�O�b��", BoardName);
  setutmpmode(NoteMoney);
  log_usies("MN",NULL);
  show_file(BBSHOME"/etc/mn_title",1,5,NO_RELOAD);

  setuserfile(genbuf, FN_MN);
  load_mn(genbuf,op);

  getdata(b_lines - 1, 0, 
    "�п�J�z����� : (c)���� 1.�s�W  2.�R��  3.���R  4.�`�p  Q.���} : ", genbuf, 2, DOECHO, 0);
  switch(genbuf[0])
  {
    case 'c':
      getdata(b_lines, 0, "�n��ĴX�� : ", genbuf, 3, DOECHO, 0);
      op = atoi(genbuf)-1;
      break;
    
    case '1':
      add_mn();
      break;
    case '2':
      del_mn();
      break;
    case '3':
      if(answer("�O�_�T�w (y/N)") == 'y')
      {
        setuserfile(genbuf, FN_MN);
        unlink(genbuf);
      }
      break;
    case '4':
      count_mn();
      break;
    case 'q':
    case 'Q':
      return;
    default :
    break;
  }
  show_mn();
}
