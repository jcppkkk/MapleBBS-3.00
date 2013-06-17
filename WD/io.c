/*-------------------------------------------------------*/
/* io.c         ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : basic console/screen/keyboard I/O routines   */
/* create : 95/02/28                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#ifdef AIX
#include <sys/select.h>
#endif

#ifdef  LINUX
#define OBUFSIZE  (2048)
#define IBUFSIZE  (128)
#else
#define OBUFSIZE  (4096)
#define IBUFSIZE  (512)
#endif

#define INPUT_ACTIVE    0
#define INPUT_IDLE      1

static char outbuf[OBUFSIZE];
static int obufsize = 0;

static char inbuf[IBUFSIZE];
static int ibufsize = 0;
static int icurrchar = 0;

static int i_mode = INPUT_ACTIVE;

extern int dumb_term;

passwd_outs(text)
  char *text;
{
  register int column = 0;
  register char ch;
  while ((ch = *text++) && (++column < 80))
  {
    outch('*');
  }
}

/* ----------------------------------------------------- */
/* �w����ܰʺA�ݪO                                      */
/* ----------------------------------------------------- */


#define STAY_TIMEOUT    (30*60)

static void
hit_alarm_clock()
{
  static int stay_time = 0;
  static int idle_time = 0;
  register user_info *uentp;
  register int mode = currutmp->mode;
  
  time_t now = time(0);
  char buf[100]="\0";

  if(currutmp->pid != currpid)
    setup_utmp(XMODE);   /* ���s�t�m shm */

  idle_time = now - currutmp->lastact;

  if(mode == MJ || mode == BIG2)
  idle_time = 0 ;
    
  if((idle_time > IDLE_TIMEOUT) && !PERM_HIDE(currutmp) )
  {
//    pressanykey("�W�L���m�ɶ��I��X�h�o�K�K");
//    abort_bbs();
   outz("[1;37;41m�W�L���m�ɶ��I��X�h�o......[m\n");
   if ((kill(uentp->pid, SIGHUP) == -1) && (errno == ESRCH))
   memset(uentp, 0, sizeof(user_info));
  }

  if (HAS_HABIT(HABIT_MOVIE) && (currstat && (currstat < CLASS || currstat == MAILALL)))
    movie(0);

  alarm(MOVIE_INT);
  stay_time += MOVIE_INT;

  if((idle_time > IDLE_TIMEOUT - 60) && !PERM_HIDE(currutmp) ) 
    sprintf(buf, "[1;5;37;41mĵ�i�G�z�w���m�L�[�A�Y�L�^���A�t�ΧY�N�����I�I[m");
  else if(stay_time > 10 * 60 && chkmail(0)) 
  {
    sprintf(buf, "\033[1;33;41m[%s] �H�c���٦��S�ݹL���H��\033[m",
      Etime(&now));
    stay_time = 0 ;
  }
  else if(stay_time > STAY_TIMEOUT && HAS_HABIT(HABIT_ALARM))
  {
    /* �b�o�̴��� user �𮧤@�U */
    char *msg[10] = {
    "�����y, �|�|��, �ܤf��....�ݤf��...�A�~��...!",
    "�@��O�ͱ� �@��O�R�� ���k���G�Ƭ����ۦۤv...",
    "�O�_���H�A�ѱz���ߪ��t�I? �j�a��talk talk�a.. ",
    "���T��,�k�T��,��l��᧾�ѧ�� �j�a�ӧ@�B�ʭ�~",
    "����..����..��������! ���w���z��...",
    "�ΡE�\\�E��E��",
    "�ѩ����F�S��....^.^",
    "���Ѧ��S���Ҹհ�...���ѭ��n��...!",
    "�ݡ�y�P�I",
    "�E�Ѧb�ߤ���۬ӡEŪ�ѥh�E"};
    int i = rand() % 10;

    sprintf(buf, "[1;33;41m[%s] %s[m", Etime(&now), msg[i]);
    stay_time = 0 ;
  }

  if(buf[0]) 
  {
    outmsg(buf);
    refresh();
    bell();
  }
}

void
init_alarm()
{
  alarm(0);
  signal(SIGALRM, hit_alarm_clock);
  alarm(MOVIE_INT);
}


/* ----------------------------------------------------- */
/* output routines                                       */
/* ----------------------------------------------------- */


void
oflush()
{
  if (obufsize)
  {
#ifdef GB_SUPPORT
    if(HAS_HABIT(HABIT_BIG5GB))
    {
      char *outmp;
      outmp = big2gb(outbuf, &obufsize, 0);
//      outmp = gb2big(outbuf, &obufsize, 0);
      write(1,outmp, obufsize);
      obufsize=0;
    }
    else
    {
#endif
      write(1, outbuf, obufsize);
      obufsize = 0;
#ifdef GB_SUPPORT
    }
#endif
  }
}


void
output(s, len)
  char *s;
{
  /* Invalid if len >= OBUFSIZE */

  if (obufsize + len > OBUFSIZE)
  {
    write(1, outbuf, obufsize);
    obufsize = 0;
  }
  memcpy(outbuf + obufsize, s, len);
  obufsize += len;
}


void
ochar(c)
{
  if (obufsize > OBUFSIZE - 1)
  {
    write(1, outbuf, obufsize);
    obufsize = 0;
  }
  outbuf[obufsize++] = c;
}


/* ----------------------------------------------------- */
/* input routines                                        */
/* ----------------------------------------------------- */


int i_newfd = 0;
static struct timeval i_to, *i_top = NULL;
static int (*flushf) () = NULL;


void
add_io(fd, timeout)
  int fd;
  int timeout;
{
  i_newfd = fd;
  if (timeout)
  {
    i_to.tv_sec = timeout;
    i_to.tv_usec = 0;
    i_top = &i_to;
  }
  else
    i_top = NULL;
}


void
add_flush(flushfunc)
  int (*flushfunc) ();
{
  flushf = flushfunc;
}


int
num_in_buf()
{
  return icurrchar - ibufsize;
}


//char watermode = -1;  /* Ptt ���y�^�U�Ϊ��Ѽ� */
extern  char no_oldmsg,oldmsg_count;

int
dogetch()
{
  int ch;

  if(currutmp) time(&currutmp->lastact);

  for (;;)
  {
    if (ibufsize == icurrchar)
    {
      fd_set readfds;
      struct timeval to;

      to.tv_sec = to.tv_usec = 0;
      FD_ZERO(&readfds);
      FD_SET(0, &readfds);
      if (i_newfd)
        FD_SET(i_newfd, &readfds);
      if ((ch = select(FD_SETSIZE, &readfds, NULL, NULL, &to)) <= 0)
      {
        if (flushf)
          (*flushf) ();

        if (dumb_term)
          oflush();
        else
          refresh();

        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        if (i_newfd)
          FD_SET(i_newfd, &readfds);

        while ((ch = select(FD_SETSIZE, &readfds, NULL, NULL, i_top)) < 0)
        {
          if (errno == EINTR)
            continue;
          else
          {
            perror("select");
            return -1;
          }
        }
        if (ch == 0)
          return I_TIMEOUT;
      }
      if (i_newfd && FD_ISSET(i_newfd, &readfds))
        return I_OTHERDATA;

      while ((ibufsize = read(0, inbuf, IBUFSIZE)) <= 0)
      {
        if (ibufsize == 0)
          longjmp(byebye, -1);
        if (ibufsize < 0 && errno != EINTR)
          longjmp(byebye, -1);
      }
      icurrchar = 0;
    }

    i_mode = INPUT_ACTIVE;
#ifdef GB_SUPPORT
    if(HAS_HABIT(HABIT_BIG5GB))
    {
      char *intmp;
      intmp = gb2big(inbuf, &ibufsize, 0);
      strcpy(inbuf,intmp);
    } 
#endif
    ch = inbuf[icurrchar++];
    return (ch);
  }
}


int
igetch()
{
    register int ch;
    while(ch = dogetch())
    {
     switch (ch)
      {
       case Ctrl('L'):
         redoscr();
         continue;
       case Ctrl('I'):
         if(currutmp != NULL && currutmp->mode == MMENU)
         {
           screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));
           vs_save(screen);
           t_idle();
           vs_restore(screen);
           continue;
         }
         else return(ch);
       case Ctrl('W'):
         if(currutmp != NULL && currutmp->mode)
         {
           screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));
           vs_save(screen);
           DL_func("SO/xyz.so:x_cdict");
           vs_restore(screen);
           continue;
         }
         else return(ch);
       case Ctrl('K'):
         if(currutmp != NULL && currutmp->mode != EDITING &&
            currutmp->mode != TALK && currutmp->mode)
         {
           update_data();

           if(HAS_HABIT(HABIT_BIG5GB))
             cuser.habit &= ~HABIT_BIG5GB;
           else
             cuser.habit |= HABIT_BIG5GB;

           substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* �O�� */
           redoscr();
           continue;
         }
         else return(ch);
       case Ctrl('Q'):  // wildcat : �ֳt���� :p
         if(currutmp->mode && currutmp->mode != READING)
         {
           if(answer("�T�w�n����?? (y/N)") != 'y')
             return(ch);
           update_data();
           u_exit("ABORT");
           pressanykey("���¥��{, �O�o�`�ӳ� !");
           log_usies1("EXP",NULL);
           exit(0);
         }
         else return (ch);

       case Ctrl('Z'):   /* wildcat:help everywhere */
       {
         static short re_entry = 0; /* CityLion: �����J��... */
         if (currutmp->mode == IDLE) return (ch);
         if(currutmp && !re_entry)
         {
           int mode0 = currutmp->mode;
           int stat0 = currstat;
           int more0 = inmore;
           int i;
           extern int roll;
           int old_roll = roll;
           int my_newfd = i_newfd;
           screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));

           re_entry = 1;
           vs_save(screen);
           i = show_help(currutmp->mode);

           currutmp->mode = mode0;
           currstat = stat0;
           inmore = more0;
           roll = old_roll;
           i_newfd = my_newfd;
           vs_restore(screen);
           re_entry = 0;
           continue;
         }
         else return (ch);
       }
       case Ctrl('U'):
         resetutmpent();
         if(currutmp != NULL && currutmp->mode != EDITING &&
            currutmp->mode != LUSERS && currutmp->mode)
         {
           int mode0 = currutmp->mode;
           int stat0 = currstat;
           screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));

           vs_save(screen);
           t_users();
           vs_restore(screen);

           currutmp->mode = mode0;
           currstat = stat0;

           continue;
         }
         else return (ch);

        case Ctrl('R'):
        {
          if(!HAS_PERM(PERM_PAGE)) return (ch);
          if(currutmp == NULL) return (ch);
          else if(watermode > 0)
          {
            watermode = (watermode + oldmsg_count)% oldmsg_count + 1;
            t_display_new();
            continue;
          }
          else if (!currutmp->mode && (currutmp->chatid[0] == 2 ||
               currutmp->chatid[0] == 3) && oldmsg_count && !watermode)
          {
            watermode=1;
            t_display_new();
            continue;
          }
          else if (currutmp->msgs[0].last_pid)
          {
            screenline* screen = (screenline *)calloc(t_lines, sizeof(screenline));
            vs_save(screen);
            watermode=1;
            t_display_new();
            my_write(currutmp->msgs[0].last_pid, "���y��^�h�G");
            vs_restore(screen);
            continue;
          }
          else return (ch);
        }


        case '\n':   /* Ptt�� \n���� */
           continue;
        case Ctrl('T'):
          if(watermode > 0 )
          {
            watermode = (watermode + oldmsg_count - 2 )% oldmsg_count + 1;
            t_display_new();
            continue;
          }

        default:
          return (ch);
       }
    }
}

int
offset_count(char *prompt)   // Robert Liu 20010813
{
  int i=0, j=0, off=0;
    for(i=0 ; i<strlen(prompt) ; i++)
    {
    if(prompt[i]==27) off=1;
    if(off==0) j++;
    if(prompt[i]=='m' && off==1) off=0;
    }
  return (strlen(prompt)-j);
}
                      

getdata(line, col, prompt, buf, len, echo, ans)
  int line, col;
  char *prompt, *buf, *ans;
  int len, echo;
{
  register int ch;
  int clen;
  int x, y , off_set=0;
  extern unsigned char scr_cols;
#define MAXLASTCMD 6
  static char lastcmd[MAXLASTCMD][80];


  if (prompt)
  {
    move(line, col);
    clrtoeol();
    outs(prompt);
    off_set=offset_count(prompt);
  }
  else
    clrtoeol();

  if (dumb_term || !echo || /* echo == PASS || */echo == 9)
  {                     /* shakalaca.990422: ���F��J passwd �ɦ��ϥ� */
    len--;              /* �U���o�q�{���X�O�S���ϥ� (!echo) */
    clen = 0;
    while ((ch = igetch()) != '\r')
    {
      if (ch == '\n')
        break;
      if (ch == '\177' || ch == Ctrl('H'))
      {
        if (!clen)
        {
          bell();
          continue;
        }
        clen--;
        if (echo)
        {
          ochar(Ctrl('H'));
          ochar(' ');
          ochar(Ctrl('H'));
        }
        continue;
      }

#ifdef BIT8
      if (!isprint2(ch))
#else
      if (!isprint(ch))
#endif

      {
        if (echo)
          bell();
        continue;
      }
      if (clen >= len)
      {
        if (echo)
          bell();
        continue;
      }
      buf[clen++] = ch;
      if (echo && echo != 9)
        ochar(/* echo == PASS ? '-' : */ch); /* shakalaca.990422: ���F passwd */
    }
    buf[clen] = '\0';
    outc('\n');
    oflush();
  }
  else
  {
   int cmdpos = MAXLASTCMD -1;
   int currchar = 0;
   int keydown;
   int dirty;

    getyx(&y, &x);
    standout();
    for (clen = len--; clen; clen--)
      outc(' ');
    standend();

    if (ans) {
       int i;

       strncpy(buf, ans, len);
       buf[len] = 0;
       for (i = strlen(buf) + 1; i < len; i++)
          buf[i] = 0;
       move(y, x);
       edit_outs(buf);
       clen = currchar = strlen(buf);

    }
    else
       memset(buf, 0, len);

    dirty = 0;
    x = x - off_set;
    while (move(y, x + currchar), (ch = igetkey()) != '\r')
    {
/*
woju
*/
       keydown = 0;
       switch (ch) {
       case Ctrl('Y'): {
          int i;

          if (clen && dirty) {
             for (i = MAXLASTCMD - 1; i; i--)
                strcpy(lastcmd[i], lastcmd[i - 1]);
             strncpy(lastcmd[0], buf, len);
          }

          move(y, x);
          for (clen = len--; clen; clen--)
            outc(' ');
          memset(buf, '\0', strlen(buf));
          clen = currchar = strlen(buf);
          continue;
          }

       case KEY_DOWN:
       case Ctrl('N'):
          keydown = 1;
       case Ctrl('P'):
       case KEY_UP: {
          int i;

          if (clen && dirty) {
             for (i = MAXLASTCMD - 1; i; i--)
                strcpy(lastcmd[i], lastcmd[i - 1]);
             strncpy(lastcmd[0], buf, len);
          }

          i = cmdpos;
          do {
             if (keydown)
                --cmdpos;
             else
                ++cmdpos;
             if (cmdpos < 0)
                cmdpos = MAXLASTCMD - 1;
             else if (cmdpos == MAXLASTCMD)
                cmdpos = 0;
          } while (cmdpos != i && (!*lastcmd[cmdpos]
                   || !strncmp(buf, lastcmd[cmdpos], len)));
          if (cmdpos == i)
             continue;

          strncpy(buf, lastcmd[cmdpos], len);
          buf[len] = 0;

          move(y, x);                   /* clrtoeof */
          for (i = 0; i <= clen; i++)
             outc(' ');
          move(y, x);

          if (echo == PASS)
            passwd_outs(buf);
          else
            edit_outs(buf);
          clen = currchar = strlen(buf);
          dirty = 0;
          continue;
       }
       case KEY_ESC:
         if (KEY_ESC_arg == 'c')
            capture_screen();
         if (KEY_ESC_arg == 'n' && currmode)
            edit_note();
         if (ch == 'U' && currstat != IDLE  &&
           !(currutmp->mode == 0 &&
           (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3)))
            t_users();
            continue;

/* yagami.000504 : ��Хi��̫e�γ̫� */ 
/* wildcat : ���� ctrl-a , ctrl-e �]�O�@�˪��� :p */
       case KEY_HOME:
         currchar = 0;
         break;
       case KEY_END:
         currchar = strlen(buf);
         break;

       case KEY_LEFT:
          if (currchar)
             --currchar;
          continue;
       case KEY_RIGHT:
          if (buf[currchar])
             ++currchar;
          continue;
       }

      if (ch == '\n' || ch == '\r')
         break;

      if (ch == Ctrl('I') && currstat != IDLE &&
          !(currutmp->mode == 0 &&
            (currutmp->chatid[0] == 2 || currutmp->chatid[0] == 3))) {
         t_idle();
         continue;
      }
      if (ch == '\177' || ch == Ctrl('H'))
      {
        if (currchar) {
           int i;

           currchar--;
           clen--;
           for (i = currchar; i <= clen; i++)
              buf[i] = buf[i + 1];
           move(y, x + clen);
           outc(' ');
           move(y, x);
           if (echo == PASS)
             passwd_outs(buf);
           else
             edit_outs(buf);
           dirty = 1;
        }
        continue;
      }
      if (ch == Ctrl('D')) {
        if (buf[currchar]) {
           int i;

           clen--;
           for (i = currchar; i <= clen; i++)
              buf[i] = buf[i + 1];
           move(y, x + clen);
           outc(' ');
           move(y, x);
           if (echo == PASS)
             passwd_outs(buf);
           else
             edit_outs(buf);
           dirty = 1;
        }
        continue;
      }
      if (ch == Ctrl('K')) {
         int i;

         buf[currchar] = 0;
         move(y, x + currchar);
         for (i = currchar; i < clen; i++)
            outc(' ');
         clen = currchar;
         dirty = 1;
         continue;
      }
      if (ch == Ctrl('A')) {
         currchar = 0;
         continue;
      }
      if (ch == Ctrl('E')) {
         currchar = clen;
         continue;
      }


      if (!(isprint2(ch)))
      {
        continue;
      }
      if (clen >= len || x + clen >= scr_cols)
      {
        continue;
      }
/*
woju
*/
      if (buf[currchar]) {               /* insert */
         int i;

         for (i = currchar; buf[i] && i < len && i < 80; i++)
            ;
         buf[i + 1] = 0;
         for (; i > currchar; i--)
            buf[i] = buf[i - 1];
      }
      else                              /* append */
         buf[currchar + 1] = '\0';

      buf[currchar] = ch;
      move(y, x + currchar);
      if (echo == PASS)
        passwd_outs(buf + currchar);
      else
      /* shakalaca.990422: �쥻�u���U������, �o�O���F��J passwd ���ϥ� */
        edit_outs(buf + currchar);
      currchar++;
      clen++;
      dirty = 1;
    }
    buf[clen] = '\0';
    if (clen > 1 && echo != PASS && echo != EDIT) {
    /* shaklaaca.990514: ^^^^^^^ ������J�� password �d�U���� */
       for (cmdpos = MAXLASTCMD - 1; cmdpos; cmdpos--)
          strcpy(lastcmd[cmdpos], lastcmd[cmdpos - 1]);
       strncpy(lastcmd[0], buf, len);
    }
    if (echo) {
      move(y, x + clen);
      outc('\n');
    }
    refresh();
  }
  if ((echo == LCECHO) && ((ch = buf[0]) >= 'A') && (ch <= 'Z'))
    buf[0] = ch | 32;
  return clen;
}


char
getans(prompt)
  char *prompt;
{
  char ans[5];

  getdata(b_lines,0,prompt,ans,4,LCECHO,0);

  return ans[0];
}

/*
woju
*/
#define TRAP_ESC

#ifdef  TRAP_ESC
int KEY_ESC_arg;

int
igetkey()
{
  int mode;
  int ch, last;

  mode = last = 0;
  while (1)
  {
    ch = igetch();
    if (mode == 0)
    {
      if (ch == KEY_ESC)
        mode = 1;
      else
        return ch;              /* Normal Key */
    }
    else if (mode == 1)
    {                           /* Escape sequence */
      if (ch == '[' || ch == 'O')
        mode = 2;
      else if (ch == '1' || ch == '4')
        mode = 3;
      else
      {
        KEY_ESC_arg = ch;
        return KEY_ESC;
      }
    }
    else if (mode == 2)
    {                           /* Cursor key */
      if (ch >= 'A' && ch <= 'D')
        return KEY_UP + (ch - 'A');
      else if (ch >= '1' && ch <= '6')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 3)
    {                           /* Ins Del Home End PgUp PgDn */
      if (ch == '~')
        return KEY_HOME + (last - '1');
      else
        return ch;
    }
    last = ch;
  }
}

#else                           /* TRAP_ESC */

int
igetkey(void)
{
  int mode;
  int ch, last;

  mode = last = 0;
  while (1)
  {
    ch = igetch();
    if (ch == KEY_ESC)
      mode = 1;
    else if (mode == 0)         /* Normal Key */
      return ch;
    else if (mode == 1)
    {                           /* Escape sequence */
      if (ch == '[' || ch == 'O')
        mode = 2;
      else if (ch == '1' || ch == '4')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 2)
    {                           /* Cursor key */
      if (ch >= 'A' && ch <= 'D')
        return KEY_UP + (ch - 'A');
      else if (ch >= '1' && ch <= '6')
        mode = 3;
      else
        return ch;
    }
    else if (mode == 3)
    {                           /* Ins Del Home End PgUp PgDn */
      if (ch == '~')
        return KEY_HOME + (last - '1');
      else
        return ch;
    }
    last = ch;
  }
}
#endif                          /* TRAP_ESC */
