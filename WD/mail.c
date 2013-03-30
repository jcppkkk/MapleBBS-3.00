/*-------------------------------------------------------*/
/* mail.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : local/internet mail routines                 */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#define MAXPATHLEN	1024
extern int cmpfmode();

char currmaildir[32];
/* static */ char msg_cc[] = "[32m[�s�զW��][0m\n";
static char listfile[] = "list.0";
static int mailkeep=0, mailsum=0;
static int mailsumlimit=0,mailmaxkeep=0;

int
setforward() /* Ptt , modify by wildcat*/
{
  char buf[80],ip[50]="",yn[4];
// wildcat : bbs �Ҧ� domain name list
  char myhost[6][50] = {"wd.twbbs.org","wdbbs.net","wdbbs.org","wd.twbbs.org.tw","bbs.wdbbs.net","bbs.wdbbs.org"}; 
  FILE *fp;
  int i=0,allow=1;
   
  sethomepath(buf, cuser.userid);
  strcat(buf,"/.forward");
  if(fp = fopen(buf,"r"))
  {
    fscanf(fp,"%s",ip);
    fclose(fp);
  }
  getdata(b_lines-1,0,"�п�J�H�c�۰���H��email�a�}:",
    ip,41, DOECHO,ip);
// �P�_�O�_��H���ۤv,�άO�H��P�@�x����
  do
  {
    str_lower(myhost[i],myhost[i]);
    str_lower(ip,ip);
    if(strstr(ip,myhost[i])) allow=0;
  }while(!strlen(myhost[i++])); 
// ������ not_addr �ӧP�_�O���O email �Y�i
  if(allow && !not_addr(ip))
  {
    getdata(b_lines,0,"�T�w�}�Ҧ۰���H�\\��?(Y/n)",yn,3,LCECHO,0);
    if(yn[0] != 'n' &&  (fp=fopen(buf,"w")))
    {
      fprintf(fp,"%s",ip);
      fclose(fp);
      pressanykey("�]�w����!");
      refresh();
      return 0;
    }
  }
  unlink(buf);
  pressanykey("�����۰���H!");
  refresh();
  return 0;
}

#ifdef INTERNET_PRIVATE_EMAIL
int
m_internet()
{
  char receiver[60];

  getdata(20, 0, "���H�H�G", receiver, 60, DOECHO,0);
  if (strchr(receiver, '@') && !not_addr(receiver) &&
    getdata(21, 0, "�D  �D�G", save_title, TTLEN, DOECHO,0))
  {
    do_send(receiver, save_title);
  }
  else
  {
    move(22, 0);
    pressanykey("���H�H�ΥD�D�����T, �Э��s������O");
  }
  return 0;
}

void
mail_forward(fhdr, direct, mode)
  fileheader *fhdr;
  char *direct;
  int mode;
{
  char buf[STRLEN];
  char *p;

  strncpy(buf, direct, sizeof(buf));
  if (p = strrchr(buf, '/'))
    *p = '\0';
  switch (doforward(buf, fhdr, mode))
  {
  case 0:
    outz(msg_fwd_ok);
    break;
  case -1:
    outz(msg_fwd_err1);
    break;
  case -2:
    outz(msg_fwd_err2);
  }
}
#endif

int
chkmailbox(void)
{
  if (!HAVE_PERM(PERM_SYSOP) && !HAVE_PERM(PERM_MAILLIMIT))
  {
    if (HAS_PERM(PERM_BM))
       mailsumlimit = 300;
    else if (HAS_PERM(PERM_LOGINOK))
       mailsumlimit = 150;
    else
       mailsumlimit = 100;
    mailsumlimit += cuser.exmailbox;
    mailmaxkeep = MAXKEEPMAIL + cuser.exmailbox;
    sethomedir(currmaildir, cuser.userid);
    if ((mailkeep = rec_num(currmaildir, sizeof(fileheader))) > mailmaxkeep)
    {
      move(b_lines, 0);
      clrtoeol();
      bell();
      prints("�z�O�s�H��ƥ� %d �W�X�W�� %d, �о�z", mailkeep, mailmaxkeep);
      bell();
      refresh();
      igetch();
      return mailkeep;
    }
    if ((mailsum = get_sum_records(currmaildir, sizeof(fileheader))) >
                mailsumlimit)
    {
      move(b_lines, 0);
      clrtoeol();
      bell();
      prints("�z�O�s�H��e�q %d(k)�W�X�W�� %d(k), �о�z", mailsum, mailsumlimit);
      bell();
      refresh();
      igetch();
      return mailkeep;
    }
  }
  return 0;
}


static void
do_hold_mail(fpath, receiver, holder)
  char *fpath;
  char *receiver;
  char *holder;
{
  char buf[80], title[128];

  fileheader mymail;

  sethomepath(buf, holder);
  stampfile(buf, &mymail);

  mymail.savemode = 'H';        /* hold-mail flag */
  mymail.filemode = FILE_READ;
  strcpy(mymail.owner, "[��.��.��]");
  if (receiver)
  {
    sprintf(title, "(%s) %s", receiver, save_title);
    strncpy(mymail.title, title, TTLEN);
  }
  else
    strcpy(mymail.title, save_title);

  sethomedir(title, holder);
  if (rec_add(title, &mymail, sizeof(mymail)) != -1)
  {
    unlink(buf);
    f_cp(fpath, buf, O_TRUNC);
  }
}


void
hold_mail(fpath, receiver)
  char *fpath;
  char *receiver;
{
  char buf[4];

  getdata(b_lines - 1, 0, "�w���Q�H�X�A�O�_�ۦs���Z(Y/N)�H[N] ", buf, 4, LCECHO,0);

  if (buf[0] == 'y')
    do_hold_mail(fpath, receiver, cuser.userid);
/*
  if (is_watched(cuser.userid))
    do_hold_mail(fpath, receiver, "SYSOP");
*/
}


int
do_send(userid, title)
  char *userid, *title;
{
  fileheader mhdr;
  char fpath[STRLEN];
  char receiver[IDLEN+1];
  char genbuf[200];

#ifdef INTERNET_PRIVATE_EMAIL
  int internet_mail;

  if (strchr(userid, '@'))
  {
    internet_mail = 1;
  }
  else
  {
    internet_mail = 0;
#endif

    if (!getuser(userid))
      return -1;
    if (!(xuser.userlevel & PERM_READMAIL))
      return -3;

    if (!title)
      getdata(2, 0, "�D�D�G", save_title, TTLEN, DOECHO,0);
    curredit |= EDIT_MAIL;
    curredit &= ~EDIT_ITEM;
#ifdef INTERNET_PRIVATE_EMAIL
  }
#endif

  setutmpmode(SMAIL);

  fpath[0] = '\0';

#ifdef INTERNET_PRIVATE_EMAIL
  if (internet_mail)
  {
    int res, ch;

    if (vedit(fpath, NA) == -1)
    {
      unlink(fpath);
      clear();
      return -2;
    }
    clear();
    prints("�H��Y�N�H�� %s\n���D���G%s\n�T�w�n�H�X��? (Y/N) [Y]",
      userid, title);
    ch = igetch();
    switch (ch)
    {
    case 'N':
    case 'n':
      outs("N\n�H��w����");
      res = -2;
      break;

    default:
      outs("Y\n�еy��, �H��ǻ���...\n");
      res = bbs_sendmail(fpath, title, userid, NULL);
      hold_mail(fpath, userid);
    }
    unlink(fpath);
    return res;
  }
  else
  {
#endif
   strcpy(receiver, userid);
    if (vedit(fpath, YEA) == -1)
    {
      unlink(fpath);
      clear();
      return -2;
    }
    clear();
   strcpy(userid, receiver);
    sethomepath(genbuf, userid);
    stampfile(genbuf, &mhdr);
    f_mv(fpath, genbuf);
    strcpy(mhdr.owner, cuser.userid);
    strncpy(mhdr.title, save_title, TTLEN);
    mhdr.savemode = '\0';
    sethomedir(fpath, userid);
    if (rec_add(fpath, &mhdr, sizeof(mhdr)) == -1)
      return -1;

    hold_mail(genbuf, userid);
    return 0;

#ifdef INTERNET_PRIVATE_EMAIL
  }
#endif
}


void
my_send(uident)
  char *uident;
{
  switch (do_send(uident, NULL))
  {
  case -1:
    outs(err_uid);
    break;
  case -2:
    outs(msg_cancel);
    break;
  case -3:
    prints("�ϥΪ� [%s] �L�k���H", uident);
    break;
  }
  pressanykey(NULL);
}


int
m_send()
{
  char uident[40];

  stand_title("���a�Ӫ�����");
  usercomplete(msg_uid, uident);
  if (uident[0])
    my_send(uident);
  return 0;
}


/* ------------------------------------------------------------ */
/* �s�ձH�H�B�^�H : multi_send, multi_reply                      */
/* ------------------------------------------------------------ */

extern struct word *toplev;

static void
multi_list(reciper)
  int *reciper;
{
  char uid[16];
  char genbuf[200];

  while (1)
  {
    stand_title("�s�ձH�H�W��");
    ShowNameList(3, 0, msg_cc);
    sprintf(genbuf, 
"(I)�ޤJ�n�� (O)�ޤJ�W�u�q�� (N)�ޤJ�s�峹�q�� (0-9)�ޤJ��L�S�O�W��\n"
"(A)�W�[     (D)�R��         (M)�T�{�H�H�W��  %s(Q)���� �H[M]",
    HAS_PERM(PERM_SYSOP) ? " (B)�Ҧ��O�D " : "");
    getdata(1, 0, genbuf, genbuf, 4, LCECHO,0);
    switch (genbuf[0])
    {
    case 'a':
      while (1)
      {
        move(2, 0);
        usercomplete("�п�J�n�W�[���N��(�u�� ENTER �����s�W): ", uid);
        if (uid[0] == '\0')
          break;

        move(3, 0);

        if (!searchuser(uid))
          outs(err_uid);
        else if (!InNameList(uid))
        {
          AddNameList(uid);
          (*reciper)++;
        }
        ShowNameList(3, 0, msg_cc);
      }
      break;

    case 'd':
      while (*reciper)
      {
        move(1, 0);
        namecomplete("�п�J�n�R�����N��(�u�� ENTER �����R��): ", uid);
        if (uid[0] == '\0')
          break;
        if (RemoveNameList(uid))
        {
          (*reciper)--;
        }
        ShowNameList(3, 0, msg_cc);
      }
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      listfile[5] = genbuf[0];
      genbuf[0] = '1';

    case 'i':
      setuserfile(genbuf, genbuf[0] == '1' ? listfile : fn_overrides);
      ToggleNameList(reciper, genbuf, msg_cc);
      break;

    case 'o':
      setuserfile(genbuf, "alohaed");
      ToggleNameList(reciper, genbuf, msg_cc);
      break;

    case 'n':
      setuserfile(genbuf, "postlist");
      ToggleNameList(reciper, genbuf, msg_cc);
      break;

    case 'm':
      *reciper = 0;
      return;
    break;

    case 'b':
      if (HAS_PERM(PERM_SYSOP)) {
         make_bmlist();
         *reciper = CountNameList();
         return;
      }
      break;

    default:
      return;
    }
  }
}



/* static */
/* shakalaca.000117: marked for list.c */
int
multi_send(title, inmail)
  char *title;
  int inmail;
{
  FILE *fp;
  struct word *p;
  fileheader mymail;
  char fpath[TTLEN], *ptr;
  int reciper, listing;
  char genbuf[256];

  if (inmail)
  {
    CreateNameList();
    listing = reciper = 0;

    if (*quote_file)
    {
      AddNameList(quote_user);
      reciper = 1;
      fp = fopen(quote_file, "r");
      while (fgets(genbuf, 256, fp))
      {
        if (strncmp(genbuf, "�� ", 3))
        {
          if (listing)
            break;
        }  
        else
        {
          if (listing)
          {
            strtok(ptr = genbuf + 3, " \n\r");
            do
            {
              if (searchuser(ptr) && !InNameList(ptr) && strcmp(cuser.userid, ptr))
              {
                AddNameList(ptr);
                reciper++;
              }
            } while (ptr = (char *) strtok(NULL, " \n\r"));
          }
          else if (!strncmp(genbuf + 3, "[�q�i]", 6))
            listing = 1;
        }
      }
      ShowNameList(3, 0, msg_cc);
    }  

    multi_list(&reciper);
  }
  else
  {
    reciper = CountNameList();
  }
  
  move(1, 0);
  clrtobot();

  if (reciper)
  {
    setutmpmode(SMAIL);

    if (title)
    {
      do_reply_title(2, title);
    }
    else
    { 
      getdata(2, 0, "�D�D�G", fpath, 64, DOECHO,0);
      sprintf(save_title, "[�q�i] %s", fpath);
    } 

    setuserfile(fpath, fn_notes);

    if (fp = fopen(fpath, "w"))
    {
      fprintf(fp, "�� [�q�i] �@ %d �H����", reciper);
      listing = 80;

      for (p = toplev; p; p = p->next)
      {
        reciper = strlen(p->word) + 1;
        if (listing + reciper > 75)
        {
          listing = reciper;
          fprintf(fp, "\n��");
        }
        else
          listing += reciper;

        fprintf(fp, " %s", p->word);
      }
      memset(genbuf, '-', 75);
      genbuf[75] = '\0';
      fprintf(fp, "\n%s\n\n", genbuf);
      fclose(fp);
    }

    curredit |= EDIT_LIST;

    if (vedit(fpath, YEA) == -1)
    {
      unlink(fpath);
      curredit = 0;
      pressanykey(msg_cancel);
      return;
    }

    stand_title("�H�H��...");
    refresh();

    listing = 80;

    for (p = toplev; p; p = p->next)
    {
      reciper = strlen(p->word) + 1;
      if (listing + reciper > 75)
      {
        listing = reciper;
        outc('\n');
      }
      else
      {
        listing += reciper;
        outc(' ');
      }
      outs(p->word);
      if (searchuser(p->word) && strcmp(STR_GUEST, p->word) )
        sethomepath(genbuf, p->word);
      else
        continue;
      stampfile(genbuf, &mymail);
      unlink(genbuf);
      f_cp(fpath, genbuf, O_TRUNC);

      strcpy(mymail.owner, cuser.userid);
      strcpy(mymail.title, save_title);
      mymail.savemode = 'M';    /* multi-send flag */
      sethomedir(genbuf, p->word);
      if (rec_add(genbuf, &mymail, sizeof(mymail)) == -1)
        outs(err_uid);
    }
    hold_mail(fpath, NULL);
    unlink(fpath);
    curredit = 0;
  }
  else
  {
    outs(msg_cancel);
  }
  pressanykey(NULL);
}


static int
multi_reply(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  if (fhdr->savemode != 'M')
    return mail_reply(ent, fhdr, direct);

  stand_title("�s�զ^�H");
  strcpy(quote_user, fhdr->owner);
  setuserfile(quote_file, fhdr->filename);
  multi_send(fhdr->title, 1);
  return 0;
}


int
mail_list()
{
  stand_title("�s�է@�~");
  multi_send(NULL, 1);
  return 0;
}

extern int
bad_user_id(char userid[]);

int
mail_all()
{
   FILE *fp;
   fileheader mymail;
   char fpath[TTLEN];
   char genbuf[200];
   extern struct UCACHE *uidshm;
   int i, unum;
   char* userid;

   stand_title("���Ҧ��ϥΪ̪��t�γq�i");
   setutmpmode(SMAIL);
/*
   if(answer("�O�_�n�����H (y/N)") == 'y')
   {
     while(money <= 0)
     {
       getdata(2, 0, "�n���h�֪���?",fpath, 4, DOECHO, 0);
       money = atoi(fpath);
       if(money <=0) return RC_FULL;
       mode = 1;
     }
   }
*/
   getdata(2, 0, "�D�D�G", fpath, 64, DOECHO,0);
   sprintf(save_title, "[�t�γq�i][1;32m %s[m", fpath);

   setuserfile(fpath, fn_notes);

   if (fp = fopen(fpath, "w")) {
      fprintf(fp, "�� [[1m�t�γq�i[m] �o�O�ʵ��Ҧ��ϥΪ̪��H\n");
      fprintf(fp, "---------------------------------------------------------------------------\n");
      fclose(fp);
    }

   *quote_file = 0;

   curredit |= EDIT_MAIL;
   curredit &= ~EDIT_ITEM;
   if (vedit(fpath, YEA) == -1) {
      curredit = 0;
      unlink(fpath);
      pressanykey(msg_cancel);
      return;
   }
   curredit = 0;

   setutmpmode(MAILALL);
   stand_title("�H�H��...");

   sethomepath(genbuf, cuser.userid);
   stampfile(genbuf, &mymail);
   unlink(genbuf);
   f_cp(fpath, genbuf, O_TRUNC);
   unlink(fpath);
   strcpy(fpath, genbuf);

   strcpy(mymail.owner, cuser.userid);  /*���� ID*/
   strcpy(mymail.title, save_title);
   mymail.savemode = 0;

   sethomedir(genbuf, cuser.userid);
   if (rec_add(genbuf, &mymail, sizeof(mymail)) == -1)
      outs(err_uid);

   for (unum = uidshm->number, i = 0; i < unum; i++) {

      if(bad_user_id(uidshm->userid[i])) continue; /* Ptt */

      userid = uidshm->userid[i];
      if (strcmp(userid, "guest") && strcmp(userid, "new") && strcmp(userid, cuser.userid)) {
         sethomepath(genbuf, userid);
         stampfile(genbuf, &mymail);
         unlink(genbuf);
         f_cp(fpath, genbuf, O_TRUNC);

         strcpy(mymail.owner, cuser.userid);
         strcpy(mymail.title, save_title);
         mymail.savemode = 0;
         sethomedir(genbuf, userid);
         if (rec_add(genbuf, &mymail, sizeof(mymail)) == -1)
            outs(err_uid);
         sprintf(genbuf, "%*s %5d / %5d", IDLEN + 1, userid, i + 1, unum);
/*
         if(mode)
           inugold(userid,money);
*/
         outmsg(genbuf);
         refresh();
      }
   }
   return;
}


mail_mbox()
{
   char cmd[100];
   fileheader fhdr;

   sprintf(cmd, "/tmp/%s.tgz", cuser.userid);
   sprintf(fhdr.title, "%s �p�H���", cuser.userid);
   doforward(cmd, &fhdr, 'Z');
}

static int
m_forward(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char uid[STRLEN];

  stand_title("��F�H��");
  usercomplete(msg_uid, uid);
  if (uid[0] == '\0')
  {
    return RC_FULL;
  }

  strcpy(quote_user, fhdr->owner);
  setuserfile(quote_file, fhdr->filename);
  sprintf(save_title, "%.64s (fwd)", fhdr->title);
  move(1, 0);
  clrtobot();
  prints("��H��: %s\n��  �D: %s\n", uid, save_title);

  switch (do_send(uid, save_title))
  {
  case -1:
    outs(err_uid);
    break;
  case -2:
    outs(msg_cancel);
    break;
  case -3:
    prints("�ϥΪ� [%s] �L�k���H", uid);
    break;
  }
  pressanykey(NULL);
  return RC_FULL;
}


/* JhLin: At most 128 mail */

int delmsgs[128];
int delcnt;
int mrd;


static int
read_new_mail(fptr)
  fileheader *fptr;
{
  static int idc;
  char done = NA, delete_it;
  char fname[256];
  char genbuf[4];

  if (fptr == NULL)
  {
    delcnt = 0;
    idc = 0;
    return 0;
  }
  idc++;
  if (fptr->filemode)
    return 0;
  clear();
  move(10, 0);
  prints("�z�nŪ�Ӧ�[%s]���T��(%s)�ܡH", fptr->owner, fptr->title);
  getdata(11, 0, "�бz�T�w(Y/N/Q)?[Y] ", genbuf, 3, DOECHO,0);
  if (genbuf[0] == 'q')
    return QUIT;
  if (genbuf[0] == 'n')
    return 0;

  setuserfile(fname, fptr->filename);
  fptr->filemode |= FILE_READ;
  if (substitute_record(currmaildir, fptr, sizeof(*fptr), idc))
    return -1;

  mrd = 1;
  delete_it = NA;
  while (!done)
  {
    int more_result = more(fname, YEA);
    switch (more_result) {
    case 1:
       return RS_PREV;
    case 2:
       return RELATE_PREV;
    case 3:
       return RS_NEXT;
    case 4:
       return RELATE_NEXT;
    case 5:
       return RELATE_FIRST;
    case 6:
       return 0;
    case 7:
      mail_reply(idc, fptr, currmaildir);
      return RC_FULL;
    case 8:
      multi_reply(idc, fptr, currmaildir);
      return RC_FULL;
    }
    move(b_lines, 0);
    clrtoeol();
    outs(msg_mailer);
    refresh();

    switch (igetkey())
    {
    case 'r':
    case 'R':
      mail_reply(idc, fptr, currmaildir);
      break;
    case 'x':
      m_forward(idc, fptr, currmaildir);
      break;
    case 'y':
      multi_reply(idc, fptr, currmaildir);
      break;
    case 'd':
    case 'D':
      delete_it = YEA;
    default:
      done = YEA;
    }
  }
  if (delete_it)
  {
    clear();
    prints("�R���H��m%s�n", fptr->title);
    getdata(1, 0, msg_sure_ny, genbuf, 2, LCECHO,0);
    if (genbuf[0] == 'y')
    {
      unlink(fname);
      delmsgs[delcnt++] = idc;
    }
  }
  clear();
  return 0;
}


int
m_new()
{
  clear();
  mrd = 0;
  setutmpmode(RMAIL);
  read_new_mail(NULL);
  clear();
  curredit |= EDIT_MAIL;
  curredit &= ~EDIT_ITEM;
  if (rec_apply(currmaildir, read_new_mail, sizeof(fileheader)) == -1)
  {
    pressanykey("�S���s�H��F");
    return -1;
  }
  curredit = 0;
  if (delcnt)
  {
    while (delcnt--)
      rec_del(currmaildir, sizeof(fileheader), delmsgs[delcnt]);
  }
  outs(mrd ? "�H�w�\\��" : "�S���s�H��F");
  pressanykey(NULL);
  return -1;
}


static void
mailtitle()
{
  char buf[100]="";

  sprintf(tmpbuf,"%s [�u�W %d �H]",BOARDNAME,count_ulist());
  showtitle("\0�l����", tmpbuf);
#ifdef HYPER_BBS
  outs(HB_BACK"\
[200;442;504;612m[��[200;442;505;612m��][201m��� [200;442;507m[��,r]�\\Ū�H�� [200;442;682m[R]�^�H [200;442;721m[y]�s�զ^�H [200;442;626m[^Z]�D�U [200;442;500;612m[PgUp[201m/[200;442;501;612mPgDn][201m\n[1m\
"COLOR1" �s��   �� ��  �@ ��          �H  ��  ��  �D ");
#else
  outs("\
[��]���} [��,��]��� [��,r]�\\Ū�H�� [R]�^�H [x]��F [y]�s�զ^�H [^Z]�D�U\n[1m\
"COLOR1" �s��   �� ��  �@ ��          �H  ��  ��  �D ");
#endif
  if(mailsumlimit)
  {
    sprintf(buf,"[32m(�e�q:%d/%dk %d/%d�g)",mailsum, mailsumlimit
                    ,mailkeep,mailmaxkeep);
  }
  sprintf(buf,"%s%*s[m",buf,34-strlen(buf),"");
  outs(buf);
}

static int
mail_del(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char genbuf[200];

  if (fhdr->filemode & FILE_MARKED)
    return RC_NONE;

  getdata(1, 0, msg_del_ny, genbuf, 3, LCECHO,0);
  if (genbuf[0] == 'y')
  {
    extern int cmpfilename();

    strcpy(currfile, fhdr->filename);
    if (!delete_file(direct, sizeof(*fhdr), ent, cmpfilename))
    {
      setdirpath(genbuf, direct, fhdr->filename);
      unlink(genbuf);
      if( currmode & MODE_SELECT ){
         int now;
         sethomedir(genbuf,cuser.userid);
         now=getindex(genbuf,fhdr->filename,sizeof(fileheader));
         delete_file (genbuf, sizeof(fileheader),now,cmpfilename);
      }
      return RC_CHDIR;
    }
  }
  return RC_FULL;
}


/*
woju
static int
*/
mail_read(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char buf[64];
  char done, delete_it, replied;

  clear();
  setdirpath(buf, direct, fhdr->filename);
  strncpy(currtitle, str_ttl(fhdr->title), 40);
  done = delete_it = replied = NA;
  while (!done)
  {
/*
woju
*/
    int more_result =  more(buf, YEA);
    if (more_result != -1) {
       fhdr->filemode |= FILE_READ;
       if (!strncmp("[�s]", fhdr->title, 4) && !(fhdr->filemode & FILE_MARKED))
          fhdr->filemode |= FILE_TAGED;
     if ( currmode & MODE_SELECT )
     {
        int now;
        now = getindex(currmaildir, fhdr->filename, sizeof(*fhdr));
        substitute_record(currmaildir, fhdr, sizeof(*fhdr), now);
     }
     else
       substitute_record(currmaildir, fhdr, sizeof(*fhdr), ent);
    }
    switch (more_result) {
    case 1:
       return RS_PREV;
    case 2:
       return RELATE_PREV;
    case 3:
       return RS_NEXT;
    case 4:
       return RELATE_NEXT;
    case 5:
       return RELATE_FIRST;
    case 6:
       return RC_FULL;
    case 7:
      mail_reply(ent, fhdr, direct);
      return RC_FULL;
    case 8:
      multi_reply(ent, fhdr, direct);
      return RC_FULL;
    }
    move(b_lines, 0);
    clrtoeol();
    refresh();
    outs(msg_mailer);
  }
}

/* ---------------------------------------------- */
/* in boards/mail �^�H����@�̡A��H����i        */
/* ---------------------------------------------- */

int
mail_reply(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char uid[STRLEN];
  char *t;
  FILE *fp;
  char genbuf[512];
  int enttmp=ent;

  stand_title("�^  �H");

  /* �P�_�O boards �� mail */

  if (curredit & EDIT_MAIL)
    setuserfile(quote_file, fhdr->filename);
  else
    setbfile(quote_file, currboard, fhdr->filename);

  /* find the author */

  strcpy(quote_user, fhdr->owner);

  if (strchr(quote_user, '.'))
  {
    genbuf[0] = '\0';
    if (fp = fopen(quote_file, "r"))
    {
      fgets(genbuf, 512, fp);
      fclose(fp);
    }

    t = strtok(genbuf, str_space);
    if (!strcmp(t, str_author1) || !strcmp(t, str_author2))
    {
      strcpy(uid, strtok(NULL, str_space));
    }
    else
    {
      pressanykey("���~: �䤣��@�̡C");
      return RC_FULL;
    }
  }
  else
    strcpy(uid, quote_user);

  /* make the title */

  do_reply_title(3, fhdr->title);
  prints("\n���H�H: %s\n��  �D: %s\n", uid, save_title);

  /* edit, then send the mail */

  ent = curredit;
  switch (do_send(uid, save_title))
  {
  case -1:
    outs(err_uid);
    break;
  case -2:
    outs(msg_cancel);
    break;
  case -3:
    prints("�ϥΪ� [%s] �L�k���H", uid);
    break;
  default:
    fhdr->filemode |= FILE_REPLYOK;
    substitute_record(currmaildir, fhdr, sizeof(*fhdr), enttmp);
    break;
  }
  curredit = ent;
  pressanykey(NULL);
  return RC_FULL;
}

static int
mail_cross_post(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  char xboard[20], fname[80], xfpath[80], xtitle[80], inputbuf[10];
  fileheader xfile;
  FILE *xptr;
  int author = 0;
  char genbuf[200];
  char genbuf2[4];

  make_blist();
  move(2, 0);
  clrtoeol();
  move(3, 0);
  clrtoeol();
  move(1, 0);
  namecomplete("������峹��ݪO�G", xboard);
  if (*xboard == '\0' || !haspostperm(xboard))
    return RC_FULL;

  ent = 1;
  if (HAS_PERM(PERM_SYSOP) || !strcmp(fhdr->owner, cuser.userid))
  {
    getdata(2, 0, "(1)������ (2)������榡�H[1] ",
      genbuf, 3, DOECHO,"1");
    if (genbuf[0] != '2')
    {
      ent = 0;
      getdata(2, 0, "�O�d��@�̦W�ٶ�?[Y] ", inputbuf, 3, DOECHO,"Y");
      if (inputbuf[0] != 'n' && inputbuf[0] != 'N') author = 1;
    }
  }

  if (ent)
    sprintf(xtitle, "[���]%.66s", fhdr->title);
  else
    strcpy(xtitle, fhdr->title);

  sprintf(genbuf, "�ĥέ���D�m%.60s�n��?[Y] ", xtitle);
  getdata(2, 0, genbuf, genbuf2, 4, LCECHO,"Y");
  if (*genbuf2 == 'n')
  {
    if (getdata(2, 0, "���D�G", genbuf, TTLEN, DOECHO,0))
      strcpy(xtitle, genbuf);
  }

  getdata(2, 0, "(S)�s�� (L)���� (Q)�����H[S] ", genbuf, 3, LCECHO,"S");
  if (genbuf[0] == 'l' || genbuf[0] == 's')
  {
    int currmode0 = currmode;

    currmode = 0;
    setbpath(xfpath, xboard);
    stampfile(xfpath, &xfile);
    if (author)
      strcpy(xfile.owner, fhdr->owner);
    else
      strcpy(xfile.owner, cuser.userid);
    strcpy(xfile.title, xtitle);
    if (genbuf[0] == 'l')
    {
      xfile.savemode = 'L';
      xfile.filemode = FILE_LOCAL;
    }
    else
      xfile.savemode = 'S';

    setuserfile(fname, fhdr->filename);
    if (ent)
    {
      xptr = fopen(xfpath, "w");

      strcpy(save_title, xfile.title);
      strcpy(xfpath, currboard);
      strcpy(currboard, xboard);
      write_header(xptr);
      strcpy(currboard, xfpath);

      fprintf(xptr, "�� [��������� %s �H�c]\n\n", cuser.userid);

      b_suckinfile(xptr, fname);
      addsignature(xptr);
      fclose(xptr);
    }
    else
    {
      unlink(xfpath);
      f_cp(fname, xfpath, O_TRUNC);
    }

    setbdir(fname, xboard);
    rec_add(fname, (char *) &xfile, sizeof(xfile));
    if (!xfile.filemode)
      outgo_post(&xfile, xboard);
    update_data();
    cuser.numposts++;
    substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
    pressanykey("�峹�������");
    currmode = currmode0;
  }
  return RC_FULL;
}



mail_save(int ent, fileheader* fhdr, char* direct)
{
   char fpath[256];
   char title[TTLEN+1];

   if (HAS_PERM(PERM_MAILLIMIT)) {
      setuserfile(fpath, fhdr->filename);
      strcpy(title, "�� ");
      strncpy(title+3, fhdr->title, TTLEN-3);
      title[TTLEN] = '\0';
      a_copyitem(fpath, title, fhdr->owner);
      sethomeman(fpath, cuser.userid);
      a_menu(cuser.userid, fpath, belong("etc/sysop", cuser.userid) ? 2 : 1);
      return RC_FULL;
   }
   return RC_NONE;
}

extern int cite();
extern int man();
extern int add_tag();
extern int del_tag();
extern int gem_tag();
extern int edit_post();
extern int mark();
extern int del_range();
extern int edit_title();

static struct one_key mail_comms[] = {
  'z', man,
  'c', cite,
  'C', gem_tag,
  's', mail_save,
  'd', mail_del,
  'D', del_range,
  'r', mail_read,
  'R', mail_reply,
  'E', edit_post,
  'm', mark,
  't', add_tag,
  'T', edit_title,
  'x', m_forward,
  'X', mail_cross_post,
  Ctrl('D'), del_tag,
  'y', multi_reply,
  '\0', NULL
};


int
m_read()
{
  if (rec_num(currmaildir, sizeof(fileheader)))
  {
    curredit = EDIT_MAIL;
    curredit &= ~EDIT_ITEM;
    i_read(RMAIL, currmaildir, mailtitle, doent, mail_comms,NULL);
    currfmode = FILE_TAGED;
    if (search_rec(currmaildir, cmpfmode))
      del_tag(0, 0, currmaildir);
    curredit = 0;
    return 0;
  }
  else
  {
    pressanykey("�z�S���ӫH");
    return 0;
  }
}


#ifdef INTERNET_EMAIL
#include <pwd.h>

int
bbs_sendmail(fpath, title, receiver, key)
  char *fpath, *title, *receiver, *key;
{
  static int configured = 0;
  static char myhostname[STRLEN];
  static char myusername[20];
  struct hostent *hbuf;
  struct passwd *pbuf;
  char *ptr, genbuf[200];

  FILE *fin, *fout;

  if (ptr = strchr(receiver, ';'))
    *ptr = '\0';

  if ((ptr = strstr(receiver, str_mail_address)) || !strchr(receiver, '@'))
  {
    fileheader mymail;
    char hacker[20];
    int len;

    if (strchr(receiver, '@'))
    {
      len = ptr - receiver;
      memcpy(hacker, receiver, len);
      hacker[len] = '\0';
    } 
    else
      strcpy(hacker, receiver);

    if (!searchuser(hacker))
      return -2;

    sethomepath(genbuf, hacker);
    stampfile(genbuf, &mymail);
    if (!strcmp(hacker, cuser.userid))
    {
      strcpy(mymail.owner, BOARDNAME);
      mymail.filemode = FILE_READ;
    } else
      strcpy(mymail.owner, cuser.userid);
    strncpy(mymail.title, title, TTLEN);
    f_rm(genbuf);
    f_cp(fpath, genbuf, O_TRUNC);
    sethomefile(genbuf, hacker, ".DIR");
    return rec_add(genbuf, &mymail, sizeof(mymail));
  }
  /* setup the hostname and username */
  if (!configured)
  {
    /* get host name */
    gethostname(myhostname, STRLEN);
    hbuf = gethostbyname(myhostname);
    if (hbuf)
      strncpy(myhostname, hbuf->h_name, STRLEN);

    /* get bbs uident */
    pbuf = getpwuid(getuid());
    if (pbuf)
      strncpy(myusername, pbuf->pw_name, 20);
    if (hbuf && pbuf)
      configured = 1;
    else
      return -1;
  }
  /* Running the sendmail */

#ifdef  INTERNET_PRIVATE_EMAIL
  if (fpath == NULL)
  {
    sprintf(genbuf, "/usr/sbin/sendmail %s > /dev/null", receiver);
    fin = fopen("etc/confirm", "r");
  } else
  {
    sprintf(genbuf, "/usr/sbin/sendmail -f %s%s %s > /dev/null"
	    ,cuser.userid, str_mail_address, receiver);
    fin = fopen(fpath, "r");
  }
  fout = popen(genbuf, "w");
  if (fin == NULL || fout == NULL)
    return -1;

  if (fpath)
    fprintf(fout, "Reply-To: %s%s\nFrom: %s%s\n",
	    cuser.userid, str_mail_address, cuser.userid, str_mail_address);
#else
  sprintf(genbuf, "/usr/sbin/sendmail %s > /dev/null", receiver);
  fout = popen(genbuf, "w");
  fin = fopen(fpath ? fpath : "etc/confirm", "r");
  if (fin == NULL || fout == NULL)
    return -1;

  if (fpath)
    fprintf(fout, "From: %s@%s (%s)\n", myusername, myhostname, BBSNAME);
#endif

  fprintf(fout, "To: %s\nSubject: %s\n", receiver, title);
  fprintf(fout, "X-Disclaimer: " BOARDNAME "�糧�H���e�����t�d�C\n\n");

  while (fgets(genbuf, sizeof(genbuf), fin))
  {
    if (genbuf[0] == '.' && genbuf[1] == '\n')
      fputs(". \n", fout);
    else
    {
#ifdef REG_MAGICKEY
      char *po;
      while (po = strstr(genbuf, "<Magic>"))
      {
	char buf[128];

	po[0] = 0;
	sprintf(buf, "%s%s%s", genbuf, key, po + 7);
	strcpy(genbuf, buf);
      }
#endif
      fputs(genbuf, fout);
    }
  }
  fclose(fin);
  fprintf(fout, ".\n");
  pclose(fout);
  return 0;
}


int
doforward(direct, fh, mode)
  char *direct;
  fileheader *fh;
  int mode;                     /* �O�_ uuencode */
{
  static char address[60];
  char fname[MAXPATHLEN];
  int return_no;
  char genbuf[200];

#if 1   /* itoc.001203: ......... */
  if ((currstat == READING) && (fh->filemode & FILE_REFUSE) && !(currmode & MODE_BOARD))
    return RC_NONE;
#endif
           


  if (!address[0])
    strcpy(address, cuser.email);

  if (address[0])
  {
    sprintf(genbuf, "�T�w��H�� [%s] ��(Y/N/Q)�H[Y] ", address);
    getdata(b_lines - 1, 0, genbuf, fname, 3, LCECHO,0);

    if (fname[0] == 'q')
    {
      outz("������H");
      return 1;
    }
    if (fname[0] == 'n')
      address[0] = '\0';
  }

  if (!address[0])
  {
    getdata(b_lines - 1, 0, "�п�J��H�a�}�G", fname, 60, DOECHO,0);
    if (fname[0])
    {
      if (strchr(fname, '.'))
        strcpy(address, fname);
      else
        sprintf(address, "%s.bbs@%s", fname, MYHOSTNAME);
    }
    else
    {
      outz("������H");
      return 1;
    }
  }

  if (not_addr(address))
    return -2;

  sprintf(fname, "����H�� %s, �еy��...", address);
  outz(fname);

  if (mode == 'Z') {
     FILE* fp;
     int address_ok = valid_ident(address);

     if (fp = fopen("mbox_sent", "a")) {
        time_t now = time(0);

        fprintf(fp, "%c%-12s %s => %s\n",
           address_ok ? ' ' : '-', cuser.userid, Cdatelite(&now), address);
        fclose(fp);
     }
     if (!address_ok) {
         sprintf(fname, "�L�Ī��u�@����} %s", address);
         outz(fname);
         return -2;
      }
     sprintf(fname, "cd home; tar cfz - %s | uuencode %s.tgz > /tmp/%s.tgz",
        cuser.userid, cuser.userid, cuser.userid);
     system(fname);
     strcpy(fname, direct);
  }
  else if (mode == 'U')
  {
    char tmp_buf[128];

    sprintf(fname, "/tmp/bbs.uu%05d", currpid);
    sprintf(tmp_buf, "/usr/bin/uuencode %s/%s uu.%05d > %s",
      direct, fh->filename, currpid, fname);
    system(tmp_buf);
    sleep(1);
  }
  else
    sprintf(fname, "%s/%s", direct, fh->filename);

  return_no = bbs_sendmail(fname, fh->title, address, NULL);

    if (mode != 'F')

  return (return_no);
}

#endif


int
chkmail(rechk)
  int rechk;
{
  static time_t lasttime = 0;
  static int ismail = 0;
  struct stat st;
  int fd;
  register numfiles;
  unsigned char ch;

//  if (!HAS_PERM(PERM_BASIC))
//    return 0;

  if (stat(currmaildir, &st) < 0)
    return (ismail = 0);

  if ((lasttime >= st.st_mtime) && !rechk)
    return ismail;

  lasttime = st.st_mtime;
  numfiles = st.st_size / sizeof(fileheader);
  if (numfiles <= 0)
    return (ismail = 0);

  /* ------------------------------------------------ */
  /* �ݬݦ��S���H���٨SŪ�L�H�q�ɧ��^�Y�ˬd�A�Ĳv���� */
  /* ------------------------------------------------ */

  if ((fd = open(currmaildir, O_RDONLY)) > 0)
  {
    lseek(fd, (off_t)(st.st_size - 1), SEEK_SET);
    while (numfiles--)
    {
      read(fd, &ch, 1);
      if (!(ch & FILE_READ))
      {
        close(fd);
        return (ismail = 1);
      }
      lseek(fd, -(off_t)(sizeof(fileheader) + 1), SEEK_CUR);
    }
    close(fd);
  }
  return (ismail = 0);
}

#ifdef  REG_MAGICKEY
void
mail_justify(userec muser)
{
  fileheader mhdr;
  char title[128], buf1[80];

  sethomepath(buf1, muser.userid);
  stampfile(buf1, &mhdr);
  unlink(buf1);
  strcpy(mhdr.owner, "����");
  strncpy(mhdr.title, "[EMail�{��]", TTLEN);
  mhdr.savemode = 0;
  mhdr.filemode = 0;

  if (valid_ident(muser.email) && !not_addr(muser.email))
  {
    char title[80], *ptr, ch, MagicKey[9];
    ushort checksum;            /* 16-bit is enough */
    time_t now;
                
    checksum = getuser(muser.userid);
    ptr = muser.email;
    while (ch = *ptr++)
    {
      if (ch <= ' ')
        break;
      if (ch >= 'A' && ch <= 'Z')
        ch |= 0x20;
      checksum = (checksum << 1) ^ ch;
    }
    /* shakalaca.990725: �����{�ҥΪ� MagicKey */
    sprintf(title, "%d", checksum + time(&now));
    strncpy(MagicKey, title, 8);
    MagicKey[8] = '\0';

/*
    sprintf(title, "[WD BBS]To %s(%d:%d) [User Justify]",
      muser.userid, getuser(muser.userid) + MAGIC_KEY, checksum);
*/ /* shakalaca.990718: �N�һ{�аO�W�ߥX�Ӥ�K�ק� */
/*
    sprintf(title, "%s%s(%d:%d) [User Justify]", TAG_VALID, 
      muser.userid, getuser(muser.userid) + MAGIC_KEY, checksum);
*/
    sprintf(title, "%s To %s: [���n ! �о\\Ū]", TAG_VALID, muser.userid);

    if (bbs_sendmail(NULL, title, muser.email, MagicKey) < 0)
      f_cp("etc/bademail", buf1, O_TRUNC);
    else
      f_cp("etc/justify", buf1, O_TRUNC);
  }
  else
    f_cp("etc/bademail", buf1, O_TRUNC);

  sethomedir(title, muser.userid);
  rec_add(title, &mhdr, sizeof(mhdr));
}
#endif                          /* REG_MAGICKEY */
