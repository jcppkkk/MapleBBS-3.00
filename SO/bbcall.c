/*-------------------------------------------------------*/
/* alphacall.c       ( NTHU CS MapleBBS Ver 3.00 )       */
/*-------------------------------------------------------*/
/* author : yshuang@dragon2.net                          */
/* target : BBS �����ǩI 0943/0946 �p�� Alphacall        */
/*                      & 0948 �j���q�H                  */
/* create : 98/12/01                                     */
/* update : 98/12/10                                     */
/*-------------------------------------------------------*/
/* wsyfish ���@�ǥ��n���ק�                              */
/*-------------------------------------------------------*/

#include "bbs.h"

#include <stdlib.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WEBPORT 80
#define PARA    "Connection: Keep-Alive\nUser-Agent: Mozilla/4.5b1 [en] (X11; I; FreeBSD 2.2.7-STABLE i386)\nContent-type: application/x-www-form-urlencoded\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png,
*/*\nAccept-Encoding: gzip\nAccept-Language: en\nAccept-Charset: iso-8859-1,*,utf-8\n"

struct BBCALL_DATA
  {
    int label;          /* 1:�Q�аO */
    char name[13];      //12

    char class[5];      //05

    char birth[6];      //05

    char phone[19];     //18

    char bbcall[21];        //21

    char callpass[11];      //10

    char email[51];     //50

    char address[65];       //64

    char explain[65];       //64=229

  };
typedef struct BBCALL_DATA BBCALL_DATA;

int call_y_position = 13;

char *
Star_date (clock)
     time_t *clock;
{
  static char foo[33];
  static char myweek[] = "�Ѥ@�G�T�|����";
  struct tm *mytm = localtime (clock);
  sprintf (foo, "%04d�~%02d��%02d�� (�P��%c%c) %02d��%02d��",
  mytm->tm_year+1900, mytm->tm_mon + 1, mytm->tm_mday, myweek[mytm->tm_wday << 1],
       myweek[(mytm->tm_wday << 1) + 1], mytm->tm_hour, mytm->tm_min);

  return (foo);
}

int
get_time_screen_word (int y_start, char *Year, char *YearC, char *Month, char *Day, char *Hour, char *Min)
{
  char buf[13];
  char t_max[13];
  char ch;
  time_t input_time;
  time_t nowtime;
  struct tm ptime;
  int y_position = y_start;
  int leave = 0;
  int i;
  int time_return = 0;

  strcpy (t_max, "200019392959");
  do
    {
      y_position = y_start;
      move (y_position, 6);
      time (&nowtime);
      prints ("�{�b���ɶ��O:%s", Star_date (&nowtime));
      y_position++;
      move (y_position, 6);
      prints ("�п�J�w���ǩI���ɶ�(�Х�24�p�ɨ�):    �~  ��  ��  ��  ��");
      for (i = 0; i < 12; i++)
    {
      if (i <= 3)
        move (y_position, (41 + i));
      else
        move (y_position, (47 + (i - 4) / 2 * 4 + (i - 4) % 2));
      ch = igetkey ();
      if (ch == '\r')
        {
          i = -1;
          continue;
        }
      else if (ch == '\177' || ch == Ctrl ('H'))
        {
          if (i)
        i -= 2;
          else
        i = -1;
          continue;
        }
      if (ch >= '0' && ch <= t_max[i])
        {
          outc (ch);
          buf[i] = ch - '0';
        }
      else
        {
          bell ();
          --i;
        }
    }
      ptime.tm_sec = 0;
      ptime.tm_year = (buf[2] * 10 + buf[3]) + 100;
      ptime.tm_mon = buf[4] * 10 + buf[5] - 1;
      ptime.tm_mday = buf[6] * 10 + buf[7];
      ptime.tm_hour = buf[8] * 10 + buf[9];
      ptime.tm_min = buf[10] * 10 + buf[11];
      input_time = mktime (&ptime);
      if (input_time < time (0))
    {
      pressanykey ("�A����J�w�g�L���o..�Э��s��J..");
    }
      else
    {
      leave = 1;
      time_return = 1;
    }
    }
  while (leave == 0);
  //prints("�A�ҿ�J���ɶ��O:[1;33m%s[m",Star_date(&input_time));
  sprintf (Year, "%4d", (buf[0] * 1000 + buf[1] * 100 + buf[2] * 10 + buf[3]));
  sprintf (YearC, "%2d", ptime.tm_year - 11);
  sprintf (Month, "%02d", ptime.tm_mon + 1);
  sprintf (Day, "%02d", ptime.tm_mday);
  sprintf (Hour, "%02d", ptime.tm_hour);
  sprintf (Min, "%02d", ptime.tm_min);
  return time_return;
}


int
bbcall_function (int y, int nummode, int callmode, char *PagerNo, char *PagerName, char *PagerPassword, char *PagerMsg)
{
  char trn[512], result[1024], sendform[512];
  char PagerYear[4], PagerYearC[3], PagerMonth[3], PagerDay[3], PagerHour[3],
    PagerMin[3];
  char PageSender[10];
  char genbuf[200];
  char ask[3];
  char LOG_FILE[100];
  char SERVER_TOOL[100];
  char *PAGER_SERVER[10] =
  {
    "0940��",           //0940
     "www.chips.com.tw",    //0941
     "0942��",          //0942
     "www.pager.com.tw",    //0943
     "0944��",          //0944
     "www.tw0945.com",      //0945
     "www.pager.com.tw",    //0946
     "web1.hoyard.com.tw",  //0947
     "www.fitel.net.tw",    //0948
     "www.southtel.com.tw"  //0949
  };
  /* PAGER_CGI[2n+0]:�Ʀr PAGER_CGI[2n+1]:��r */
  char *PAGER_CGI[20] =
  {
    "��", "��",
    "/cgi-bin/paging1.pl", "/cgi-bin/paging1.pl",
    "��", "��",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "��", "��",
    "/Scripts/fiss/PageForm.exe", "/Scripts/fiss/PageForm.exe",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "/scripts/fp_page1.dll", "/scripts/fp_page1.dll",
    "/cgi-bin/Webpage.dll", "/cgi-bin/Webpage.dll",
    "../pager/Webpg.asp", "../pager/Webpg.asp"};
  /* PAGER_REFER[2n+0]:�Ʀr PAGER_REFER[2n+1]:��r */
  char *PAGER_REFER[20] =
  {
    "��", "��",
    "http://www.chips.com.tw:9100/WEB2P/page_1.htm", "http://www.chips.com.tw:9100/WEB2P/page_1.htm",
    "��", "��",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "��", "��",
    "http://www.tw0945.com/call_numPg.HTM", "http://www.tw0945.com/call_AlphaPg.HTM",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "http://web1.hoyard.com.tw/freeway/freewayi.html#top", "http://web1.hoyard.com.tw/freeway/freewayi.html#top",
    "http://www.fitel.net.tw/html/svc03.htm#top", "http://www.fitel.net.tw/html/svc03.htm#top",
    "http://www.southtel.com.tw/numpg.htm", "http://www.southtel.com.tw/Alphapg.htm"};
  /* PAGER_CHECK[2n+0]:�ߧY PAGER_CHECK[2n+1]:�w�� */
  char *PAGER_CHECK[20] =
  {
    "��", "��",
    "�ǰe��", "�w����",
    "��", "��",
    "���T", "���T",
    "��", "��",
    "SUCCESS.HTM", "SUCCESS.HTM",
    "���T", "���T",
    "�T �� �w �� �e", "�T �� �w �� �e",
    "�ǩI���\\", "�ǩI���\\",
    "SUCCESS.HTM", "SUCCESS.HTM"};

  struct sockaddr_in serv_addr;
  struct hostent *hp;
  int sockfd;
  int timemode = 0;
  int y_position = y;
  int whichone;

  whichone = (nummode - 940) * 2 + callmode;
  getdata (y_position, 6, "[1]�ߧY�I�s [2]�w���ǩI�G", ask, 2, DOECHO, 0);
  y_position++;
  if (ask[0] == '2')
    {
      timemode = 1;
      get_time_screen_word (y_position, PagerYear, PagerYearC, PagerMonth, PagerDay, PagerHour, PagerMin);
      y_position += 2;
    }
  else
    {
      timemode = 0;
      sprintf (PagerYear, "%4d", 1999);
      sprintf (PagerYearC, "%2d", 88);
      sprintf (PagerMonth, "%02d", 1);
      sprintf (PagerDay, "%02d", 1);
      sprintf (PagerHour, "%02d", 0);
      sprintf (PagerMin, "%02d", 0);
    }

  y_position++;
  clrchyiuan (call_y_position - 1, y_position);
  y_position = call_y_position;
  move (y_position, 6);
  prints ("[1;33;5m�T���e�X�e�A�нT�{�H�U����ƬO�_���T�H[0m");
  y_position++;

  move (y_position, 6);
  prints (" �z�ҭn�ǩI�����X�G%s", PagerNo);
  y_position++;
  move (y_position, 6);
  prints (" �z�ҭn�ǩI���T���G%s", PagerMsg);
  y_position++;

  move (y_position, 6);
  sprintf (genbuf, "�ɶ�:%s�~%s��%s��%s��%s���ǩI", PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin);
  prints (" �z�ҭn�ǩI���Ҧ��G[%s] %s", (timemode) ? "�w���ǩI" : "�ߧY�ǩI", (timemode) ? genbuf : " ");
  y_position += 2;

  getdata (y_position, 6, "�z�T�w�n�e�X��? [y/N]", ask, 2, DOECHO, 0);
  if (ask[0] != 'y' && ask[0] != 'Y')
    {
      pressanykey ("��M�Q���ϥνu�W�ǩI");
      return;
    }

  sprintf (LOG_FILE, "log/0%d.log", nummode);
  move (b_lines - 1, 0);
  sprintf (result, "\n\n----�ǰe�����}�l----\n�ϥΪ�: %s ���X: 0%3d%s \n�T��: %s\n�Ҧ�: [%s] %s\n----�ǰe��������----\n",
       cuser.userid, nummode, PagerNo, PagerMsg, (timemode) ? "�w���ǩI" : "�ߧY�ǩI", (timemode) ? genbuf : " ");
  f_cat (LOG_FILE, result);

  genbuf[0] = '\0';
  switch (nummode)
    {
    case 941:
      sprintf (genbuf, "=on&year=%s&month=%s&day=%s&hour=%s&min=%s", PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin);
      sprintf (trn, "PRE_PNO=0941&PAGER_NO=%s&MSG_TYPE=NUMERIC&TRAN_MSG=%s&%s%s",
           PagerNo, PagerMsg, timemode ? "DELAY" : "NOW", timemode ? genbuf : "=on");
      break;
    case 943:
    case 946:
      sprintf (trn, "CoId=0%d&ID=%s&Name=%s&FriendPassword=%s&&Year=%s&Month=%s&Day=%s&Hour=%s&Minute=%s&Msg=%s",
           nummode, PagerNo, PagerName, PagerPassword, PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 945:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
      if (callmode)
    sprintf (trn, "hiUsage=AlphaPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\\RmtFiles&txPagerNo=%s&txPassword=%s&txSender=%s&txContent=%s&&%s",
         PagerNo, PagerPassword, cuser.userid, PagerMsg, genbuf);
      else
    sprintf (trn, "hiUsage=NumericPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\\RmtFiles&txPagerNo=%s&txPassword=%s&txContent=%s&&%s",
         PagerNo, PagerPassword, PagerMsg, genbuf);
      break;
    case 947:
      sprintf (trn, "AccountNo_0=0947%s&Password_0=%s&Sender=%s&Message=%s",
           PagerNo, PagerPassword, cuser.userid, PagerMsg);
      break;
    case 948:
      sprintf (trn, "MfcISAPICommand=SinglePage&svc_no=%s&reminder=%s&year=%s&month=%s&day=%s&hour=%s&min=%s&message=%s\n",
           PagerNo, (timemode) ? "1" : "0", PagerYearC, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 949:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
//      if(callmode)
      sprintf (PageSender, "&txSender=%s", cuser.userid);
      sprintf (trn, "func=%s&txPagerNo=%s&username=%s%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
           callmode ? "Alphapg" : "Numpg", PagerNo, PagerName, callmode ? PageSender : "", PagerPassword, PagerMsg, timemode ? "off" : "on", genbuf);

/*      else
   sprintf(trn,"func=Alphapg&hiDataDir=S:\fiss\\RmtFiles&hiInterval=0&hiLanguage=Taiwan&hiUsage=NumericPage&txPagerNo=%s&username=%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
   PagerNo,PagerName,PagerPassword,PagerMsg,timemode?"off":"on",genbuf); */
      break;
    }
  log_usies("BBCALL", NULL);
  sprintf (sendform, "POST %s HTTP/1.0\nReferer: %s\n%sContent-length:%d\n\n%s",
       PAGER_CGI[whichone], PAGER_REFER[whichone], PARA, strlen (trn), trn);

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return;

  memset ((char *) &serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;

  strcpy (SERVER_TOOL, PAGER_SERVER[nummode - 940]);
  if ((hp = gethostbyname (SERVER_TOOL)) == NULL)
    return;

  memcpy (&serv_addr.sin_addr, hp->h_addr, hp->h_length);
  switch (nummode)
    {
    case 943:
    case 945:
    case 946:
    case 947:
    case 948:
    case 949:
      serv_addr.sin_port = htons (WEBPORT);
      break;
    case 941:
      serv_addr.sin_port = htons (9100);
      break;
    }
  if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof serv_addr))
    {
      sprintf (result, "�L�k�s�u�A�ǩI����\n");
      f_cat (LOG_FILE, result);
      pressanykey ("�L�k�P���A�����o�s���A�ǩI����");
      return;
    }
  else
    {
      y_position++;
      move (y_position, 6);
      prints ("���A���w�g�s���W�A�еy��....");
      refresh ();
    }
  write (sockfd, sendform, strlen (sendform));
  shutdown (sockfd, 1);
  while (read (sockfd, result, sizeof (result)) > 0)
    {
      if (strstr (result, PAGER_CHECK[(nummode - 940) * 2 + timemode]) != NULL)
    {
      close (sockfd);
      f_cat (LOG_FILE, result);
      pressanykey ("���Q�e�X�ǩI");
      return;
    }
      f_cat (LOG_FILE, result);
      memset (result, 0, sizeof (result));
    }
  close (sockfd);
  pressanykey ("�L�k�e�X�A�i���] 1.�K�X���~ 2.�S�����X");
  return;
}

int
bbcall_main ()
{
  int y_position = call_y_position;
  char PagerNo[7], PagerPassword[32], PagerName[32], PagerMsg[80];
  char ask[5];
  char genbuf[100];
  int nummode;
  int callmode = 1;
  int msgnum[10][2] =
  {
    {0, 0},
    {16, 16},
    {0, 0},
    {14, 60},
    {0, 0},
    {20, 60},
    {14, 60},
    {20, 60},
    {17, 60},
    {20, 40}};

  clear ();
  show_file (BBSHOME "/etc/bbcall", 0, 13, ONLY_COLOR);
  getdata (y_position, 6, "���п�J�z�Q�ϥΪ��u�W�ǩI�t�ΡG", ask, 5, DOECHO, 0);
  if (ask[0] == 'H' || ask[0] == 'h')
    {
      more (BBSHOME"/etc/BBCALLhelp", YEA);
      return 0;
    }
  nummode = atoi (ask);
  if (!(nummode == 941 || nummode == 943 || nummode == 945 || nummode == 946 || nummode == 947 || nummode == 948 || nummode == 949))
    {
      pressanykey ("���~���ǩI�t��  ���ϥνu�W�ǩI");
      return 0;
    }

  y_position++;
  sprintf (genbuf, "���п�J�z�Q�ǩI�����X�G0%3d-", nummode);
  if (!getdata (y_position, 6, genbuf, PagerNo, 7, DOECHO, 0)
       && ((nummode == 943 || nummode == 946 || nummode == 949)
       && !getdata (y_position, 6, "���п�J�z�Q�ǩI���N���G", PagerName, 32, DOECHO, 0)))
    {
      pressanykey ("���~���X�ΥN��  ���ϥνu�W�ǩI");
      return 0;
    }
  PagerNo[6] = '\0';

  y_position++;
  if (nummode == 949 || nummode == 945)
    {
      ask[0] = '\0';
      getdata (y_position, 6, "���п�J������ (1)�Ʀr�� (2)��r���H[1]�G", ask, 2, DOECHO, 0);
      if (ask[0] == '2')
    callmode = 1;
      else
    callmode = 0;
      y_position++;
    }
  getdata (y_position, 6, "���п�J�ǩI�K�X�]�S���̽и��L�^�G", PagerPassword, 11, PASS, 0);

  y_position++;
  if (!getdata (y_position, 6, "���п�J�ǩI�Ʀr�Τ�r�T���]�̾����өw�^\n        �G", PagerMsg, msgnum[nummode - 940][callmode], DOECHO, 0))
    {
      pressanykey ("�ʤֶǰe�T��  ���ϥνu�W�ǩI");
      return 0;
    }
  y_position += 2;
  bbcall_function (y_position, nummode, callmode, PagerNo, PagerName, PagerPassword, PagerMsg);
  return;
}

/* �W�[�W�� */

int
bbcall_adduser (int max)
{
  struct BBCALL_DATA newfriend;
  char fpath[80];
  char genbuf[256];
  int y_pos = 3;

  setuserfile (fpath, "bbcalllist");
  if (file_list_count (fpath) > max)
    {
      pressanykey ("�w�F�W��W���A�L�k�A�W�[�F..");
      return 0;
    }
  clrchyiuan (3, 24);
  if (!getdata (y_pos, 0, "���п�J�W�r�ΥN���]���^�峣�i�^�G", genbuf, 13, DOECHO, 0))
    {
      pressanykey ("�S���W��A�L�k�W�[..");
      return 0;
    }
  if (genbuf[0] != ' ' && !belong (fpath, genbuf))
    {
      FILE *fp;
      int ok;

      y_pos++;
      sprintf (newfriend.name, "%s", genbuf);
      if (!getdata (y_pos, 0, "���п�J�����]�H��K��z�^�G", newfriend.class, 5, DOECHO, 0))
    sprintf (newfriend.class, "%s", "����");
      y_pos++;
      do
    {
      int len;
      char birth[6];
      len = getdata (y_pos, 0, "���п�J�ͤ�]��/�� �ҡG01/01�^�G", birth, 6, DOECHO, 0);
      if (len == 5 && birth[2] == '/')
        {
          ok = 1;
          strcpy (newfriend.birth, birth);
        }
      else if (len == 5 && birth[2] != '/')
        {
          pressanykey ("��J���~");
        }
      else
        {
          ok = 1;
          strcpy (newfriend.birth, "��");
        }
    }
      while (ok != 1);
      y_pos++;
      if (!getdata (y_pos, 0, "���п�J�q�ܡ]�� 03-5712121-12345�^\n�G", newfriend.phone, 19, DOECHO, 0))
    sprintf (newfriend.phone, "%s", "����J�q�ܸ��");
      y_pos += 2;
      if (getdata (y_pos, 0, "���п�J�ǩI���X�]�� 0943-1234556 �� 0943-�N���^�G", newfriend.bbcall, 21, DOECHO, 0))
    {
      y_pos++;
      if (!getdata (y_pos, 0, "���п�J�ǩI�K�X�]�S�����ܧK��^�G", newfriend.callpass, 11, DOECHO, 0))
        {
          strcpy (newfriend.callpass, "�L�K�X");
        }
    }
      else
    {
      strcpy (newfriend.bbcall, "�L�ǩI");
      strcpy (newfriend.callpass, "�L�K�X");

    }
      y_pos++;
      do
    {
      if (getdata (y_pos, 0, "���п�J�q�l�H�c�G", newfriend.email, 51, DOECHO, 0))
        {
          if (strchr (newfriend.email, '@'))
        ok = 1;
        }
      else
        {
          ok = 1;
          strcpy (newfriend.email, "�L�q�l�H�c");
        }
    }
      while (ok != 1);
      y_pos++;
      if (!getdata (y_pos, 0, "���п�J��}�]�Ǯթήa�̪����i�H�^\n�G", newfriend.address, 61, DOECHO, 0))
    sprintf (newfriend.address, "%s", "�L�a�}���");
      y_pos += 2;
      if (!getdata (y_pos, 0, "���п�J�����]�L�O�֡H�L������H�L���ߺD�H�^\n�G", newfriend.explain, 61, DOECHO, 0))
    strcpy (newfriend.explain, "�L���U����");
      if (fp = fopen (fpath, "a"))
    {
      fprintf (fp, "%s &%s&%s&%s&%s&%s&%s&%s&%s&����\n",
           newfriend.name,
           newfriend.class,
           newfriend.birth,
           newfriend.phone,
           newfriend.bbcall,
           newfriend.callpass,
           newfriend.email,
           newfriend.address,
           newfriend.explain
        );
      fclose (fp);
    }
      return 1;
    }
  else
    {
      pressanykey ("���ƦW��A�Шϥέק�");
      return 0;
    }
}


/* �ק�W�� */

int
bbcall_changeuser (struct BBCALL_DATA *newfriend)
{
  FILE *fp, *fpnew;
  char fpath[80];
  char fpathnew[80];
  char genbuf[256];
  char oldname[13];
  char ask[3];
  int y_pos = 3;
  int ok = 0;
  int len;

  setuserfile (fpath, "bbcalllist");
  sprintf (fpathnew, "%s-", fpath);

  clrchyiuan (3, 24);
  strcpy (oldname, newfriend->name);
  if (!getdata (y_pos, 0, "���п�J�W�r�ΥN���]���^�峣�i�^�G", newfriend->name, 13, DOECHO, newfriend->name))
    {
      pressanykey ("����S���W�r�A���");
      return 0;
    }
  len = strlen (newfriend->name);
  if (newfriend->name[len - 1] == ' ')
    newfriend->name[len - 1] = '\0';
  y_pos++;
  if (!getdata (y_pos, 0, "���п�J�����]�H��K��z�^�G", newfriend->class, 5, DOECHO, ((strstr (newfriend->class, "����")) != NULL) ? 0 : newfriend->class))
    sprintf (newfriend->class, "%s", "����");
  y_pos++;
  do
    {
      int len;
      char birth[6];
      len = getdata (y_pos, 0, "���п�J�ͤ�]��/�� �ҡG01/01�^�G", birth, 6, DOECHO, ((strstr (newfriend->birth, "��")) != NULL) ? 0 : newfriend->birth);
      if (len == 5 && birth[2] == '/')
    {
      ok = 1;
      strcpy (newfriend->birth, birth);
    }
      else if (len == 5 && birth[2] != '/')
    {
      pressanykey ("��J���~");
    }
      else
    {
      ok = 1;
      strcpy (newfriend->birth, "��");
    }
    }
  while (ok != 1);
  y_pos++;
  if (!getdata (y_pos, 0, "���п�J�q�ܡ]�� 03-5712121-12345�^\n�G", newfriend->phone, 19, DOECHO, ((strstr (newfriend->phone, "����J�q�ܸ��")) != NULL) ? 0 : newfriend->phone))
    sprintf (newfriend->phone, "%s", "����J�q�ܸ��");
  y_pos += 2;
  if (getdata (y_pos, 0, "���п�J�ǩI���X�]�� 0941-1234556 �� 0943-�N���^�G", newfriend->bbcall, 21, DOECHO, ((strstr (newfriend->bbcall, "�L")) != NULL) ? 0 : newfriend->bbcall))
    {
      y_pos++;
      if (!getdata (y_pos, 0, "���п�J�ǩI�K�X�]�S�����ܧK��^�G", newfriend->callpass, 11, DOECHO, ((strstr (newfriend->callpass, "�L")) != NULL) ? 0 : newfriend->callpass))
    {
      strcpy (newfriend->callpass, "�L�K�X");
    }
    }
  else
    {
      strcpy (newfriend->bbcall, "�L�ǩI");
      strcpy (newfriend->callpass, "�L�K�X");
    }
  y_pos++;
  do
    {
      if (getdata (y_pos, 0, "���п�J�q�l�H�c�G", newfriend->email, 51, DOECHO, ((strstr (newfriend->email, "�L")) != NULL) ? 0 : newfriend->email))
    {
      if (strchr (newfriend->email, '@'))
        ok = 1;
    }
      else
    {
      strcpy (newfriend->email, "�L�q�l�H�c");
      ok = 1;
    }
    }
  while (ok != 1);
  y_pos++;
  if (!getdata (y_pos, 0, "���п�J��}�]�Ǯթήa�̪����i�H�^\n�G", newfriend->address, 61, DOECHO, ((strstr (newfriend->address, "�L")) != NULL) ? 0 : newfriend->address))
    sprintf (newfriend->address, "%s", "�L��}���");
  y_pos += 2;
  if (!getdata (y_pos, 0, "���п�J�����]�L�O�֡H�L������H�L���ߺD�H�^\n�G", newfriend->explain, 61, DOECHO, ((strstr (newfriend->explain, "�L")) != NULL) ? 0 : newfriend->explain))
    {
      strcpy (newfriend->explain, "�L���U����");
    }
  y_pos += 3;
  getdata (y_pos, 0, "���T�w�ק�ܡH [y/N]�G", ask, 3, DOECHO, 0);
  if (ask[0] != 'Y' && ask[0] != 'y')
    {
      pressanykey ("���ק�");
      return 0;
    }
  if ((fp = fopen (fpath, "r")) && (fpnew = fopen (fpathnew, "w")))
    {
      char *chartmp;
      char getname[20];
      char buf[256];

      while (fgets (genbuf, 256, fp))
    {
      getname[0] = '\0';
      strcpy (buf, genbuf);
      chartmp = (char *) strtok (buf, "&");
      strcpy (getname, chartmp);
      if ((genbuf[0] != ' ') && strcmp (getname, oldname))
        {
          fputs (genbuf, fpnew);
        }
      else if ((genbuf[0] != ' ') && !strcmp (getname, oldname))
        {
          char buf2[256];
          buf2[0] = '\0';
          sprintf (buf2, "%s &%s&%s&%s&%s&%s&%s&%s&%s&����\n",
               newfriend->name,
               newfriend->class,
               newfriend->birth,
               newfriend->phone,
               newfriend->bbcall,
               newfriend->callpass,
               newfriend->email,
               newfriend->address,
               newfriend->explain
        );
          fputs (buf2, fpnew);
        }
    }
      fclose (fp);
      fclose (fpnew);
      f_mv (fpathnew, fpath);
    }
  return 1;
}


/* �R���W�� */

int
bbcall_deluser (struct BBCALL_DATA *newfriend)
{
  FILE *fp, *fpnew;
  char fpath[80];
  char fpathnew[80];
  char genbuf[256];
  char oldname[13];
  char ask[3];

  setuserfile (fpath, "bbcalllist");
  sprintf (fpathnew, "%s-", fpath);
  sprintf (oldname, "%s", newfriend->name);
  getdata (1, 0, "���T�w�R���ܡH [y/N]�G", ask, 3, DOECHO, 0);
  if (ask[0] != 'Y' && ask[0] != 'y')
    {
      pressanykey ("���R��");
      return 0;
    }
  if ((fp = fopen (fpath, "r")) && (fpnew = fopen (fpathnew, "w")))
    {
      char *chartmp;
      char getname[20];
      char buf[256];

      while (fgets (genbuf, 256, fp))
    {
      getname[0] = '\0';
      strcpy (buf, genbuf);
      chartmp = (char *) strtok (buf, "&");
      strcpy (getname, chartmp);
      if ((genbuf[0] != ' ') && strcmp (getname, oldname))
        {
          fputs (genbuf, fpnew);
        }
    }
      fclose (fp);
      fclose (fpnew);
      f_mv (fpathnew, fpath);
    }
  return 1;
}

int
bbcall_showuser (struct BBCALL_DATA *newfriend)
{
  clear ();
  move (3, 0);
  prints ("\n\
  �������������������������������������\n\
    [�W�r] �� %s\n\
    [����] �� %s\n\
    [�ͤ�] �� %s\n", newfriend->name, newfriend->class, newfriend->birth);
  prints ("\
    [�q��] �� %s\n\
    [�ǩI] �� %s\n\
    [�K�X] �� %s\n\
    [�H�c] �� %s\n", newfriend->phone, newfriend->bbcall, newfriend->callpass, newfriend->email);
  prints ("\
    [�a�}] �� %s\n\
    [����] �� %s\n\
  �������������������������������������\n", newfriend->address, newfriend->explain);
  pressanykey ("�ӤH���");
  return 0;
}

static int
invalidaddr (addr)
     char *addr;
{
  if (*addr == '\0')
    return 1;
  while (*addr)
    {
      if (not_alnum (*addr) && !strchr ("[].%!@:-_", *addr))
    return 1;
      addr++;
    }
  return 0;
}


static int
bbcall_name_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->name, j->name);
}

static int
bbcall_class_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->class, j->class);
}

static int
bbcall_birth_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->birth, j->birth);
}

static int
bbcall_phone_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->phone, j->phone);
}

static int
bbcall_bbcall_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->bbcall, j->bbcall);
}

static int
bbcall_email_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->email, j->email);
}

static int
bbcall_address_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->address, j->address);
}

static int
bbcall_explain_cmp (i, j)
     BBCALL_DATA *i, *j;
{
  return strcasecmp (i->explain, j->explain);
}


int
bbcall_menu ()
{

  struct BBCALL_DATA myfriend[128];
  FILE *fp;
  char fpath[80];
  int data_max = 128;
  int data_count = 0;
  int data_refresh = 1;
  int file_refresh = 0;
  int system_leave = 0;
  int screen_refresh = 1;
  int screen_pointer = 0;
  int screen_page = 0;
  int screen_startnum = 0;
  int screen_mode = 0;
  int screen_ch;

  setutmpmode (MSGMENU);
  log_usies("MSGMENU",NULL);
  setuserfile (fpath, "bbcalllist");
  do
    {
      if (file_refresh && data_count > 0)
    {
      int i;
      fp = fopen (fpath, "w");
      for (i = 0; i < data_count; i++)
        {
          fprintf (fp, "%s &%s&%s&%s&%s&%s&%s&%s&%s&����\n",
               myfriend[i].name,
               myfriend[i].class,
               myfriend[i].birth,
               myfriend[i].phone,
               myfriend[i].bbcall,
               myfriend[i].callpass,
               myfriend[i].email,
               myfriend[i].address,
               myfriend[i].explain
        );
        }
      fclose (fp);
    }
      if (screen_refresh)
    {
      clear ();
      showtitle ("�q�T���p����", BoardName);
      move (1, 0);
      prints ("[��]���} [a]�s�W [d]�R�� [c]�ק� [k]�R���Ҧ� [s]�ƧǾ�z [b]�u�W�ǩI [��]�d�� \n");
      switch (screen_mode)
        {
        case 0:
          prints ("[1;37;42m �s��  �W�r�ΥN��  ��  �� �� �� �q��               �ǩI���X     �ǩI�K�X      [0m");
          break;
        case 1:
          prints ("[1;37;42m �s��  �W�r�ΥN��  ��  �� �q�l�H�c[33m�]�N�O�ҿת�E-Mail�աI���� M �^             [0m");
          break;
        case 2:
          prints ("[1;37;42m �s��  �W�r�ΥN��  �s���a�}[33m�]�Ǯթήa�̪���}�A���ŰO�o�h���X���X��I�^       [0m");
          break;
        case 3:
          prints ("[1;37;42m �s��  �W�r�ΥN��  ���U����[33m�]�ͽ˴y�z�^                                       [0m");
          break;
        }
    }
      if (data_refresh)
    {
      char genbuf[256];
      char *chartmp;

      data_count = 0;
      if (fp = fopen (fpath, "r"))
        {
          while (fgets (genbuf, 256, fp) && data_count < data_max)
        {
          myfriend[data_count].label = 0;
          chartmp = (char *) strtok (genbuf, "&");
          strcpy (myfriend[data_count].name, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].class, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].birth, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].phone, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].bbcall, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].callpass, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].email, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].address, chartmp);

          chartmp = (char *) strtok (NULL, "&");
          strcpy (myfriend[data_count].explain, chartmp);
          data_count++;
        }
          fclose (fp);
        }
      data_refresh = 0;
    }
      if (data_count == 0)
    {
      move (4, 0);
      screen_refresh = 0;
      prints ("�W�椤�L�����ơA�Ы� a �[�J���");
      move (b_lines, 0);
      prints ("[1;41;37m �q�T���W�� [0;30;47m �@�]�w[31m%3d[30m/%3d���  [31m[M/m][30m���~/�����H�H [31m[Tab][30m�����e��              [0m", data_count, data_max);
    }
      else
    {
      if (screen_refresh || screen_page != screen_pointer / 20)
        {
          int i;
          clrchyiuan (3, 24);
          screen_page = screen_pointer / 20;
          screen_startnum = screen_page * 20;
          for (i = screen_startnum; i < screen_startnum + 20; i++)
        {
          if (i < data_count)
            {
              int pass_len = 0;
              int pass_i;
              char pass_word[11] = "";

              pass_len = strlen (myfriend[i].callpass);
              for (pass_i = 0; pass_i < pass_len; pass_i++)
            pass_word[pass_i] = '*';
              move (3 + i - screen_startnum, 0);
              switch (screen_mode)
            {
            case 0:
              prints ("  %3d%s%-12s[%-4s] %-5s %-18s %-11s  %s",
                  i + 1,
                  myfriend[i].label ? "��" : "  ",
                  myfriend[i].name,
                  myfriend[i].class,
                  myfriend[i].birth,
                  myfriend[i].phone,
                  myfriend[i].bbcall,
                  ((strstr (myfriend[i].callpass, "�L�K�X")) == NULL) ? pass_word : "�L�K�X");
              break;
            case 1:
              prints ("  %3d%s%-12s[%-4s] %-50s",
                  i + 1,
                  myfriend[i].label ? "��" : "  ",
                  myfriend[i].name,
                  myfriend[i].class,
                  myfriend[i].email);
              break;
            case 2:
              prints ("  %3d%s%-12s%-64s",
                  i + 1,
                  myfriend[i].label ? "��" : "  ",
                  myfriend[i].name,
                  myfriend[i].address);
              break;
            case 3:
              prints ("  %3d%s%-12s%-64s",
                  i + 1,
                  myfriend[i].label ? "��" : "  ",
                  myfriend[i].name,
                  myfriend[i].explain);
              break;
            }
            }
        }
          move (b_lines, 0);
          prints ("[1;41;37m �q�T���W�� [0;30;47m �@�]�w[31m%3d[30m/%3d���  [31m[M/m][30m���~/�����H�H [31m[Tab][30m�����e��              [0m", data_count, data_max);
          screen_refresh = 0;
        }
      if (screen_pointer >= data_count)
        screen_pointer = data_count - 1;
      move (3 + screen_pointer % 20, 0);
      prints ("��");
    }
      move (b_lines, 0);
      screen_ch = igetkey ();
      move (3 + screen_pointer % 20, 0);
      prints ("  ");
      switch (screen_ch)
    {
    case 'A':
    case 'a':
      if (bbcall_adduser (data_max))
        data_refresh = 1;
      screen_refresh = 1;
      break;

    case 'D':
    case 'd':
      if (data_count > 0)
        {
          bbcall_deluser (&myfriend[screen_pointer]);
          data_refresh = 1;
          screen_refresh = 1;
        }
      break;

    case 'C':
    case 'c':
      if (data_count > 0)
        {
          bbcall_changeuser (&myfriend[screen_pointer]);
          data_refresh = 1;
          screen_refresh = 1;
        }
      break;

    case 'E':
    case 'e':
    case 'Q':
    case KEY_LEFT:
      screen_ch = 'q';
    case 'q':
      system_leave = 1;
      break;

    case KEY_PGUP:
      if (screen_pointer == 0)
        screen_pointer = data_count;
      else
        {
          screen_pointer -= 20;
          if (screen_pointer < 0)
        screen_pointer = 0;
        }
      break;

    case KEY_PGDN:
    case ' ':
      if (screen_pointer == data_count)
        screen_pointer = 0;
      else
        {
          screen_pointer += 20;
          if (screen_pointer >= data_count)
        screen_pointer = data_count - 1;
        }
      break;

    case KEY_UP:
      screen_pointer--;
      if (screen_pointer < 0)
        screen_pointer = data_count - 1;
      break;

    case KEY_DOWN:
      screen_pointer++;
      if (screen_pointer >= data_count)
        screen_pointer = data_count - 1;
      break;

    case KEY_HOME:
    case '0':
      screen_pointer = 0;
      break;

    case '$':
    case KEY_END:
      screen_pointer = data_count - 1;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (data_count > 0)
        {
          int i;
          if ((i = search_num (screen_ch, data_count)) >= 0)
        {
          screen_pointer = i;
          if (screen_pointer >= data_count)
            screen_pointer = data_count - 1;
          if (screen_pointer < 0)
            screen_pointer = 0;
        }
        }
      screen_refresh = 1;
      break;

    case KEY_RIGHT:
    case '\n':
    case '\r':
    case 'r':
      if (data_count > 0)
        {
          bbcall_showuser (&myfriend[screen_pointer]);
        }
      screen_refresh = 1;
      break;

    case 'm':
      if (data_count > 0)
        {
          int tuid;
          int len;
          char email[13];
          strcpy (email, myfriend[screen_pointer].name);
          len = strlen (email);
          email[len - 1] = '\0';

          if (tuid = searchuser (email) && HAS_PERM (PERM_BASIC) && HAS_PERM (PERM_CHAT) && HAS_PERM (PERM_PAGE))
        {
          screenline *screen;
          screen = (screenline *) calloc (t_lines, sizeof (screenline));
          vs_save (screen);
          showtitle ("�������y", BoardName);
          prints ("[�H�H] ���H�H�G%s", email);
          my_send (email);
          vs_restore (screen);
          free (screen);
        }
          else
        pressanykey ("�S���o�ӤH��");
        }
      screen_refresh = 1;
      break;
    case 'M':
      if (data_count > 0)
        {
          char email[51];
          strcpy (email, myfriend[screen_pointer].email);
          if (strchr (email, '@') && !invalidaddr (email) && getdata (1, 0, "�D  �D�G", save_title, TTLEN, DOECHO, 0))
        do_send (email, save_title);
          else if ((strstr (email, "�L�q�l�H�c")) != NULL)
        pressanykey ("���H�L�q�l�H�c��");
          else
        pressanykey ("���ϥ�Internet Email");
        }
      screen_refresh = 1;
      break;

    case 'b':
    case 'B':
      if (data_count > 0)
        {
          int y_position = 3;
          int nummode, callmode = 1;
          char bbcall[21];
          char PagerNo[7], PagerPassword[32], PagerName[32], PagerMsg[80];
          strcpy (bbcall, myfriend[screen_pointer].bbcall);
          nummode = (bbcall[0] - '0') * 1000 + (bbcall[1] - '0') * 100 + (bbcall[2] - '0') * 10 + (bbcall[3] - '0');
          if ((strstr (bbcall, "�L�ǩI")) == NULL && bbcall[4] == '-' && (nummode == 941 || nummode == 943 || nummode == 945 || nummode == 946 || nummode == 947 || nummode == 948 || nummode == 949))
        {
          int msgnum[10][2] =
          {
            {0, 0},
            {16, 16},
            {0, 0},
            {14, 60},
            {0, 0},
            {20, 60},
            {14, 60},
            {20, 60},
            {17, 60},
            {20, 40}};
          if ((bbcall[5] - '0') >= 0 && (bbcall[5] - '0') <= 9)
            sprintf (PagerNo, "%s", bbcall + 5);
          else
            sprintf (PagerName, "%s", bbcall + 5);
          //if(myfriend[screen_pointer].callpass[0]!=' ')
          if ((strstr (myfriend[screen_pointer].callpass, "�L�K�X")) == NULL)
            sprintf (PagerPassword, "%s", myfriend[screen_pointer].callpass);
          clrchyiuan (y_position, y_position + 19);
          move (y_position, 0);
          prints ("[1;31m �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[0m");
          move (y_position + 19, 0);
          prints ("[1;31m �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[0m");
          y_position++;
          clrchyiuan (y_position, y_position + 8);
          if (nummode == 949 || nummode == 945)
            {
              char ask[2];
              ask[0] = '\0';
              getdata (y_position, 0, "      ���п�J������ (1)�Ʀr�� (2)��r���H[1]�G", ask, 2, DOECHO, 0);
              if (ask[0] == '2')
            callmode = 1;
              else
            callmode = 0;
              y_position++;
            }
          if (getdata (y_position, 0, "      ���п�J�ǩI�Ʀr�Τ�r�T���]�̾����өw�^\n        �G", PagerMsg, msgnum[nummode - 940][callmode], DOECHO, 0))
            {
              y_position += 2;
              bbcall_function (y_position, nummode, callmode, PagerNo, PagerName, PagerPassword, PagerMsg);
            }
          else
            pressanykey ("�ʤֶǰe�T��  ���ϥνu�W�ǩI");
        }
          else if ((strstr (bbcall, "�L�ǩI")) != NULL)
        pressanykey ("���H�S���ǩI���X����Ƴ�");
          else
        pressanykey ("�����Ѧ����X�������ǩI");
        }
      screen_refresh = 1;
      break;

    case 'S':
    case 's':
      if (data_count > 1)
        {
          char ans[5];
          getdata (1, 0, "[1]�W�r [2]���� [3]�ͤ� [4]�q�� [5]�ǩI [6]�H�c [7]�a�} [8]���� [Q]���}:", ans, 4, LCECHO, 0);
          file_refresh = 1;
          switch (ans[0])
        {
        case '1':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_name_cmp);
          screen_mode = 0;
          break;
        case '2':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_class_cmp);
          screen_mode = 0;
          break;
        case '3':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_birth_cmp);
          screen_mode = 0;
          break;
        case '4':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_phone_cmp);
          screen_mode = 0;
          break;
        case '5':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_bbcall_cmp);
          screen_mode = 0;
          break;
        case '6':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_email_cmp);
          screen_mode = 1;
          break;
        case '7':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_address_cmp);
          screen_mode = 2;
          break;
        case '8':
          qsort (myfriend, data_count, sizeof (BBCALL_DATA), bbcall_explain_cmp);
          screen_mode = 3;
          break;
        default:
          file_refresh = 0;
          break;
        }
        }
      screen_refresh = 1;
      break;

    case KEY_TAB:
      screen_mode = (screen_mode + 1) % 4;
      screen_refresh = 1;
      break;

    case 'K':
    case 'k':
      if (data_count > 0)
        {
          char ans[5];
          getdata (1, 0, "����W��N�|�Q�R��,�z�T�w�� (y/N)?", ans, 3, LCECHO, 0);
          if (ans[0] == 'y' || ans[0] == 'Y')
        {
          unlink (fpath);
          pressanykey ("�W��R������....");
        }
          screen_refresh = 1;
          data_refresh = 1;
        }
      break;
    }
    }
  while (system_leave != 1);
  return 0;
}

int
bbcall_func(int nummode, int callmode, char *PagerNo, char *PagerPassword, char *PagerMsg)
{
  char trn[512], result[1024], sendform[512];
  char PagerYear[4], PagerYearC[3], PagerMonth[3], PagerDay[3], PagerHour[3],
    PagerMin[3];
  char PageSender[10];
  char genbuf[200];
  char LOG_FILE[100];
  char SERVER_TOOL[100];
  char *PAGER_SERVER[10] =
  {
    "0940��",           //0940
     "www.chips.com.tw",    //0941
     "0942��",          //0942
     "www.pager.com.tw",    //0943
     "0944��",          //0944
     "www.tw0945.com",      //0945
     "www.pager.com.tw",    //0946
     "web1.hoyard.com.tw",  //0947
     "www.fitel.net.tw",    //0948
     "www.southtel.com.tw"  //0949
  };
  /* PAGER_CGI[2n+0]:�Ʀr PAGER_CGI[2n+1]:��r */
  char *PAGER_CGI[20] =
  {
    "��", "��",
    "/cgi-bin/paging1.pl", "/cgi-bin/paging1.pl",
    "��", "��",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "��", "��",
    "/Scripts/fiss/PageForm.exe", "/Scripts/fiss/PageForm.exe",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "/scripts/fp_page1.dll", "/scripts/fp_page1.dll",
    "/cgi-bin/Webpage.dll", "/cgi-bin/Webpage.dll",
    "../pager/Webpg.asp", "../pager/Webpg.asp"};
  /* PAGER_REFER[2n+0]:�Ʀr PAGER_REFER[2n+1]:��r */
  char *PAGER_REFER[20] =
  {
    "��", "��",
    "http://www.chips.com.tw:9100/WEB2P/page_1.htm", "http://www.chips.com.tw:9100/WEB2P/page_1.htm",
    "��", "��",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "��", "��",
    "http://www.tw0945.com/call_numPg.HTM", "http://www.tw0945.com/call_AlphaPg.HTM",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "http://web1.hoyard.com.tw/freeway/freewayi.html#top", "http://web1.hoyard.com.tw/freeway/freewayi.html#top",
    "http://www.fitel.net.tw/html/svc03.htm#top", "http://www.fitel.net.tw/html/svc03.htm#top",
    "http://www.southtel.com.tw/numpg.htm", "http://www.southtel.com.tw/Alphapg.htm"};
  /* PAGER_CHECK[2n+0]:�ߧY PAGER_CHECK[2n+1]:�w�� */
  char *PAGER_CHECK[20] =
  {
    "��", "��",
    "�ǰe��", "�w����",
    "��", "��",
    "���T", "���T",
    "��", "��",
    "SUCCESS.HTM", "SUCCESS.HTM",
    "���T", "���T",
    "�� �\\", "�� �\\",
    "�ǩI���\\", "�ǩI���\\",
    "SUCCESS.HTM", "SUCCESS.HTM"};

  struct sockaddr_in serv_addr;
  struct hostent *hp;
  int sockfd;
  int timemode = 0;
  int whichone;

  whichone = (nummode - 940) * 2 + callmode;
  timemode = 0;
  sprintf (PagerYear, "%4d", 1999);
  sprintf (PagerYearC, "%2d", 88);
  sprintf (PagerMonth, "%02d", 1);
  sprintf (PagerDay, "%02d", 1);
  sprintf (PagerHour, "%02d", 0);
  sprintf (PagerMin, "%02d", 0);

  sprintf (LOG_FILE, BBSHOME"/log/0%d.log", nummode);
  sprintf (result, "\n\n----�ǰe�����}�l----\n���X: 0%3d%s \n�T��: %s\n�Ҧ�: [%s] %s\n----�ǰe��������----\n",
       nummode, PagerNo, PagerMsg, (timemode) ? "�w���ǩI" : "�ߧY�ǩI", (timemode) ? genbuf : " ");
  f_cat (LOG_FILE, result);

  genbuf[0] = '\0';
  switch (nummode)
    {
    case 941:
      sprintf (genbuf, "=on&year=%s&month=%s&day=%s&hour=%s&min=%s", PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin);
      sprintf (trn, "PAGER_NO=%s&MSG_TYPE=NUMERIC&TRAN_MSG=%s&%s%s",
           PagerNo, PagerMsg, timemode ? "DELAY" : "NOW", timemode ? genbuf : "=on");
      break;
    case 943:
    case 946:
      sprintf (trn, "CoId=0%d&ID=%s&Name=&FriendPassword=%s&&Year=%s&Month=%s&Day=%s&Hour=%s&Minute=%s&Msg=%s",
           nummode, PagerNo, PagerPassword, PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 945:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
      if (callmode)
    sprintf (trn, "hiUsage=AlphaPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\\RmtFiles&txPagerNo=%s&txPassword=%s&txSender=WD&txContent=%s&&%s",
         PagerNo, PagerPassword, PagerMsg, genbuf);
      else
    sprintf (trn, "hiUsage=NumericPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\\RmtFiles&txPagerNo=%s&txPassword=%s&txContent=%s&&%s",
         PagerNo, PagerPassword, PagerMsg, genbuf);
      break;
    case 947:
      sprintf (trn, "AccountNo_0=%s&Password_0=%s&Sender=WD&Message=%s",
           PagerNo, PagerPassword, PagerMsg);
      break;
    case 948:
      sprintf (trn, "MfcISAPICommand=SinglePage&svc_no=%s&reminder=%s&year=%s&month=%s&day=%s&hour=%s&min=%s&message=%s\n",
           PagerNo, (timemode) ? "1" : "0", PagerYearC, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 949:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
//      if(callmode)
      sprintf (PageSender, "&txSender=WD-BBS");
      sprintf (trn, "func=%s&txPagerNo=%s&username=%s%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
           callmode ? "Alphapg" : "Numpg", PagerNo, "", callmode ? PageSender : "", PagerPassword, PagerMsg, timemode ? "off" : "on", genbuf);

/*      else
   sprintf(trn,"func=Alphapg&hiDataDir=S:\fiss\RmtFiles&hiInterval=0&hiLanguage=Taiwan&hiUsage=NumericPage&txPagerNo=%s&username=%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
   PagerNo,"",PagerPassword,PagerMsg,timemode?"off":"on",genbuf); */
      break;
    }
  sprintf (sendform, "POST %s HTTP/1.0\nReferer: %s\n%sContent-length:%d\n\n%s",
       PAGER_CGI[whichone], PAGER_REFER[whichone], PARA, strlen (trn), trn);

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return;

  memset ((char *) &serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;

  strcpy (SERVER_TOOL, PAGER_SERVER[nummode - 940]);
  if ((hp = gethostbyname (SERVER_TOOL)) == NULL)
    return;

  memcpy (&serv_addr.sin_addr, hp->h_addr, hp->h_length);
  switch (nummode)
    {
    case 943:
    case 945:
    case 946:
    case 947:
    case 948:
    case 949:
      serv_addr.sin_port = htons (WEBPORT);
      break;
    case 941:
      serv_addr.sin_port = htons (9100);
      break;
    }
  if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof serv_addr))
    {
      sprintf (result, "�L�k�s�u�A�ǩI����\n");
      f_cat (LOG_FILE, result);
      return;
    }
  write (sockfd, sendform, strlen (sendform));
  shutdown (sockfd, 1);
  while (read (sockfd, result, sizeof (result)) > 0)
    {
      if (strstr (result, PAGER_CHECK[(nummode - 940) * 2 + timemode]) != NULL)
    {
      close (sockfd);
      f_cat (LOG_FILE, result);
      return;
    }
      f_cat (LOG_FILE, result);
      memset (result, 0, sizeof (result));
    }
  close (sockfd);
  return;
}

#include<stdarg.h>
int
bbcall(va_list pvar)
{
  int nummode, callmode;
  char *PagerNo, *PagerPassword, *PagerMsg;

  nummode = va_arg(pvar, int);
  callmode = va_arg(pvar, int);
  PagerNo = va_arg(pvar, char *);
  PagerPassword = va_arg(pvar, char *);
  PagerMsg = va_arg(pvar, char *);

  return bbcall_func(nummode,callmode,PagerNo,PagerPassword,PagerMsg);
}
