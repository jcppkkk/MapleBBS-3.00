/*-------------------------------------------------------*/
/* fortune.c    ( NTHU CS MapleBBS Ver 3.10 )            */
/*-------------------------------------------------------*/
/* author : Kepler.bbs@bbs.cs.nthu.edu.tw                */
/* target : �B�չw��                                     */
/* create : 99/08/26                                     */
/* update :   /  /                                       */
/*-------------------------------------------------------*/

#include "bbs.h"


#define mouts(y,x,s)    { move(y,x); outs(s); }

#define SERVER_fortune     "www.big.com.tw"
#define HTTP_PORT   80

#define CGI_fortune "/wkluck/wkluck.asp"

#define REFER_fortune      "http://www.big.com.tw/"

#define PARA "\
Connection: Keep-Alive\r\n\
User-Agent: Lynx/2.6  libwww-FM/2.14\r\n\
Content-type: application/x-www-form-urlencoded\r\n\
Accept: text/html, text/plain, application/x-wais-source, application/html, */*\r\n\
Accept-Encoding: gzip\r\n\
Accept-Language: en\r\n\
Accept-Charset: iso-8859-1,*,utf-8\r\n"


static int
http_conn(char *server, char *s)
{
  int sockfd, start_show;
  int cc, len;
  char *xhead, *xtail, fname[50],ans[3], msg[101];
  static char pool[2048];
  FILE *fp;

  if ((sockfd = dns_open(server, HTTP_PORT)) < 0)
  {
    pressanykey("�L�k�P���A�����o�s���A�d�ߥ���");
    return 0;
  }
  else
  {
    mouts(22, 0, "\033[1;36m���b�s�����A���A�еy��.....................\033[m");
    refresh();
  }

  write(sockfd, s, strlen(s));
  shutdown(sockfd, 1);

  /* parser return message from web server */
  xhead = pool;
  xtail = pool;
  start_show = 0;

  /* usr_fpath(fname, cuser.userid,Msg_File); */
  sprintf(fname, "tmp/%s.fortune", cuser.userid);

  fp = fopen(fname, "w");
  len = 0;
  start_show = 0;

  for (;;)
  {
    if (xhead >= xtail)
    {
      xhead = pool;
      cc = read(sockfd, xhead, sizeof(pool));
      if (cc <= 0)
       break;
      xtail = xhead + cc;
    }

    cc = *xhead++;

    if ((len == 5) && (!str_ncmp(msg, "body", 4)))
    {
      start_show = 1;
    }

    if (cc == '<')
    {
      len = 1;
      continue;
    }

    if (len)
    {
      /* support<br>and<P>and</P> */

      if (cc == '>')
      {
       if ((len == 3) && (!str_ncmp(msg, "br", 2)))
       {
         fputc('\n', fp);
       }
       else if ((len == 2) && (!str_ncmp(msg, "P", 1)))
       {
         fputc('\n', fp);
       }
       else if ((len == 3) && (!str_ncmp(msg, "ul", 2)))
       {
         fputc('\n', fp);
       }

      /*
       if ((len == 4) && (!str_ncmp(msg, "/ul", 2)))
       {
         if(start_show++==3)
          start_show=0;
       }
      */

       len = 0;
       continue;
      }

      if (len <= 5)
      {
       msg[len - 1] = cc;
      }

      len++;
      continue;
    }
     if (start_show)
     {
      if ((cc != '\r')&&(cc != '\n'))
       fputc(cc, fp);
     }
  }

  close(sockfd);
  fputc('\n', fp);
  fclose(fp);

  /* show message that return from the web server */

  if ( more(fname, (void *) -1 /* NULL */) != -1)
  {
    getdata(b_lines, 0, "�M��(C) ���ܳƧѿ�(M) [C]�H", ans, 3, LCECHO,0);
    switch (*ans)
    {
      case 'm' :
      {
        fileheader mymail;
        char title[128], buf[80];
        sethomepath(buf, cuser.userid);
        stampfile(buf, &mymail);
        mymail.savemode = '\0';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.title, "�B�չw�����G");
        strcpy(mymail.owner, "[�� �� ��]");
        sethomedir(title, cuser.userid);
        rec_add(title, &mymail, sizeof(mymail));
        f_mv(fname, buf);
      }
        break;

      /* case 'c': */
      default :
        unlink(fname);
        break;
    }
  }
  return 0;
}

/* SiE:
 	i don't know can't it locate the function attr_get (),
	maybe u can run gdb or other trace tool to find why it happened....
	but locate the program here can resolve the problem...
*/

#if 1
static int
_my_attr_get(char *userid, int key, void *value)
{
  char fpath[64];
  int k;
  FILE *fp;

  sethomefile(fpath, userid, ".ATTR");

  if(fp = fopen(fpath, "rb"))
  {
    while(fread(&k,sizeof k, 1,fp))
    {
      if(k == key)
      {
        k = fread(value, (size_t) (k & 0xff), 1, fp);
        fclose(fp);
        return k-1;
      }
      else
        fseek(fp, (unsigned long) (k & 0xff), SEEK_CUR);
    }
    fclose(fp);
  }
  return -1; 
}

static int
test_my_attr_get (char *id, int key, void *value)
{
  return _my_attr_get (id, key, value);
}
#endif

static int
fortuneQuery()
{
  char atrn[256], sendform[512];
  char Name[16],Sex[2],Blood[2],Birth_year[8],Birth_month[8],Birth_day[8],UserName[IDLEN+1];
  char Kind[2],Hot_year[8],Hot_month[8],Hot_day[8];

  char attr[12];

  str_ncpy(UserName, cuser.userid, sizeof UserName);

  /* Thor.991019: ���J ATTR ��� */
  /* memset(attr, 0 ,sizeof attr); */
  if (test_my_attr_get(cuser.userid, ATTR_BIRTH_MFB, attr) >= 0)
  {
    Sex[0] = attr[8] == 'M' ? '1' : '2';
    Sex[1] = '\0';

    switch(attr[9]) {
      case 'O':
        Blood[0] = '1';
       break;
      case 'A':
        Blood[0] = '2';
       break;
      case 'B':
        Blood[0] = '3';
       break;
      default:
        Blood[0] = '4';
       break;
    }
    Blood[1] = '\0';

    str_ncpy(Birth_year, &attr[0], 5);
    str_ncpy(Birth_month, &attr[4], 3);
    str_ncpy(Birth_day, &attr[6], 3);
  }
  else {
    memset(Sex, 0 ,sizeof Sex);
    memset(Blood, 0 ,sizeof Blood);
    memset(Birth_year, 0 ,sizeof Birth_year);
    memset(Birth_month, 0 ,sizeof Birth_month);
    memset(Birth_day, 0 ,sizeof Birth_day);
  }

  /* print data */
  move(16, 0); prints("�m�W�G%-12s   �ʧO <1>�k <2>�k�G%s", UserName, Sex);
  move(17, 0); prints("�嫬 <1>O�� <2>A�� <3>B�� <4>AB���G%s       ", Blood);
  move(18, 0); prints("�X�ͤ���G�褸(����+1911=�褸) %5s�~ %3s�� %3s��",Birth_year, Birth_month, Birth_day);
#if 0
  mouts(16, 0, "�m�W�G               �ʧO <1>�k <2>�k�G");
  mouts(17, 0, "�嫬 <1>O�� <2>A�� <3>B�� <4>AB���G       ");
  mouts(18, 0, "�X�ͤ���G�褸(����+1911=�褸)      �~    ��    ��");
#endif
#if 0
  mouts(19, 0, "�w����� <1>���g <2>�ۤv�M�w�G   �褸      �~    ��    ��");
#endif

  for(;;)
  {
    if(*Sex && *Blood && *Birth_year && *Birth_month && *Birth_day)
    {
      if(answer("��ƥ��T��(Y/N)?[Y]") != 'n')
        break;
    }

    if(!getdata(16,21, "�ʧO <1>�k <2>�k�G",Sex , 2, GCARRY,0)
     ||!getdata(17, 0, "�嫬 <1>O�� <2>A�� <3>B�� <4>AB���G", Blood, 2, GCARRY,0)
     ||!getdata(18, 0, "�X�ͤ���G�褸(����+1911=�褸) ",Birth_year , 5, GCARRY,0)
     ||!getdata(18,36, "�~ ",Birth_month , 3, GCARRY,0)
     ||!getdata(18,42, "��",Birth_day , 3, GCARRY,0))
    {
       pressanykey("��...��ƬO�@�w�n��.....@_@");
       return 0;
    }
    mouts(18, 48, "��");
  }

  /* Thor.991019: �x�s ATTR ��� */
  sprintf(attr, "%04d%02d%02d%c%c", atoi(Birth_year), atoi(Birth_month), atoi(Birth_day), Sex[0] == '1' ? 'M' : 'F', Blood[0] == '1' ? 'O' : 'A' + (Blood[0] - '2'));


    pressanykey("put..");
  attr_put(cuser.userid, ATTR_BIRTH_MFB, attr);

#if 0
  if(!getdata(19, 0, "�w����� <1>���g <2>�ۤv�M�w [1]�G",Kind, 2, LCECHO,0))

  if (!strcmp(Kind,"2"))
  {
    if(!getdata(19,37, "�褸 ",Hot_year , 5, LCECHO,0)
     ||!getdata(19,47, "�~ ",Hot_month , 3, LCECHO,0)
     ||!getdata(19,53, "��",Hot_day , 3, LCECHO,0))
    {
       pressanykey("��...��ƬO�@�w�n��.....@_@");
       return 0;
    }
    mouts(19, 58, "��");
  }
  else
  {
    *Hot_year = *Hot_month = *Hot_day = '\0';
  }
#endif
  strcpy(Kind,"1");
  *Hot_year = *Hot_month = *Hot_day = '\0';

  if(answer("�T�w�n���D�A���B�աH[y] ") == 'n')
  {
    pressanykey("hmm.....�ڤ��H�o�@�M��...^o^");
    return 0;
  }
  else
  {

    url_encode(Name, UserName);

    sprintf(atrn, "workdir=%2Fwkluck%2F&name=%s&sex=%s&blood=%s&year=%s&month=%s&day=%s&kind=%s&hotyear=%s&hotmonth=%s&hotday=%s",Name ,Sex ,Blood ,Birth_year ,Birth_month ,Birth_day ,Kind ,Hot_year ,Hot_month ,Hot_day);

    /* Thor.990330: �[�W log */
    log_usies("FORTUNE",atrn);

    sprintf(sendform, "POST %s HTTP/1.0 Referer: %s\n%sContent-length:%d\n\n%s", CGI_fortune, REFER_fortune, PARA, strlen(atrn), atrn);

    http_conn(SERVER_fortune, sendform);
    return 0;
  }
}

static void show_image()
{
  char *image[12][4] = {
{
  "����������",
  "��  ��  ��",
  "��  ��  ��",
  "    ��    "
},
{
  "����  ����",
  "����������",
  "��      ��",
  "����������"
},
{
  "����������",
  "  ��  ��  ",
  "  ��  ��  ",
  "����������",
},
{
  "����������",
  "����  ����",
  "����  ����",
  "����������",
},
{
  "����������",
  "����  ����",
  "����  ��  ",
  "����  ����",
},
{
  "����������",
  "  ��������",
  "  ��������",
  "  ��������",
},
{
  "����������",
  "����  ����",
  "����  ����",
  "����������",
},
{
  "��������  ",
  "  ������  ",
  "  ��������",
  "  ��������",
},
{
  "  ������  ",
  "����������",
  "��  ��  ��",
  "  ������  ",
},
{
  "����������",
  "����������",
  "����������",
  "����������",
},
{
  "����������",
  "����������",
  "����������",
  "����������",
},
{
  "����  ����",
  "����������",
  "  ��  ��  ",
  "����  ����",
}, };

  int x[12], y[12];
  int i, j, k;

  srand(time(NULL));

retry:
  k = 0;

  for(i = 0; i < 12; i++)
  {
    for(;;)
    {
      x[i] = rand() % 70;
      y[i] = (rand() % 11) + 1; /* 1~12 */
      for(j = 0; j < i; j++)
      {
        if( abs(x[i]-x[j]) <= 10 && abs(y[i]-y[j]) < 4)
          break; /* Thor: ���� */
      }
      if(j == i) break; /* Thor: ���S���� */
      if(++k > 10000) goto retry; /* Thor: ���� */
    }
  }

  for(i = 0; i < 12; i++)
    for(j = 0; j < 4; j++)
      mouts(y[i] + j, x[i], image[i][j]);

}

#if 0
void show_image()
{
  FILE *astrofile;
  int astrology,y=2;
  char *scanline;
  char *Image_File[7]={"1.img","2.img","3.img","4.img","5.img","6.img","7.img"};
  char *temp;

  scanline=(char*)malloc(sizeof(char)*180);
  temp=(char*)malloc(sizeof(char)*80);

  strcpy(temp,BBSHOME"/etc/");
  srand(time(NULL));
  astrology=rand() % 7;
  temp=strcat(temp,Image_File[astrology]);
  astrofile=fopen(temp, "r");
  fgets(scanline,80,astrofile);
  while (!feof(astrofile))
  {
      mouts(y,0,scanline);
      y++;
      fgets(scanline,80,astrofile);
  }
  fclose(astrofile);
}
#endif

int
main_fortune()
{
  setutmpmode(FORTUNE);
  clear();
  move(0, 23);
  outs("\033[1;37;44m�� www.big.com.tw�B�չw���t�� ��\033[m");

  show_image();

  fortuneQuery();
  return 0;
}
