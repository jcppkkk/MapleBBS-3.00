/*-------------------------------------------------------*/
/* railway.c    ( NTHU CS MapleBBS Ver 3.10 )            */
/*-------------------------------------------------------*/
/* author : kepler.bbs@bbs.cs.nthu.edu.tw                */
/* target : 台鐵火車時刻表查詢                           */
/* create : 99/03/30                                     */
/* update :   /  /                                       */
/*-------------------------------------------------------*/

#include "../include/bbs.h"

#define mouts(y,x,s)    { move(y,x); outs(s); }

#define SERVER_railway     "www.railway.gov.tw"
#define HTTP_PORT	80

#define CGI_railway     "/cgi-bin/timetk.cgi"

#define REFER_railway_1      "http://db.twtraffic.com/train/tai1w.html"

#define PARA "\
Connection: Keep-Alive\r\n\
User-Agent: Lynx/2.6  libwww-FM/2.14\r\n\
Content-type: application/x-www-form-urlencoded\r\n\
Accept: text/html, text/plain, application/x-wais-source, application/html, */*\r\n\
Accept-Encoding: gzip\r\n\
Accept-Language: en\r\n\
Accept-Charset: iso-8859-1,*,utf-8\r\nHost: www.railway.gov.tw:80\r\n"


static int
data_ready(int sockfd)
{
  static struct timeval tv = {1, 100};
  /* Thor.980806: man page 假設 timeval struct是會改變的 */ 
 
  int cc;
  fd_set rset; 
 
  for(;;)
  {
    struct timeval tmp_tv = tv;
    FD_ZERO(&rset); 
    FD_SET(0, &rset);
    FD_SET(sockfd, &rset);
    cc = select(sockfd + 1, &rset, NULL, NULL, &tmp_tv);
    if(cc > 0)
    {
      if(FD_ISSET(0, &rset))
        return 0; /* Thor.990401: user break */
      if(FD_ISSET(sockfd, &rset))
        return 1; /* Thor.990401: data ready */
    }
    else if(cc == 0) /* Thor.990401: time out */
    {
      refresh();
    }
  }
}


static int
http_conn(char *server, char *s)
{
  int sockfd, start_show;
  int cc, tlen;
  char *xhead, *xtail, tag[8], fname[50];
  static char pool[2048];
  FILE *fp;

  if ((sockfd = dns_open(server, HTTP_PORT)) < 0)
  {
    pressanykey("無法與伺服器取得連結，查詢失敗");
    return 0;
  }
  else
  {
    /* mouts(22, 0, "\033[1;36m正在連接伺服器，請稍後(按任意鍵離開).............\033[m"); */
    mouts(22, 0, "正在連接伺服器，請稍後(按任意鍵離開).............");
    refresh();
  }

  write(sockfd, s, strlen(s));
  shutdown(sockfd, 1);

  /* parser return message from web server */
  xhead = pool;
  xtail = pool;
  tlen = 0;
  start_show = 0;

  /* usr_fpath(fname, cuser.userid,Msg_File); */
  sprintf(fname, "tmp/%s.railway", cuser.userid);

  fp = fopen(fname, "w");
  for (;;)
  {
    if (xhead >= xtail)
    {
      xhead = pool;
#if 1
      /* Thor.990401: 無聊一下 */
      if(!data_ready(sockfd))
      {
        close(sockfd);
        fclose(fp);
        unlink(fname);
        return 0;
      }
#endif
      cc = read(sockfd, xhead, sizeof(pool));
      if (cc <= 0)
	break;
      xtail = xhead + cc;
    }

    cc = *xhead++;

    if ((tlen == 5) && (!str_ncmp(tag, "body", 4)))
    {
      start_show = 1;
      /* continue; */
    }

    if (cc == '[')
      start_show = 0;

    if ((tlen == 3) && (!str_ncmp(tag, "td", 2)))
      fputc(' ', fp);

    if (cc == '<')
    {
      tlen = 1;
      continue;
    }

    if (tlen)
    {
      /* support<br>and<P>and</P> */

      if (cc == '>')
      {
	if ((tlen == 3) && (!str_ncmp(tag, "tr", 2)))
	{
	  fputc('\n', fp);
	}
	else if ((tlen == 2) && (!str_ncmp(tag, "P", 1)))
	{
	  fputc('\n', fp);
	}
	else if ((tlen == 3) && (!str_ncmp(tag, "br", 2)))
	{
	  fputc('\n', fp);
	}

	tlen = 0;
	continue;
      }

      if (tlen <= 5)
      {
	tag[tlen - 1] = cc;
      }

      tlen++;
      continue;
    }
    if (start_show)
    {
      if (cc != '\r' && cc != '\n')
	fputc(cc, fp);
    }
  }

  close(sockfd);
  fputc('\n', fp);
  fclose(fp);

  /* show message that return from the web server */

  /* usr_fpath(fname, cuser.userid,Msg_File); */

  if (more(fname, YEA) != -1) 
  {
    do 
    {
      getdata(b_lines - 1, 0, "清除(C) 移至備忘錄(M) (C/M)？", tag, 3, LCECHO,"c");
      if (*tag == 'c' || *tag == 'C') 
      {
        unlink(fname);
        return 0;
      }
      if (*tag == 'm' || *tag == 'M') 
      {
        fileheader mymail;
        char title[128], buf[80];
        sethomepath(buf, cuser.userid);
        stampfile(buf, &mymail);
        mymail.savemode = '\0';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.title, "西部幹線火車時刻查詢結果");
        strcpy(mymail.owner, "[備 忘 錄]");
        sethomedir(title, cuser.userid);
        rec_add(title, &mymail, sizeof(mymail));
        f_mv(fname, buf);
        return 0;
      }
    } while (1);
  }

  return 0;
}


static int
railway(char *REF)
{
  char from_station[10], to_station[10];
  char atrn[256], ans[2], sendform[512],num[140];
  char Ft[30], Tt[30], type[8], tt[8];
  int from_time, to_time , i;

  mouts(14,0,"\n請留言 (至多三行)，按[Enter]結束");
  for (i = 0; (i < 3) &&
  getdata(16 + i, 0, "：", num, 70, DOECHO,0); i++);
              
  if(!getdata(19, 0, "到達站-->", to_station, 9, DOECHO, 0))
  {
    pressanykey("hmm...不想查囉....^o^");
    return 0;
  }
  strcpy(from_station, "台中");
  if(!getdata(20, 0, "查詢時間：  ", atrn, 3, LCECHO, 0))
  {
    from_time = 0;
    mouts(20, 12, "00");
  }
  else
    from_time = atoi(atrn);

  mouts(20, 14, ":00 ");

  if(!getdata(20, 18, "至 ", atrn, 3, LCECHO, 0))
  {
    to_time = 24;
    mouts(20, 21, "24");
  }
  else
    to_time = atoi(atrn);

  mouts(20, 23, ":00 ");

  strcpy(tt, "start");

  getdata(21, 0, "查詢車種：<1>對號快車  <2>普通平快：[1] ", ans, 3, LCECHO, 0);
  if(ans[0] != '2')
    strcpy(type, "fast");
  else
    strcpy(type, "slow");

  getdata(22, 0, "確定要開始查詢？[y] ", ans, 3, LCECHO, 0);
  if (ans[0] == 'n')
  {
    pressanykey("hmm.....不想查囉...^o^");
    return 0;
  }
  else
  {

    url_encode(Ft, from_station);
    url_encode(Tt, to_station);

    sprintf(atrn, "from-station=%s&to-station=%s&from-time=%02d00&to-time=%02d00&tt=%s&type=%s", Ft, Tt, from_time, to_time, tt, type);

    /* Thor.990330: 加上 log */
    log_usies("RAILWAY",atrn);

    sprintf(sendform, "POST %s HTTP/1.0 Referer: %s\n%sContent-length:%d\n\n%s", CGI_railway, REF, PARA, strlen(atrn), atrn);

    http_conn(SERVER_railway, sendform);
    return 0;
  }
}

int 
railway2()
{

  clear();
  showtitle("時刻查詢", "test◎ 台鐵火車時刻表查詢系統 ◎");
  move(3, 0);

  outs("
            ▃ ▃      ≒ ≒       ▃ ▃      ▃     ▃
      ◢█████████████████〣〣████████████████
    ／  \033[30;41m■\033[31;40m██≡██████████≡██\033[31;40m █ \033[30;41m╭─╮\033[31;40m█    ██       ██
  \033[30;47m◤\033[37;41m◤\033[31;40m█▉████████████████ █ \033[30;41m│  │\033[31;40m█    ██       ██
\033[30;41m◤  \033[31;47m◤\033[37;40m█▉██████\033[34;47m㊣\033[37;40m█████████〣〣\033[30;47m╰─╯\033[37;40m█████████████
◥▓◤◎≡≡◎◥▓▓▓▓▓▓◤◎≡≡◎◥▓    ▓◤◎≡≡◎◥▓︺︺︼__∞≡≡∞
");

      railway(REFER_railway_1);
  return 0;
}
//      more("etc/railway.1", NA);  