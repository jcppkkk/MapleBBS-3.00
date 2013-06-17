/*-------------------------------------------------------*/
/* railway.c    ( NTHU CS MapleBBS Ver 3.10 )            */
/*-------------------------------------------------------*/
/* author : kepler.bbs@bbs.cs.nthu.edu.tw                */
/* target : �x�K�����ɨ��d��                           */
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
  /* Thor.980806: man page ���] timeval struct�O�|���ܪ� */ 
 
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
    pressanykey("�L�k�P���A�����o�s���A�d�ߥ���");
    return 0;
  }
  else
  {
    /* mouts(22, 0, "\033[1;36m���b�s�����A���A�еy��(�����N�����}).............\033[m"); */
    mouts(22, 0, "���b�s�����A���A�еy��(�����N�����}).............");
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
      /* Thor.990401: �L��@�U */
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
      getdata(b_lines - 1, 0, "�M��(C) ���ܳƧѿ�(M) (C/M)�H", tag, 3, LCECHO,"c");
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
        strcpy(mymail.title, "�賡�F�u�����ɨ�d�ߵ��G");
        strcpy(mymail.owner, "[�� �� ��]");
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

  mouts(14,0,"\n�Яd�� (�ܦh�T��)�A��[Enter]����");
  for (i = 0; (i < 3) &&
  getdata(16 + i, 0, "�G", num, 70, DOECHO,0); i++);
              
  if(!getdata(19, 0, "��F��-->", to_station, 9, DOECHO, 0))
  {
    pressanykey("hmm...���Q�d�o....^o^");
    return 0;
  }
  strcpy(from_station, "�x��");
  if(!getdata(20, 0, "�d�߮ɶ��G  ", atrn, 3, LCECHO, 0))
  {
    from_time = 0;
    mouts(20, 12, "00");
  }
  else
    from_time = atoi(atrn);

  mouts(20, 14, ":00 ");

  if(!getdata(20, 18, "�� ", atrn, 3, LCECHO, 0))
  {
    to_time = 24;
    mouts(20, 21, "24");
  }
  else
    to_time = atoi(atrn);

  mouts(20, 23, ":00 ");

  strcpy(tt, "start");

  getdata(21, 0, "�d�ߨ��ءG<1>�︹�֨�  <2>���q���֡G[1] ", ans, 3, LCECHO, 0);
  if(ans[0] != '2')
    strcpy(type, "fast");
  else
    strcpy(type, "slow");

  getdata(22, 0, "�T�w�n�}�l�d�ߡH[y] ", ans, 3, LCECHO, 0);
  if (ans[0] == 'n')
  {
    pressanykey("hmm.....���Q�d�o...^o^");
    return 0;
  }
  else
  {

    url_encode(Ft, from_station);
    url_encode(Tt, to_station);

    sprintf(atrn, "from-station=%s&to-station=%s&from-time=%02d00&to-time=%02d00&tt=%s&type=%s", Ft, Tt, from_time, to_time, tt, type);

    /* Thor.990330: �[�W log */
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
  showtitle("�ɨ�d��", "test�� �x�K�����ɨ��d�ߨt�� ��");
  move(3, 0);

  outs("
            �d �d      �� ��       �d �d      �d     �d
      ���i�i�i�i�i�i�i�i�i�i�i�i�i�i�i�i�i�ŢŢi�i�i�i�i�i�i�i�i�i�i�i�i�i�i�i
    ��  \033[30;41m��\033[31;40m�i�i�ݢi�i�i�i�i�i�i�i�i�i�ݢi�i\033[31;40m �i \033[30;41m�~�w��\033[31;40m�i    �i�i       �i�i
  \033[30;47m��\033[37;41m��\033[31;40m�i�p�i�i�i�i�i�i�i�i�i�i�i�i�i�i�i�i �i \033[30;41m�x  �x\033[31;40m�i    �i�i       �i�i
\033[30;41m��  \033[31;47m��\033[37;40m�i�p�i�i�i�i�i�i\033[34;47m��\033[37;40m�i�i�i�i�i�i�i�i�i�Ţ�\033[30;47m���w��\033[37;40m�i�i�i�i�i�i�i�i�i�i�i�i�i
���������ݡݡ��������������������ݡݡ�����    �������ݡݡ������h�h�l__�ۡݡݡ�
");

      railway(REFER_railway_1);
  return 0;
}
//      more("etc/railway.1", NA);  