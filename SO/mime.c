/*-------------------------------------------------------*/
/* mime.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* author : visor.bbs@bbs.yzu.edu.tw			 */
/* author : statue.bbs@bbs.yzu.edu.tw			 */
/* target : mime				 	 */
/* create : 99/12/26				 	 */
/* update : 99/12/26				 	 */
/*-------------------------------------------------------*/

#undef	_MIME_C_


#include "bbs.h"
#ifdef HAVE_MIME_TRANSFER
extern XZ xz[];

static FILE *fsock; /* �s�u�Ϊ� FILE */
static int is_base64;
static int is_qp;
#define MIME_BASE64	0x01
#define MIME_QP		0x02
#define MIME_MIME       0x10

static int del_num; /* �Ψ� UnDelete �� , �w�R���峹��*/

/* ----------------------------------------------------- */
/* QP code : "0123456789ABCDEF"                          */
/* QP �O�@�ظѽX�覡 quoted-printable			 */
/* �@�g�峹���� '=' ���S���W�L�@�w�ƥ�.			 */
/* ----------------------------------------------------- */

static int
qp_code(x)
  register int x;
{
  if (x >= '0' && x <= '9')
    return x - '0';
  if (x >= 'a' && x <= 'f')
    return x - 'a' + 10;
  if (x >= 'A' && x <= 'F')
    return x - 'A' + 10;
  return -1;
}

/* ------------------------------------------------------------------ */
/* BASE64 :                                                           */
/* "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" */
/* Base64�L�k�P�_, ���D�ڭ̦�header				      */
/* ------------------------------------------------------------------ */

static int
base64_code(x)
  register int x;
{
  if (x >= 'A' && x <= 'Z')
    return x - 'A';
  if (x >= 'a' && x <= 'z')
    return x - 'a' + 26;
  if (x >= '0' && x <= '9')
    return x - '0' + 52;
  if (x == '+')
    return 62;
  if (x == '/')
    return 63;
  return -1;
}

/* ----------------------------------------------------- */
/* judge & decode QP / BASE64                            */
/* ----------------------------------------------------- */

/* �h�������n�����Y */
int mime_cut(unsigned char* src,HDR *hdr, FILE *fp)
{
  char *ptr;

  /* �Y�S���}�� MIME �ѽX�ﶵ�h�����ѽX */
  if((cuser.ufo & UFO_MIME) == 0)
    return 1;

  if(!strncmp(src, "----", strlen("----"))||
     !strncmp(src, "\tboundary", strlen("\tboundary")))
    return 0; /* ���L�������ѽX */

  if(!strncmp(src, "Content-Type: ", strlen("Content-Type: ")))
  {
    ptr = src + strlen("Content-Type: ");
    if(!strncmp(ptr, "text/html",strlen("text/html")))
    {
      hdr->xid |= MIME_MIME;
      
      fprintf(fp, "html �������榡\n");
      return 0;
    }
    /*if(!strncmp(ptr, "text/plain",strlen("text/plain")))
    {
      hdr->xid |= MIME_MIME;
      fprintf(fp, "text/plain ������ : �¤�r��\n");
    }*/
    if(!strncmp(ptr, "image",strlen("image")))
    {
      hdr->xid |= MIME_MIME;
      fprintf(fp, "image ������ : �Ϥ���\n");
      return 0;
    }
    if(!strncmp(ptr, "audio",strlen("audio")))                                  
    {
      hdr->xid |= MIME_MIME;
      fprintf(fp, "audio ������ : ������\n");
      return 0;
    }
    if(!strncmp(ptr, "video",strlen("video")))                                  
    {
      hdr->xid |= MIME_MIME;
      fprintf(fp, "video ������ : �v����\n");
      return 0;
    }
    if(!strncmp(ptr, "application",strlen("application")))
    {
      hdr->xid |= MIME_MIME;
      fprintf(fp, "application ������ : �����榡\n");
      return 0;
    }
    hdr->xid &= MIME_QP|MIME_BASE64;
    
    /* �J�� text or image �����ѽX�ʧ@ */ 
    fgets(src, 256, fsock);
    if(*src == '\t') /* Contect-Type �ᦳ boundary & charset,�e�|�� tab */
    {
      return 0;
    }
  }

  if(!strncmp(src, "Content-Transfer-Encoding: ", strlen("Content-Transfer-Encoding: ")))
  { /* �� rfc1522 body code */
    ptr = src + strlen("Content-Transfer-Encoding: ");
    if(!strncmp(ptr, "base64", strlen("base64")))
      hdr->xid |= MIME_BASE64;
    if(!strncmp(ptr, "quoted-printable", strlen("quoted-printable")))
      hdr->xid |= MIME_QP;
    return 0;
  }
  if(hdr->xid & MIME_MIME)
    return 0;

  return 1; /* �i��ѽX�ʧ@ */
}

/* �ѽX , �� bmtad.c �� multipart() �ܹ�*/
char*
mime_transfer(unsigned char* src)
{
  unsigned char* dst = src;
  char *ans;
  register int is_done;
  register int c1, c2, c3, c4;

  /* �Y�S���}�� MIME �ѽX�ﶵ�h�����ѽX */
  if((cuser.ufo & UFO_MIME) == 0)
    return src;
  
  ans = src;

   /* �P�_�e 10 �Ӧr���O�_���bbase64 �� */
  for (c1 = 0; c1 < 10 ; c1++)
     if (base64_code(src[c1]) == -1) 
     {
        is_base64 = 0;
        break;
     }

  for (is_done = 0; c1 = *src; src++) /* ��Ƕi�Ӫ��ȧ@�ѽX */
  {
    if (c1 == '?' && src[1] == '=') /* qb code �� To: From: */
    {
      if(src[2] == ' ') /* �q�` qb code �� base64 ���ĤT�Ӥ��|�O�ť� */
      {
        src++;
        is_qp = is_base64 = 0;
      }
      continue;
    }
    else if ((c1 == '=') && (src[1] == '?'))
    {
      /* c2 : qmarks, c3 : code_kind */

      c2 = c3 = 0;

      for (;;)
      {
        c1 = *++src;
        if (c1 != '?')
        {
          if (c2 == 2)
            c3 = c1 | 0x20;
        }
        else
        {
          if (++c2 >= 3)
            break;
        }
      }

      if (c3 == 'q') /* =?q */
        is_qp = MIME_QP;
      else if (c3 == 'b') /* =?big5? */
        is_base64 = MIME_BASE64;
    }
    else if (c1 == '\n')        /* chuan: multi line encoding */
    {
      if (!is_base64)
         *dst++ = c1;
      is_done = is_qp = is_base64 = 0;
      continue;
    }
    else if (is_qp && c1 == '=')
    {
      c1 = *++src;
      if (!*src)
         break;
      c2 = *++src;
      if (!*src)
         break;
      *dst++ = (qp_code(c1) << 4) | qp_code(c2);
      if (dst[-1] == (unsigned char)-1)
         --dst;
    }
    else if (is_base64 && !is_done)
    {
      while (isspace(c1))
      {
        c1 = *++src;
      }
      if (!c1)
        break;
      do
      {
        c2 = *++src;
      } while (isspace(c2));
      if (!c2)
        break;
      do
      {
        c3 = *++src;
      } while (isspace(c3));
      if (!c3)
        break;
      do
      {
        c4 = *++src;
      } while (isspace(c4));
      if (!c4)
        break;
      if (c1 == '=' || c2 == '=')
      {
        is_done = 1;
        continue;
      }
      c2 = base64_code(c2);
      *dst++ = (base64_code(c1) << 2) | ((c2 & 0x30) >> 4);
      if (c3 == '=')
        is_done = 1;
      else
      {
        c3 = base64_code(c3);
        *dst++ = ((c2 & 0xF) << 4) | ((c3 & 0x3c) >> 2);
        if (c4 == '=')
          is_done = 1;
        else
        {
          *dst++ = ((c3 & 0x03) << 6) | base64_code(c4);
        }
      }
    }
    else
      *dst++ = c1;
  }
  if (!is_base64 && dst[-1] == '=')
     dst[-1] = '\n';
  *dst = '\0';
  return ans;
}

char *
parse_nick(token)
  char *token;
{
  char *ptr;

  /* =?big5?B?qHSyzrresnqt+w==?= <postmaster@mail86.yzu.edu.tw> */

  ptr = token;
  if(*ptr == '=')
  {
    ptr = strchr(token, '<'); /* <postmaster@mail86.yzu.edu.tw> */
    if(ptr[-1] == ' ');
      ptr[-1] = 0; /* <postmaster@mail86.yzu.edu.tw> */
  }
  return token; /* =?big5?B?qHSyzrresnqt+w==?= */
}

/* ��ӷ����X�ݭn������ */
char *
parse_owner(token)
  char *token;
{
  char *ptr;

  /* =?big5?B?qHSyzrresnqt+w==?= <postmaster@mail86.yzu.edu.tw> */

  ptr = strchr(token,'>'); /* > */
  if(ptr) /* �ݳ̫�@�Ӧr���O�_�� > */
  {
    *ptr = 0; /*  */
    ptr = strchr(token, '<'); /* <postmaster@mail86.yzu.edu.tw */
    if(ptr[-1] == ' ')
      ptr[-1] = 0; /* <postmaster@mail86.yzu.edu.tw */
    *ptr++ = 0; /* postmaster@mail86.yzu.edu.tw */
  }
  else /* statue.bbs@cnpa-6.admin.yzu.edu.tw */
    ptr = token;

  return ptr;
}

/* �q header ���X�ݭn������ */
void
parse_header(hdr, ptr)
  HDR *hdr;
  char *ptr;
{
  char *token, buf[256];
  struct tm tmdate;
  
  sprintf(buf, "%s.mail", cuser.userid);
  snprintf(hdr->xname, 32, "%s", buf);
  
  strcpy(buf, ptr);
  
  hdr->xid = 0; /* �w�]���� qp | base64 �ѽX */
  /* From: */
  /* Content-Type: multipart/alternative; */
  /* Content-Type: text/plain; charset="big5" */ 
  /*         boundary="----=_NextPart_000_0007_01BF4F46.00513410" */
  /* Content-Transfer-Encoding: base64 */

 /* �n�D��ƪ������O ) ���r�� , �����D���S������n�����k statue.1227*/
 while(*buf != ')')
 {
  fgets(buf, 256, fsock);
  if(!strncmp(buf, "Content-Type: ", strlen("Content-Type: ")))
  {
    ptr = buf + strlen("Content-Type: ");
    if(!strncmp(ptr, "multipart", strlen("multipart")))
      hdr->xid |= MIME_QP;
  }

  if(!strncmp(buf, "Content-Transfer-Encoding: ", strlen("Content-Transfer-Encoding: ")))
  {
    ptr = buf + strlen("Content-Transfer-Encoding: ");
    if(!strncmp(ptr, "quoted-printable", strlen("quoted-printable")))
      hdr->xid |= MIME_QP;
    if(!strncmp(ptr, "base64", strlen("base64")))
      hdr->xid |= MIME_BASE64;
  }

  /* �@�� */
  /* From: =?big5?B?qEirVL+z?= <s874070@mail.yzu.edu.tw> */
  if(!strncmp(buf, "From: ", strlen("From: ")))
  {
    char tmp[256];
    token = ptr = buf + strlen("From: ");
    strcpy(tmp,token);
    buf[strlen(buf)-2] = 0;
    
    /* �n���n is_base64 | is_qb */
    snprintf(hdr->nick, 50, "%s", mime_transfer(parse_nick(token)));
    /* �ӷ��ݨ��X�u�ݭn������ <s874070@mail.yzu.edu.tw> */
    mime_transfer(tmp);
    snprintf(hdr->owner, 80, "%s", parse_owner(tmp));
  }

  /* ��� *//* Date: Sun, 26 Dec 1999 02:07:46 -0800 */
  if(!strncmp(buf, "Date: ", strlen("Date: ")))
  {
    token = ptr = buf + strlen("Date: ");
    buf[strlen(buf)-2] = 0;

    strptime(token, "%a, %d %b %Y", &tmdate);
    snprintf(hdr->date, 9, "%02d/%02d/%02d",
             tmdate.tm_year, tmdate.tm_mon+1, tmdate.tm_mday);
  }

  /* ���D *//* Subject: xxxxxx */
  if(!strncmp(buf, "Subject: ", strlen("Subject: ")))
  {
    token = ptr = buf + strlen("Subject: ");
    buf[strlen(buf)-2] = 0;
    
    is_base64 = hdr->xid & MIME_BASE64;
    is_qp = hdr->xid & MIME_QP;
    mime_transfer(ptr); /* ���D�ݸѽX */
    
    strncpy(hdr->title, token, 72);
    hdr->title[72] = 0;
  }
 }
}

int
load_dir()
{
  char buf[256];
  int total, count, step = 0, begin = 1;
  HDR *hdr;
  char smaildir[80];

  total = 0;
  hdr = NULL;

    /* * 4 EXISTS */
    /* * 0 RECENTM */
    /* * FLAGS (\Seen \Answered \Flagged \Deleted \Draft) */
    /* * OK [UIDVALIDITY 16097] UIDVALIDITY value. */
    /* ~ OK [READ-WRITE] SELECT completed. */
  strcpy(buf, "~ SELECT INBOX\r\n");
  fprintf(fsock, buf);
  while(strncmp(buf, "~ OK", 3))
  {
      fgets(buf, 256, fsock);
      if(strstr(buf, "EXISTS"))               /* meet system message */
      {
        if(!total)
          total = atoi(buf+2);
        if(total >= 256) /* �p�G�H��q�j�� 256 �g , �uŪ�� 256 �g*/
          begin = total-255;
        else if(total == 0)
        {
          vmsg("�z���q�l�l��H�c�S���H��");
          return 1;
        }
        else
          begin = 1;
      }
  }

    /* * 1 FETCH (FLAGS (\Seen)) */
    /* * 2 FETCH (FLAGS (\Seen)) */
    /* ~ OK FETCH completed. */
  count = 0;
  hdr = (HDR *)calloc(total-begin+1, sizeof(HDR));        /* memory */
  if(hdr == NULL)                                 /* fail */
    return 1;
  snprintf(buf, 256, "~ FETCH %d:%d FLAGS\r\n", begin, total);
  fprintf(fsock, buf);
  while(strncmp(buf, "~ OK", 3))
  {
    fgets(buf, 256, fsock);
    if(strstr(buf, "Seen")) /* * 1 FETCH (FLAGS (\Seen)) */
      hdr[count].xmode = MAIL_READ;
    hdr[count].chrono = begin+count;            /* mail number */
    count++;                             /* ���ɥ� HDR.chrono �Τ@�U */
  }

    /* * 1 FETCH (RFC822.HEADER {103} *//* �� HEADER lo */
    /* * 2 FETCH (RFC822.HEADER {103} */
    /* ~ OK FETCH completed. */
  count = 0;
#if 1
        snprintf(buf, 256, "~ FETCH %d:%d RFC822.HEADER.LINES \
(CONTENT-TYPE CONTENT-TRANSFER-ENCODING FROM DATE SUBJECT)\r\n", begin, total); 
#else
        snprintf(buf, 256, "~ FETCH %d:%d RFC822.HEADER.LINES \
(FROM DATE SUBJECT)\r\n", begin, total);
#endif
  fprintf(fsock, buf);
  while(strncmp(buf, "~ OK", 3))
  {
    fgets(buf, 256, fsock);
    if(*buf == '*')
      parse_header(&hdr[count], buf);
    count++;
  }

  sprintf(smaildir, "mail86/%s.DIR", cuser.userid);   /* DIRECTORY/MBOX */
  if((step = open(smaildir, O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0)
    return 1;
  write(step, hdr, sizeof(HDR) * (total-begin+1));      /* hdr to file */
  close(step);

  return 0;
}

int
mime_delete(xo)
  XO *xo;
{
  HDR *hdr;

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);

  if (hdr->xmode & MAIL_MARKED)
    return XO_NONE;

  if (!(hdr->xmode & POST_DELETE) ? 
    (vans(MSG_DEL_NY) == 'y'):(vans("�нT�w�Ϧ^(Y/N)?[N]") == 'y'))
  {
    if(hdr->xmode & POST_DELETE)
      del_num--;
    else
      del_num++;
    hdr->xmode ^= POST_DELETE;

    /* �R�����峹�۰� reload �ù� */
    if (!rec_put(xo->dir, hdr, sizeof(HDR), xo->pos))
      return XO_LOAD;
  }
  return XO_BODY;
}

static int
mime_get(fpath,hdr)
  char *fpath;
  HDR *hdr;
{
  char buf[256];
  char *ptr;
  FILE *fp;
  int total, now,ck,OK_FETCH; 
  /* ck �O�� mime transfer �� check */
  /* OK_FETCH �O��᭱���峹�짹, ���� ~ OK FETCH completed., �ɮ׵���*/

  if((fp = fopen(fpath, "w")) == NULL)
    return 0;

/* GET HEADER, �] HDR ��ƪ��פ���, �����q��@�� */
  fprintf(fsock, "~ FETCH %d RFC822.HEADER.LINES (FROM DATE)\r\n", 
    hdr->chrono);
  fgets(buf, 256, fsock);
  if(strstr(buf, " EXIST\r\n"))
  {
    fgets(buf, 256, fsock);
    fgets(buf, 256, fsock);
  }
  fgets(buf, 256, fsock);
  buf[strlen(buf)-2] = 0;

  is_qp = hdr->xid & MIME_QP;
  is_base64 = hdr->xid & MIME_BASE64;
  mime_transfer(buf);
  
  strncpy(buf, "�@��", 4); /* �@�̪��ʺ٥i�঳�[�K�ʧ@ */
  fprintf(fp, "%s\n", buf); /* �� From �令 �@�� */
  fprintf(fp, "���D: %s\n", hdr->title);
  fgets(buf, 256, fsock);
  if(*buf != '\r')
  {
    strncpy(buf, "�ɶ�", 4);
    fprintf(fp, "%s\n\n", buf);
  }
  else
    fprintf(fp, "�ɶ�: ����\n\n");
  /* From: "s874070" <s874070@mail.yzu.edu.tw> */
  /* Date: Sun, 26 Dec 1999 02:07:46 -0800*/


/* GET BODY */
  fprintf(fsock, "~ FETCH %d RFC822.TEXT\r\n", hdr->chrono);
  fgets(buf, 256, fsock);
  /* * 1 FETCH (RFC822.TEXT {624} */
  if(strstr(buf, " EXIST\r\n"))
  {
    fgets(buf, 256, fsock);
    fgets(buf, 256, fsock);
  }
  if(ptr = strrchr(buf, '{'))
    total = atoi(ptr+1)+1;                      /* total = size */
    
  now = 0;
  OK_FETCH=1; /* ���峹���� */
  while(1)
  {
    if(now >= total)
    {
      OK_FETCH = 0;
      break;
    }
    fgets(buf, 256, fsock);

    if(strstr(buf, "~ OK FETCH completed"))
    {
      OK_FETCH = 0;
      ck = -1;
      break;
    }

    now += strlen(buf);
    ck = mime_cut(buf, hdr, fp); /* �h�������n�����Y */

    if(ck == 1) /* �H mime_cut ���Ǧ^���P�_�O�_�ݭn�ѽX */
    {

      is_qp = hdr->xid & MIME_QP;
      is_base64 = hdr->xid & MIME_BASE64;
      mime_transfer(buf); /* �ѽX�ʧ@ */
      
      fprintf(fp, "%s", buf);
    }
    else if (ck == -1) 
      break;
  }
  
  while(OK_FETCH && fgets(buf, 256, fsock))
  {
     if(strstr(buf, "~ OK FETCH completed"))
       break;
  }

  if(hdr->xid && (cuser.ufo & UFO_MIME))
  {
    fprintf(fp, "\n�o�O�@�ʸg�L MIME �ѽX�����A�p�����D�гq���t�κ޲z�̡C\n");
  }

  fclose(fp);
  return 1;
}

static int
mime_browse(xo)
  XO *xo;
{
  char  file[32];
  HDR *hdr;

  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  
  sprintf(file, "mail86/%s.mail", cuser.userid);             /* Tsaoyc.mail */

  if(!mime_get(file, hdr))
    return XO_NONE;

  strncpy(currtitle, hdr->title, 40); /* title �u���Ϋe 40 �� ? */

  more(file, NULL);
  hdr->xmode |= MAIL_READ;

  return XO_HEAD;
}

static inline int
mbox_attr(type)
  int type;
{
  if (type & MAIL_DELETE)
    return 'D';

  if (type & MAIL_REPLIED)
    return (type & MAIL_MARKED) ? 'R' : 'r';

  return "+ Mm"[type & 3];
}

static int
mime_mark(xo)
  XO *xo;
{
  HDR *mhdr;
  int cur, pos;

  pos = xo->pos;
  cur = pos - xo->top;
  mhdr = (HDR *) xo_pool + cur;
  move(3 + cur, 6);
  outc(mbox_attr(mhdr->xmode ^= MAIL_MARKED));
  rec_put(xo->dir, mhdr, sizeof(HDR), pos);
  return XO_NONE;
}

static void
mime_item(pos, hdr)
  int pos;
  HDR *hdr;
{
  int xmode = hdr->xmode;
  prints(xmode & MAIL_DELETE ? "%5d \033[1;5;37;41m%c\033[m"
    : xmode & MAIL_MARKED ? "%5d \033[1;36m%c\033[m"
    : "%5d %c", pos, mbox_attr(hdr->xmode));

  hdr_outs(hdr, 47);
}

static int
mime_body(xo)
  XO *xo;
{
  HDR *mime;
  int num, max, tail;

  max = xo->max;
  
  if (max <= 0)
  {
    vmsg("�z���q�l�l��H�c�S���H��");
    return XO_QUIT;
  }

  mime = (HDR *) xo_pool;
  num = xo->top;
  tail = num + XO_TALL;
  if (max > tail)
    max = tail;

  move(3, 0);  
  do
  {
    mime_item(++num, mime++);
  } while (num < max);
  clrtobot();

  return XO_NONE;
}

static int
mime_head(xo)
  XO *xo;
{
  vs_head("\0�l����", str_site);
  outs("\
[��]���} [��,r]Ū�H [d]�R��/�Ϧ^ [S]��Ū [R]�^�H [x]��F [h]����\n\033[44m\
 �s��   �� ��  �@ ��          �H  ��  ��  �D                                  \033[m");
 
  return mime_body(xo);
}

static int
mime_load(xo)
  XO *xo;
{
  xo_load(xo, sizeof(HDR));
  return mime_body(xo);
}

static int
mime_init(xo)
  XO *xo;
{
  xo_load(xo, sizeof(HDR));
  return mime_head(xo);
}

static int
mime_help(xo)
  XO *xo;
{
  film_out(FILM_MIME, -1);
  return mime_head(xo);
}

void
log_usies(mode, mesg, fileusage)
  char *mode, *mesg, *fileusage;
{
  FILE *fp;

  if (fp = fopen(fileusage, "a+"))
  {
    time_t now = time(0);
    struct tm *p;

    p = localtime(&now);
    fprintf(fp, "%02d/%02d %02d:%02d:%02d %-8s %-14s  %s\n",
      p->tm_mon + 1, p->tm_mday,
      p->tm_hour, p->tm_min, p->tm_sec, mode, cuser.userid, mesg);

    fclose(fp);
  }
}

int
mime_send(xo)
  XO *xo;
{
  sprintf(quote_file,"mail86/%s.mail",cuser.userid);
  m_send();
  return mime_head(xo);
}

static int
mime_reply(xo)
  XO *xo;
{
  HDR *hdr;
  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  sprintf(quote_file,"mail86/%s.mail",cuser.userid); 
  if(!mime_get(quote_file,hdr))
    return XO_NONE;  
  mail_reply(hdr);
  *quote_file = '\0';
  return XO_HEAD;
}

static int
mime_forward(xo)
  XO *xo;
{
  HDR *hdr,mhdr;
  char fpath[80],rcpt[64],folder[128];
  int userno;
  
  hdr = (HDR *) xo_pool + (xo->pos - xo->top);
  strcpy(rcpt,cuser.email);
  sprintf(fpath,"mail86/%s.mail",cuser.userid);
  if(!mime_get(fpath,hdr))
    return XO_NONE;  
  if (!vget(b_lines, 0, "�ت��a�G", rcpt, sizeof(rcpt), GCARRY))
    return XO_FOOT;

  if(mail_external(rcpt))  
    bsmtp(fpath, hdr->title, rcpt, MQ_MIME);
  else
  {
     if ((userno = acct_userno(rcpt)) <= 0)
        return -1;
     usr_fpath(folder, rcpt, fn_dir);
     hdr_stamp(folder, HDR_LINK, &mhdr, fpath);
     strcpy(mhdr.owner, cuser.userid);
     strcpy(mhdr.nick, cuser.username);   
     strcpy(mhdr.title, hdr->title);
     rec_add(folder, &mhdr, sizeof(mhdr));
     m_biff(userno);
  }
  return XO_HEAD;

}

static int
mime_load_dir(xo)
  XO *xo;
{
  if(load_dir())
  {
    fclose(fsock);
    vmsg("��Ū����");
    return XO_QUIT;
  }
  del_num = 0; /* ���]�w�R���峹�� 1 */
  return XO_INIT;
}

KeyFunc mime_cb[] =
{
  XO_INIT, mime_init,
  XO_LOAD, mime_load,
  XO_HEAD, mime_head,
  XO_BODY, mime_body,

  'r', mime_browse,
  'd', mime_delete,
  'R', mime_reply,
  's', mime_send,
  'S', mime_load_dir,
  'x', mime_forward,
  'm', mime_mark,
  'h', mime_help
};

static int
server_login(xo)
{
  char buf[256], user[60], pass[20];
  int sock = 143;

  more("etc/mail86", (char *)-1);

  strcpy(user,cuser.email); /* �۰��� user �[�W�b���W */
  *(strchr(user,'@')) = 0;

  if(!vget(20, 0, "�b��: ", user, 20, GCARRY) ||
     !vget(21, 0, "�K�X: ", pass, 20, NOECHO) )
  {                                             /* ���@���S��J�h��� */
    return -1;
  }

  if(user[2] < '5')
    strcpy(buf, "graduate.yzu.edu.tw");
  else if((user[2] == '6') || (user[2] == '7'))
    strcpy(buf, "cloud.yzu.edu.tw");
  else if((user[2] == '8') || (user[2] == '5'))
    strcpy(buf, "wind.yzu.edu.tw");

  if(Get_Socket(buf, &sock))
  {
    vmsg("�D���L�k�s�u");
    return 1;
  }

  fsock = fdopen(sock, "r+");
  fgets(buf, 256, fsock);                                  /* Welcome */
  fprintf(fsock, "BBS LOGIN %s \"%s\"\r\n", user, pass);   /* LOGIN */

  fgets(buf, 256, fsock);
  /* BBS OK LOGIN completed.*/
  if(strncmp(buf, "BBS OK", 6))                         /* Verify */
  {
    log_usies("-NO-", user, "run/mailxx.log");            /* �n�J���ѰO�� */
    vmsg("�n�J����");
    fclose(fsock);
    return 1;
  }
  else
    log_usies("-OK-", user, "run/mailxx.log");            /* �n�J���\�O�� */

  if(load_dir())
  {
    fclose(fsock);
    return 1;
  }
  return 0;
}

static int
server_logout(xo)
  XO *xo;
{
  if(del_num && vans(msg_del_ny) == 'y')
  {  
    int pos=0,fd;
    HDR hdr;
    char buf[200];
    while(fd = open(xo->dir, O_RDONLY))
    {
      lseek(fd, (off_t) (sizeof(HDR) * pos), SEEK_SET);
      if (read(fd, &hdr, sizeof(hdr)) == sizeof(hdr))
      {
        if(hdr.xmode & POST_DELETE)
        {
          fprintf(fsock, "~ STORE %d +FLAGS (\\Deleted)\r\n", hdr.chrono);
          while(1)
            if(!strncmp(fgets(buf, 200, fsock), "~ OK", 3))/* purge messages */
              break;
        }
        pos++;
      }
      else
        break;
    }
    
    fprintf(fsock, "BBS CLOSE\r\nBBS LOGOUT\r\n");
    fclose(fsock);
  }
}

int
Mime()
{
  XO *xo;
  char currsmaildir[30];

  if(server_login())
    return 0;

  del_num = is_qp = is_base64 = 0; /* �w�]�w�R���峹�Ƭ� 0, �L�ѽX�覡 */

  sprintf(currsmaildir, "mail86/%s.DIR", cuser.userid);   /* DIRECTORY/MBOX */

  utmp_mode(M_MIME); /* ���m���A���] */
  xz[XZ_OTHER - XO_ZONE].xo = xo = xo_new(currsmaildir); /* �]�� .DIR ���| */
  xz[XZ_OTHER - XO_ZONE].cb = mime_cb;
  xover(XZ_OTHER);

  server_logout(xo);
    
  free(xo);
  return 0;
}

#endif