/*-------------------------------------------------------*/
/* fortune.c    ( NTHU CS MapleBBS Ver 3.10 )            */
/*-------------------------------------------------------*/
/* author : Kepler.bbs@bbs.cs.nthu.edu.tw                */
/* target : 運勢預測                                     */
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
    pressanykey("無法與伺服器取得連結，查詢失敗");
    return 0;
  }
  else
  {
    mouts(22, 0, "\033[1;36m正在連接伺服器，請稍後.....................\033[m");
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
    getdata(b_lines, 0, "清除(C) 移至備忘錄(M) [C]？", ans, 3, LCECHO,0);
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
        strcpy(mymail.title, "運勢預測結果");
        strcpy(mymail.owner, "[備 忘 錄]");
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

  /* Thor.991019: 載入 ATTR 資料 */
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
  move(16, 0); prints("姓名：%-12s   性別 <1>男 <2>女：%s", UserName, Sex);
  move(17, 0); prints("血型 <1>O型 <2>A型 <3>B型 <4>AB型：%s       ", Blood);
  move(18, 0); prints("出生日期：西元(民國+1911=西元) %5s年 %3s月 %3s日",Birth_year, Birth_month, Birth_day);
#if 0
  mouts(16, 0, "姓名：               性別 <1>男 <2>女：");
  mouts(17, 0, "血型 <1>O型 <2>A型 <3>B型 <4>AB型：       ");
  mouts(18, 0, "出生日期：西元(民國+1911=西元)      年    月    日");
#endif
#if 0
  mouts(19, 0, "預測日期 <1>本週 <2>自己決定：   西元      年    月    日");
#endif

  for(;;)
  {
    if(*Sex && *Blood && *Birth_year && *Birth_month && *Birth_day)
    {
      if(answer("資料正確嗎(Y/N)?[Y]") != 'n')
        break;
    }

    if(!getdata(16,21, "性別 <1>男 <2>女：",Sex , 2, GCARRY,0)
     ||!getdata(17, 0, "血型 <1>O型 <2>A型 <3>B型 <4>AB型：", Blood, 2, GCARRY,0)
     ||!getdata(18, 0, "出生日期：西元(民國+1911=西元) ",Birth_year , 5, GCARRY,0)
     ||!getdata(18,36, "年 ",Birth_month , 3, GCARRY,0)
     ||!getdata(18,42, "月",Birth_day , 3, GCARRY,0))
    {
       pressanykey("ㄟ...資料是一定要填的.....@_@");
       return 0;
    }
    mouts(18, 48, "日");
  }

  /* Thor.991019: 儲存 ATTR 資料 */
  sprintf(attr, "%04d%02d%02d%c%c", atoi(Birth_year), atoi(Birth_month), atoi(Birth_day), Sex[0] == '1' ? 'M' : 'F', Blood[0] == '1' ? 'O' : 'A' + (Blood[0] - '2'));


    pressanykey("put..");
  attr_put(cuser.userid, ATTR_BIRTH_MFB, attr);

#if 0
  if(!getdata(19, 0, "預測日期 <1>本週 <2>自己決定 [1]：",Kind, 2, LCECHO,0))

  if (!strcmp(Kind,"2"))
  {
    if(!getdata(19,37, "西元 ",Hot_year , 5, LCECHO,0)
     ||!getdata(19,47, "年 ",Hot_month , 3, LCECHO,0)
     ||!getdata(19,53, "月",Hot_day , 3, LCECHO,0))
    {
       pressanykey("ㄟ...資料是一定要填的.....@_@");
       return 0;
    }
    mouts(19, 58, "日");
  }
  else
  {
    *Hot_year = *Hot_month = *Hot_day = '\0';
  }
#endif
  strcpy(Kind,"1");
  *Hot_year = *Hot_month = *Hot_day = '\0';

  if(answer("確定要知道你的運勢？[y] ") == 'n')
  {
    pressanykey("hmm.....我不信這一套啦...^o^");
    return 0;
  }
  else
  {

    url_encode(Name, UserName);

    sprintf(atrn, "workdir=%2Fwkluck%2F&name=%s&sex=%s&blood=%s&year=%s&month=%s&day=%s&kind=%s&hotyear=%s&hotmonth=%s&hotday=%s",Name ,Sex ,Blood ,Birth_year ,Birth_month ,Birth_day ,Kind ,Hot_year ,Hot_month ,Hot_day);

    /* Thor.990330: 加上 log */
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
  "╭═╦═╮",
  "║  ║  ║",
  "╰  ║  ╯",
  "    ║    "
},
{
  "╰╮  ╭╯",
  "╭╩═╩╮",
  "║      ║",
  "╰═══╯"
},
{
  "═╦═╦═",
  "  ║  ║  ",
  "  ║  ║  ",
  "═╩═╩═",
},
{
  "╭═══╮",
  "╠╮  ╭╮",
  "╰╯  ╰╣",
  "╰═══╯",
},
{
  "╭═══╮",
  "╰╮  ╭╯",
  "╭╣  ║  ",
  "╰╯  ╰╯",
},
{
  "╭╦╦╦╮",
  "  ║║╭╯",
  "  ║║║╮",
  "  ╰╰╰╯",
},
{
  "╭═══╮",
  "╰╮  ╭╯",
  "═╯  ╰═",
  "═════",
},
{
  "╭╦╦╮  ",
  "  ║║║  ",
  "  ║║║╦",
  "  ╰╰╰╯",
},
{
  "  ╭╦╮  ",
  "╭╯║╰╮",
  "╯  ║  ╰",
  "  ═╬═  ",
},
{
  "╮╭╭═╮",
  "║║╠═╯",
  "║║╰═╮",
  "╰╯╰═╯",
},
{
  "╭╮╭╮╭",
  "╯╰╯╰╯",
  "╭╮╭╮╭",
  "╯╰╯╰╯",
},
{
  "═╮  ╭═",
  "═╬═╬═",
  "  ║  ║  ",
  "═╯  ╰═",
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
          break; /* Thor: 撞到 */
      }
      if(j == i) break; /* Thor: 都沒撞到 */
      if(++k > 10000) goto retry; /* Thor: 重來 */
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
  outs("\033[1;37;44m◎ www.big.com.tw運勢預測系統 ◎\033[m");

  show_image();

  fortuneQuery();
  return 0;
}
