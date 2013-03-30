
/*-------------------------------------------------------*/
/* xchatd.c     ( NTHU CS MapleBBS Ver 3.00 )            */
/*-------------------------------------------------------*/
/* target : super KTV daemon for chat server             */
/* create : 95/03/29                                     */
/* update : 2001/07/29  ( Robert Liu)                    */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "xchat.h"

#define _BBS_UTIL_C_

#include "cache.c"
#include "record.c"

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../lib/shm.c"
#include "../lib/sem.c"
#include "../lib/str_time.c"
#include "../lib/rec_get.c"

#define STAND_ALONE              /* ���f�t BBS �W�߰��� */

#define SERVER_USAGE
#define WATCH_DOG
#define  MONITOR_                 /* �ʷ� chatroom ���ʥH�ѨM�ȯ� */
#define  DEBUG

#ifdef  DEBUG
#define MONITOR_
#endif

static int gline;

#ifdef  WATCH_DOG
#define MYDOG  gline = __LINE__
#else
#define MYDOG                   /* NOOP */
#endif



#define CHAT_PIDFILE    "log/chat.pid"
#define CHAT_LOGFILE    "log/chat.log"
#define CHAT_INTERVAL   (60 * 30)
#define SOCK_QLEN       1


/* name of the main room (always exists) */


#define MAIN_NAME       "main"
#define MAIN_TOPIC      "���ӳ��y��Ѽs��"


#define ROOM_LOCKED     1
#define ROOM_SECRET     2
#define ROOM_OPENTOPIC  4
#define ROOM_HANDUP     8
#define ROOM_ALL        (NULL)


#define LOCKED(room)    (room->rflag & ROOM_LOCKED)
#define SECRET(room)    (room->rflag & ROOM_SECRET)
#define OPENTOPIC(room) (room->rflag & ROOM_OPENTOPIC)
#define RHANDUP(room)    (room->rflag & ROOM_HANDUP)

#define RESTRICTED(usr) (usr->uflag == 0)       /* guest */
#define CHATSYSOP(usr)  (usr->uflag & ( PERM_SYSOP | PERM_CHATROOM))
#define PERM_ROOMOP     PERM_CHAT       /* Thor: �� PERM_CHAT�� PERM_ROOMOP */
#define PERM_HANDUP     PERM_BM  /* �� PERM_BM �����S���|��L */
#define PERM_SAY        PERM_NOTOP /* �� PERM_NOTOP �����S���o���v */

/* �i�J�ɻݲM��              */
/* Thor: ROOMOP���ж��޲z�� */
#define ROOMOP(usr)  (usr->uflag & ( PERM_ROOMOP | PERM_SYSOP | PERM_CHATROOM))
#define CLOAK(usr)      (usr->uflag & PERM_CLOAK)
#define HANDUP(usr)  (usr->uflag & PERM_HANDUP)
#define SAY(usr)      (usr->uflag & PERM_SAY)
 /* Thor: ��ѫ������N */


/* ----------------------------------------------------- */
/* ChatRoom data structure                               */
/* ----------------------------------------------------- */

typedef struct ChatRoom ChatRoom;
typedef struct ChatUser ChatUser;
typedef struct UserList UserList;
typedef struct ChatCmd ChatCmd;
typedef struct ChatAction ChatAction;

struct ChatUser
{
  struct ChatUser *unext;
  int sock;                     /* user socket */
  int talksock;                 /* talk socket */
  ChatRoom *room;
  UserList *ignore;
  int userno;
  int uflag;
  int clitype;                  /* Xshadow: client type. 1 for common client,
                                 * 0 for bbs only client */
  time_t uptime;                /* Thor: unused */
  char userid[IDLEN + 1];       /* real userid */
  char chatid[IDLEN + 1];               /* chat id */
  char lasthost[32];            /* host address */
  char ibuf[80];                /* buffer for non-blocking receiving */
  int isize;                    /* current size of ibuf */
  int color;
};


struct ChatRoom
{
  struct ChatRoom *next, *prev;
  char name[IDLEN+1];
  char topic[48];               /* Let the room op to define room topic */
  int rflag;                    /* ROOM_LOCKED, ROOM_SECRET, ROOM_OPENTOPIC */
  int occupants;                /* number of users in room */
  UserList *invite;
};


struct UserList
{
  struct UserList *next;
  int userno;
  char userid[IDLEN + 1];
};


struct ChatCmd
{
  char *cmdstr;
  void (*cmdfunc) ();
  int exact;
};


static ChatRoom mainroom;
static ChatUser *mainuser;
static fd_set mainfds;
static int maxfds;              /* number of sockets to select on */
static int totaluser;           /* current number of connections */
static struct timeval zerotv;   /* timeval for selecting */
static char chatbuf[256];       /* general purpose buffer */
static int common_client_command;
static char currchatroom[IDLEN+1];

#ifdef STAND_ALONE
static int userno_inc = 0;      /* userno auto-incrementer */
#endif

static char msg_not_op[] = "�� �z���O�o����ѫǪ� Op";
static char msg_no_such_id[] = "�� �ثe�S���H�ϥ� [%s] �o�Ӳ�ѥN��";
static char msg_not_here[] = "�� [%s] ���b�o����ѫ�";


#define FUZZY_USER      ((ChatUser *) -1)


#ifndef STAND_ALONE
typedef struct userec ACCT;

/* ----------------------------------------------------- */
/* acct_load for check acct                              */
/* ----------------------------------------------------- */

int
acct_load(acct, userid)
  ACCT *acct;
  char *userid;
{
  rec_get(FN_PASSWD, acct, sizeof(ACCT), searchuser(userid));
}



/* ----------------------------------------------------- */
/* str_lower for check acct                              */
/* ----------------------------------------------------- */
void
str_lower(dst, src)
  char *dst, *src;
{
  register int ch;

  do
  {
    ch = *src++;
    if (ch >= 'A' && ch <= 'Z')
      ch |= 0x20;
    *dst++ = ch;
  } while (ch);
}

/*
 * str_ncpy() - similar to strncpy(3) but terminates string always with '\0'
 * if n != 0, and doesn't do padding
 */

void
str_ncpy(dst, src, n)
  char *dst;
  char *src;
  int n;
{
  char *end;

  end = dst + n;

  do
  {
    n = (dst == end) ? 0 : *src++;
    *dst++ = n;
  } while (n);
}


/* ----------------------------------------------------- */
/* usr_fpath for check acct                              */
/* ----------------------------------------------------- */
char *str_home_file = "home/%s/%s";

void
usr_fpath(buf, userid, fname)
  char *buf, *userid, *fname;
{
  sprintf(buf, str_home_file, userid, fname);
}

/* ----------------------------------------------------- */
/* chkpasswd for check passwd                            */
/* ----------------------------------------------------- */
char *crypt();
static char pwbuf[PASSLEN];

int
chkpasswd(passwd, test)
  char *passwd, *test;
{
  char *pw;

  str_ncpy(pwbuf, test, PASSLEN);
  pw = crypt(pwbuf, passwd);
  return (!strncmp(pw, passwd, PASSLEN));
}
#endif                          /* STAND_ALONE */


/* ----------------------------------------------------- */
/* operation log and debug information                   */
/* ----------------------------------------------------- */


static int flog;                /* log file descriptor */


static void
logit(key, msg, etc)
  char *key;
  char *msg;
  char *etc;
{
  time_t now;
  struct tm *p;
  char buf[256];

  time(&now);
  p = localtime(&now);
  sprintf(buf, "%02d/%02d %02d:%02d:%02d %-13s%s (%s)\n",
    p->tm_mon + 1, p->tm_mday,
    p->tm_hour, p->tm_min, p->tm_sec, key, msg, etc);
  write(flog, buf, strlen(buf));
}


static void
log_init()
{
  flog = open(CHAT_LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  logit("START", "chat daemon", "main");
}


static void
log_close()
{
  close(flog);
}


#ifdef  DEBUG
/*
static void
debug_user()
{
  register ChatUser *user;
  int i;
  char buf[80];

  i = 0;
  for (user = mainuser; user; user = user->unext)
  {
    sprintf(buf, "%d) %s %s", ++i, user->userid, user->chatid);
    logit("DEBUG_U", buf, "debug");
  }
}


static void
debug_room()
{
  register ChatRoom *room;
  int i;
  char buf[80];

  i = 0;
  room = &mainroom;

  do
  {
    sprintf(buf, "%d) %s %d", ++i, room->name, room->occupants);
    logit("DEBUG_R", buf, "debug");
  } while (room = room->next);
}
*/
#endif                          /* DEBUG */


/* ----------------------------------------------------- */
/* string routines                                       */
/* ----------------------------------------------------- */


static int
valid_chatid(id)
  register char *id;
{
  register int ch, len;

  for (len = 0; ch = *id; id++)
  {                             /* Thor: check for endless */
    MYDOG;

    if (ch == '/' || ch == '*' || ch == ':')
      return 0;
    if (++len > 8)
      return 0;
  }
  return len;
}

#if 0
static int
Isspace(ch)
  int ch;
{
  return (ch == ' ' || ch == '\t' || ch == 10 || ch == 13);
}

static char *
nextword(str)
  char **str;
{
  char *head, *tail;
  int ch;

  head = *str;
  for (;;)
  {                             /* Thor: check for endless */
    MYDOG;

    ch = *head;
    if (!ch)
    {
      *str = head;
      return head;
    }
    if (!Isspace(ch))
      break;
    head++;
  }

  tail = head + 1;
  while (ch = *tail)
  {                             /* Thor: check for endless */
    MYDOG;

    if (Isspace(ch))
    {
      *tail++ = '\0';
      break;
    }
    tail++;
  }
  *str = tail;

  return head;
}
#endif

/* Case Independent strcmp : 1 ==> euqal */


static int
str_equal(s1, s2)
  register unsigned char *s1, *s2;      /* Thor: �[�W unsigned,
                                         * �קK���媺���D */
{
  register int c1, c2;

  for (;;)
  {                             /* Thor: check for endless */
    MYDOG;

    c1 = *s1;
    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;

    c2 = *s2;
    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;

    if (c1 != c2)
      return 0;

    if (!c1)
      return 1;

    s1++;
    s2++;
  }
}


/* ----------------------------------------------------- */
/* match strings' similarity case-insensitively          */
/* ----------------------------------------------------- */
/* str_match(keyword, string)                            */
/* ----------------------------------------------------- */
/* 0 : equal            ("foo", "foo")                   */
/* -1 : mismatch        ("abc", "xyz")                   */
/* ow : similar         ("goo", "good")                  */
/* ----------------------------------------------------- */


static int
str_match(s1, s2)
  register unsigned char *s1, *s2;      /* Thor: �[�W unsigned,
                                         * �קK���媺���D */
{
  register int c1, c2;

  for (;;)
  {                             /* Thor: check for endless */
    MYDOG;

    c2 = *s2;
    c1 = *s1;
    if (!c1)
    {
      return c2;
    }

    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;

    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;

    if (c1 != c2)
      return -1;

    s1++;
    s2++;
  }
}


/* ----------------------------------------------------- */
/* search user/room by its ID                            */
/* ----------------------------------------------------- */


static ChatUser *
cuser_by_userid(userid)
  char *userid;
{
  register ChatUser *cu;

  for (cu = mainuser; cu; cu = cu->unext)
  {
    MYDOG;

    if (str_equal(userid, cu->userid))
      break;
  }
  return cu;
}


static ChatUser *
cuser_by_chatid(chatid)
  char *chatid;
{
  register ChatUser *cu;

  for (cu = mainuser; cu; cu = cu->unext)
  {
    MYDOG;

    if (str_equal(chatid, cu->chatid))
      break;
  }
  return cu;
}


static ChatUser *
fuzzy_cuser_by_chatid(chatid)
  char *chatid;
{
  register ChatUser *cu, *xuser;
  int mode;

  xuser = NULL;

  for (cu = mainuser; cu; cu = cu->unext)
  {
    MYDOG;

    mode = str_match(chatid, cu->chatid);
    if (mode == 0)
      return cu;

    if (mode > 0)
    {
      if (xuser == NULL)
        xuser = cu;
      else
        return FUZZY_USER;      /* �ŦX�̤j�� 2 �H */
    }
  }
  return xuser;
}


static ChatRoom *
croom_by_roomid(roomid)
  char *roomid;
{
  register ChatRoom *room;

  room = &mainroom;
  do
  {
    MYDOG;

    if (str_equal(roomid, room->name))
      break;
  } while (room = room->next);
  return room;
}


/* ----------------------------------------------------- */
/* UserList routines                                     */
/* ----------------------------------------------------- */


static void
list_free(list)
  UserList *list;
{
  UserList *tmp;

  while (list)
  {
    MYDOG;

    tmp = list->next;

    free(list);
    MYDOG;
    list = tmp;
  }
}


static void
list_add(list, user)
  UserList **list;
  ChatUser *user;
{
  UserList *node;

  MYDOG;

  if (node = (UserList *) malloc(sizeof(UserList)))
  {                             /* Thor: ����Ŷ����� */
    strcpy(node->userid, user->userid);
    node->userno = user->userno;
    node->next = *list;
    *list = node;
  }
  MYDOG;
}


static int
list_delete(list, userid)
  UserList **list;
  char *userid;
{
  UserList *node;

  while (node = *list)
  {
    MYDOG;

    if (str_equal(node->userid, userid))
    {
      *list = node->next;
      MYDOG;
      free(node);
      MYDOG;
      return 1;
    }
    list = &node->next;         /* Thor: list�n��۫e�i */
  }

  return 0;
}


static int
list_belong(list, userno)
  UserList *list;
  int userno;
{
  while (list)
  {
    MYDOG;

    if (userno == list->userno)
      return 1;
    list = list->next;
  }
  return 0;
}


/* ------------------------------------------------------ */
/* non-blocking socket routines : send message to users   */
/* ------------------------------------------------------ */


static void
do_send(nfds, wset, msg, number)
  int nfds;
  fd_set *wset;
  char *msg;
  int number;
{
  int sr;

  /* Thor: for future reservation bug */
  zerotv.tv_sec = 0;
  zerotv.tv_usec = 0;

  MYDOG;

  sr = select(nfds + 1, NULL, wset, NULL, &zerotv);

  MYDOG;

  if (sr > 0)
  {
    register int len;

    len = strlen(msg) + 1;
    while (nfds >= 0)
    {
      MYDOG;

      if (FD_ISSET(nfds, wset))
      {
        MYDOG;
        send(nfds, msg, len, 0);/* Thor: �p�Gbuffer���F, ���| block */
        MYDOG;
        if (--sr <= 0)
          return;
      }
      nfds--;
    }
  }
}


static void
send_to_room(room, msg, userno, number)
  ChatRoom *room;
  char *msg;
  int userno;
  int number;
{
  ChatUser *cu;
  fd_set wset, *wptr;
  int sock, max;
  static char sendbuf[256];
  int clitype;                  /* ���� bbs client �� common client �⦸�B�z */

  for (clitype = (number == MSG_MESSAGE || !number) ? 0 : 1; clitype < 2; clitype++)
  {

    FD_ZERO(wptr = &wset);
    max = -1;

    for (cu = mainuser; cu; cu = cu->unext)
    {
      MYDOG;

      if (room == cu->room || room == ROOM_ALL)
      {
        if (cu->clitype == clitype && (!userno || !list_belong(cu->ignore, userno)))
        {
          sock = cu->sock;
          FD_SET(sock, wptr);
          if (max < sock)
            max = sock;
        }
      }
    }

    if (max < 0)
      continue;

    if (clitype)
    {
      if (strlen(msg))
        sprintf(sendbuf, "%3d %s", number, msg);
      else
        sprintf(sendbuf, "%3d", number);

      do_send(max, wptr, sendbuf);
    }
    else
      do_send(max, wptr, msg);
  }
}


static void
send_to_user(user, msg, userno, number)
  ChatUser *user;
  char *msg;
  int userno;
  int number;
{
  if (!user->clitype && number && number != MSG_MESSAGE)
    return;

  if (!userno || !list_belong(user->ignore, userno))
  {
    fd_set wset, *wptr;
    int sock;
    static char sendbuf[256];

    sock = user->sock;
    FD_ZERO(wptr = &wset);
    FD_SET(sock, wptr);

    if (user->clitype)
    {
      if (strlen(msg))
        sprintf(sendbuf, "%3d %s", number, msg);
      else
        sprintf(sendbuf, "%3d", number);
      do_send(sock, wptr, sendbuf);
    }
    else
      do_send(sock, wptr, msg);
  }
}

#if 0
static void
send_to_sock(sock, msg)         /* Thor: unused */
  int sock;
  char *msg;
{
  fd_set wset, *wptr;

  FD_ZERO(wptr = &wset);
  FD_SET(sock, wptr);
  do_send(sock, wptr, msg);
}
#endif

/* ----------------------------------------------------- */

static void
room_changed(room)
  ChatRoom *room;
{
  if (!room)
    return;

  sprintf(chatbuf, "= %s %d %d %s", room->name, room->occupants, room->rflag, room->topic);
  send_to_room(ROOM_ALL, chatbuf, 0, MSG_ROOMNOTIFY);
}

static void
user_changed(cu)
  ChatUser *cu;
{
  if (!cu)
    return;

  sprintf(chatbuf, "= %s %s %d %s %s", cu->userid, cu->chatid, cu->color, cu->room->name, cu->lasthost);
  if (ROOMOP(cu))
    strcat(chatbuf, " Op");
  send_to_room(cu->room, chatbuf, 0, MSG_USERNOTIFY);
}

static void
exit_room(user, state, msg, mode)
  ChatUser *user;
  int state;
  char *msg;
  int mode;
{
  ChatRoom *room;

  logit("exit:", user->userid, user->room->name);

  if (room = user->room)
  {
    user->room = NULL;
    user->uflag &= ~PERM_ROOMOP;

    if (--room->occupants > 0)
    {
      char *chatid;

      chatid = user->chatid;
      switch (state)
      {
      case EXIT_LOGOUT:

        sprintf(chatbuf, "�� %s ���}�F ...", chatid);
        if (msg && *msg)
        {
          strcat(chatbuf, ": ");
          msg[79] = 0;          /* Thor:����Ӫ� */
          strncat(chatbuf, msg, 80);
        }
        break;

      case EXIT_LOSTCONN:

        sprintf(chatbuf, "�� %s ���F�_�u�������o", chatid);
        break;

      case EXIT_KICK:

        sprintf(chatbuf, "�� �����I%s �Q��X�h�F", chatid);
        break;
      }
                                /* Thor: ��ѫ������N */
//      if (!CLOAK(user))

     if(mode)
        send_to_room(room, chatbuf, 0, MSG_MESSAGE);

      sprintf(chatbuf, "- %s", user->userid);
      send_to_room(room, chatbuf, 0, MSG_USERNOTIFY);
      room_changed(room);

      return;
    }

    else if (room != &mainroom)
    {                           /* Thor: �H�Ƭ�0��,���Omainroom�~free */
      register ChatRoom *next;

#ifdef  DEBUG
//      debug_room();
#endif

      sprintf(chatbuf, "- %s", room->name);
      send_to_room(ROOM_ALL, chatbuf, 0, MSG_ROOMNOTIFY);

      room->prev->next = room->next;
      if (next = room->next)
        next->prev = room->prev;
      list_free(room->invite);

      MYDOG;
      free(room);
      MYDOG;

#ifdef  DEBUG
//      debug_room();
#endif
    }
  }
}


/* ----------------------------------------------------- */
/* chat commands                                         */
/* ----------------------------------------------------- */

#ifndef STAND_ALONE
/* ----------------------------------------------------- */
/* (.ACCT) �ϥΪ̱b�� (account) subroutines              */
/* ----------------------------------------------------- */

static char datemsg[32];

char *
Ctime(clock)
  time_t *clock;
{
  struct tm *t = localtime(clock);
  static char week[] = "��@�G�T�|����";

  sprintf(datemsg, "%d�~%2d��%2d��%3d:%02d:%02d �P��%.2s",
    t->tm_year - 11, t->tm_mon + 1, t->tm_mday,
    t->tm_hour, t->tm_min, t->tm_sec, &week[t->tm_wday << 1]);
  return (datemsg);
}

static void
chat_query(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char str[256];
  int i;
  ACCT xuser;
  FILE *fp;

  if (acct_load(&xuser, msg) >= 0)
  {
    sprintf(chatbuf, "%s(%s) �@�W�� %d ���A�峹 %d �g",
      xuser.userid, xuser.username, xuser.numlogins, xuser.numposts);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);

    sprintf(chatbuf, "�̪�(%s)�q(%s)�W��", Ctime(&xuser.lastlogin),
      (xuser.lasthost[0] ? xuser.lasthost : "�~�Ӫ�"));
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);

//    usr_fpath(chatbuf, xuser.userid, "plans");
    sethomefile(chatbuf, xuser.userid, "plans");
    fp = fopen(chatbuf, "rt");
    i = 0;
    while (fp && fgets(str, 256, fp))
    {
      if (!strlen(str))
        continue;

      str[strlen(str) - 1] = 0;
      send_to_user(cu, str, 0, MSG_MESSAGE);
      if (++i >= MAXQUERYLINES)
        break;
    }
    fclose(fp);
  }
  else
  {
    sprintf(chatbuf, msg_no_such_id, msg);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
  }
}
#endif

static void
chat_clear(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (cu->clitype)
    send_to_user(cu, "", 0, MSG_CLRSCR);
  else
    send_to_user(cu, "/c", 0, MSG_MESSAGE);
}

static void
chat_date(cu, msg)
  ChatUser *cu;
  char *msg;
{
  time_t thetime;

  time(&thetime);
  sprintf(chatbuf, "�� �зǮɶ�: %s", Ctime(&thetime));
  send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_topic(cu, msg)
  ChatUser *cu;
  char *msg;
{
  ChatRoom *room;
  char *topic;

  if (!ROOMOP(cu) && !OPENTOPIC(cu->room))
  {
    send_to_user(cu, msg_not_op, 0, MSG_MESSAGE);
    return;
  }

  if (*msg == '\0')
  {
    send_to_user(cu, "�� �Ы��w���D", 0, MSG_MESSAGE);
    return;
  }

  room = cu->room;
  topic = room->topic;          /* Thor: room ���i�� NULL��?? */
  strncpy(topic, msg, 47);
  topic[47] = '\0';

  if (cu->clitype)
    send_to_room(room, topic, 0, MSG_TOPIC);
  else
  {
    sprintf(chatbuf, "/t%s", topic);
    send_to_room(room, chatbuf, 0, 0);
  }

  room_changed(room);

  sprintf(chatbuf, "�� %s �N���D�אּ [1;32m%s[m", cu->chatid, topic);
  if (!CLOAK(cu))               /* Thor: ��ѫ������N */
    send_to_room(room, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_version(cu, msg)
  ChatUser *cu;
  char *msg;
{
  sprintf(chatbuf, "%d %d", XCHAT_VERSION_MAJOR, XCHAT_VERSION_MINOR);
  send_to_user(cu, chatbuf, 0, MSG_VERSION);
}

static void
chat_nick(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *chatid, *str;
  ChatUser *xuser;

  chatid = nextword(&msg);
  chatid[8] = '\0';
  if (!valid_chatid(chatid))
  {
    send_to_user(cu, "�� �o�Ӳ�ѥN���O�����T��", 0, MSG_MESSAGE);
    return;
  }

  xuser = cuser_by_chatid(chatid);
  if (xuser != NULL && xuser != cu)
  {
    send_to_user(cu, "�� �w�g���H�������n�o", 0, MSG_MESSAGE);
    return;
  }

  str = cu->chatid;

  sprintf(chatbuf, "�� %s �N��ѥN���אּ [1;33m%s[m", str, chatid);
  if (!CLOAK(cu))               /* Thor: ��ѫ������N */
    send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);

  strcpy(str, chatid);

  user_changed(cu);

  if (cu->clitype)
    send_to_user(cu, chatid, 0, MSG_NICK);
  else
  {
    sprintf(chatbuf, "/n%s", chatid);
    send_to_user(cu, chatbuf, 0, 0);
  }
}

static void
chat_color(cu, msg)
  ChatUser *cu;
  char *msg;
{
  int color;

  color = atoi(nextword(&msg));
  if (color >= 8 || color < 1 || color == 4)
  {
    send_to_user(cu, "�� �п�J 1~7 ���Ʀr , ���F 4 ���~", 0, MSG_MESSAGE);
    return;
  }

  sprintf(chatbuf, "�� %s ����C�⬰ [1;3%dm�o��[m", cu->chatid, color);
  if (!CLOAK(cu))               /* Thor: ��ѫ������N */
    send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);

  cu->color = color;
}

static void
chat_list_rooms(cuser, msg)
  ChatUser *cuser;
  char *msg;
{
  ChatRoom *cr, *room;

  if (RESTRICTED(cuser))
  {
    send_to_user(cuser, "�� �z�S���v���C�X�{������ѫ�", 0, MSG_MESSAGE);
    return;
  }

  if (common_client_command)
    send_to_user(cuser, "", 0, MSG_ROOMLISTSTART);
  else
    send_to_user(cuser, "[7m �ͤѫǦW��  �x�H�Ƣx���D        [m", 0, MSG_MESSAGE);

  room = cuser->room;
  cr = &mainroom;
  do
  {
    MYDOG;


    if (!SECRET(cr) || CHATSYSOP(cuser) || (cr == room && ROOMOP(cuser)))
    {
      if (common_client_command)
      {
        sprintf(chatbuf, "%s %d %d %s", cr->name, cr->occupants, cr->rflag, cr->topic);
        send_to_user(cuser, chatbuf, 0, MSG_ROOMLIST);
      }
      else
      {
        sprintf(chatbuf, " %-12s�x%4d�x%s", cr->name, cr->occupants, cr->topic);
        if (LOCKED(cr))
          strcat(chatbuf, " [���]");
        if (SECRET(cr))
          strcat(chatbuf, " [���K]");
        if (OPENTOPIC(cr))
          strcat(chatbuf, " [���D]");
        send_to_user(cuser, chatbuf, 0, MSG_MESSAGE);
      }

    }
  } while (cr = cr->next);

  if (common_client_command)
    send_to_user(cuser, "", 0, MSG_ROOMLISTEND);
}



static void
chat_do_user_list(cu, msg, theroom)
  ChatUser *cu;
  char *msg;
  ChatRoom *theroom;
{
  ChatRoom *myroom, *room;
  ChatUser *user;

  int start, stop, curr;
  start = atoi(nextword(&msg));
  stop = atoi(nextword(&msg));

  myroom = cu->room;

#ifdef DEBUG
//  logit(cu->chatid, "do user list", "");
#endif

  if (common_client_command)
    send_to_user(cu, "", 0, MSG_USERLISTSTART);
  else
    send_to_user(cu, "[7m ��ѥN���x�ϥΪ̥N��  �x��ѫ� [m", 0, MSG_MESSAGE);

  for (user = mainuser; user; user = user->unext)
  {
    MYDOG;

#ifdef DEBUG
//    logit(cu->chatid, "list in for", "");
#endif

    room = user->room;
    if ((theroom != ROOM_ALL) && (theroom != room))
      continue;

    if (myroom != room)
    {
      if (RESTRICTED(cu) ||
        (room && SECRET(room) && !CHATSYSOP(cu)))
        continue;
    }

    if (CLOAK(user))            /* Thor: �����N */
      continue;

#ifdef DEBUG
//    logit(cu->chatid, "list in for 2", "");
#endif

    curr++;
    if (start && curr < start)
      continue;
    else if (stop && (curr > stop))
      break;

    if (common_client_command)
    {
      if (!room)
        continue;               /* Xshadow: �٨S�i�J����ж����N���C�X */

      sprintf(chatbuf, "%s %s %s %s", user->chatid, user->userid, room->name, user->lasthost);
      if (ROOMOP(user))
        strcat(chatbuf, " Op");
    }
    else
    {
      sprintf(chatbuf, " %-8s�x%-12s�x%s", user->chatid, user->userid, room ? room->name : "[�b���f�r��]");
      if (ROOMOP(user))
        strcat(chatbuf, " [Op]");
    }

#ifdef  DEBUG
//    logit("list_U", chatbuf, "");
#endif

    send_to_user(cu, chatbuf, 0, common_client_command ? MSG_USERLIST : MSG_MESSAGE);
  }
  if (common_client_command)
    send_to_user(cu, "", 0, MSG_USERLISTEND);
}

static void
chat_list_by_room(cu, msg)
  ChatUser *cu;
  char *msg;
{
  ChatRoom *whichroom;
  char *roomstr;

  roomstr = nextword(&msg);
  if (*roomstr == '\0')
    whichroom = cu->room;
  else
  {
    if ((whichroom = croom_by_roomid(roomstr)) == NULL)
    {
      sprintf(chatbuf, "�� �S�� [%s] �o�Ӳ�ѫ�", roomstr);
      send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
      return;
    }

    if (whichroom != cu->room && SECRET(whichroom) && !CHATSYSOP(cu))
    {                           /* Thor: �n���n���P�@room��SECRET���i�H�C?
                                 * Xshadow: �ڧ令�P�@ room �N�i�H�C */
      send_to_user(cu, "�� �L�k�C�X�b���K��ѫǪ��ϥΪ�", 0, MSG_MESSAGE);
      return;
    }
  }
  chat_do_user_list(cu, msg, whichroom);
}


static void
chat_list_users(cu, msg)
  ChatUser *cu;
  char *msg;
{
  chat_do_user_list(cu, msg, ROOM_ALL);
}

static void
chat_chatroom(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (common_client_command)
    send_to_user(cu, "���Я��� 4 21", 0, MSG_CHATROOM);
}

static void
chat_map_chatids(cu, whichroom)
  ChatUser *cu;                 /* Thor: �٨S���@���P���� */
  ChatRoom *whichroom;
{
  int c;
  ChatRoom *myroom, *room;
  ChatUser *user;

  /* myroom = cu->room; */
  myroom = whichroom;
  send_to_user(cu,
    "[7m ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N��  �x ��ѥN�� �ϥΪ̥N�� [m", 0, MSG_MESSAGE);

  c = 0;

  for (user = mainuser; user; user = user->unext)
  {
    MYDOG;

    room = user->room;
    MYDOG;
    if (whichroom != ROOM_ALL && whichroom != room)
      continue;
    MYDOG;
    if (myroom != room)
    {
      if (RESTRICTED(cu) ||     /* Thor: �n��check room �O���O�Ū� */
        (room && SECRET(room) && !CHATSYSOP(cu)))
        continue;
    }
    MYDOG;
    if (CLOAK(user))            /* Thor:�����N */
      continue;
    sprintf(chatbuf + (c * 24), " %-8s%c%-12s%s",
      user->chatid, ROOMOP(user) ? '*' : ' ',
      user->userid, (c < 2 ? "�x" : "  "));
    MYDOG;
    if (++c == 3)
    {
      send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
      c = 0;
    }
    MYDOG;
  }
  if (c > 0)
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_map_chatids_thisroom(cu, msg)
  ChatUser *cu;
  char *msg;
{
  chat_map_chatids(cu, cu->room);
}


static void
chat_setroom(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *modestr;
  ChatRoom *room;
  char *chatid;
  int sign;
  int flag;
  char *fstr;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0, MSG_MESSAGE);
    return;
  }

  modestr = nextword(&msg);
  sign = 1;
  if (*modestr == '+')
    modestr++;
  else if (*modestr == '-')
  {
    modestr++;
    sign = 0;
  }
  if (*modestr == '\0')
  {
    send_to_user(cu,
      "�� �Ы��w���A: {[+(�]�w)][-(����)]}{[l(���)][s(���K)][t(�}����D)}", 0, MSG_MESSAGE);
    return;
  }

  room = cu->room;
  chatid = cu->chatid;

  while (*modestr)
  {
    flag = 0;
    switch (*modestr)
    {
    case 'l':
    case 'L':
      flag = ROOM_LOCKED;
      fstr = "���";
      break;

    case 's':
    case 'S':
      flag = ROOM_SECRET;
      fstr = "���K";
      break;

    case 't':
    case 'T':
      flag = ROOM_OPENTOPIC;
      fstr = "�}����D";
      break;
    case 'h':
    case 'H':
      flag = ROOM_OPENTOPIC;
      fstr = "�|��o��";
      break;

    default:
      sprintf(chatbuf, "�� ���A���~�G[%c]", *modestr);
      send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    }

    /* Thor: check room �O���O�Ū�, ���Ӥ��O�Ū� */
    if (flag && (room->rflag & flag) != sign * flag)
    {
      room->rflag ^= flag;
      sprintf(chatbuf, "�� ����ѫǳQ %s %s [%s] ���A",
        chatid, sign ? "�]�w��" : "����", fstr);
      if (!CLOAK(cu))           /* Thor: ��ѫ������N */
        send_to_room(room, chatbuf, 0, MSG_MESSAGE);
    }
    modestr++;
  }
  room_changed(room);
}

static char *chat_msg[] =
{
  "[//]help", "MUD-like ����ʵ�",
  "[/h]elp op", "�ͤѫǺ޲z���M�Ϋ��O",
  "[/a]ct <msg>", "���@�Ӱʧ@",
  "[/b]ye [msg]", "�D�O",
  "[/c]lear  [/d]ate", "�M���ù�  �ثe�ɶ�",
  "[/co]lor", "�����C��",

#if 0
  "[/f]ire <user> <msg>", "�o�e���T",   /* Thor.0727: �M flag ��key */
#endif

  "[/i]gnore [user]", "�����ϥΪ�",
  "[/j]oin <room>", "�إߩΥ[�J�ͤѫ�",
  "[/l]ist [start [stop]]", "�C�X�ͤѫǨϥΪ�",
  "[/m]sg <id|user> <msg>", "�� <id> ��������",
  "[/n]ick <id>", "�N�ͤѥN������ <id>",
  "[/p]ager", "�����I�s��",
  "[/q]uery <user>", "�d�ߺ���",
  "[/r]oom  [/t]ape", "�C�X�@��ͤѫ�  �}��������",
  "[/u]nignore <user>", "��������",

#if 0
  "[/u]sers", "�C�X���W�ϥΪ�",
#endif

  "[/w]ho", "�C�X���ͤѫǨϥΪ�",
  "[/w]hoin <room>", "�C�X�ͤѫ�<room> ���ϥΪ�",
  NULL
};


static char *room_msg[] =
{
  "[/f]lag [+-][lsth]", "�]�w��w�B���K�B�}����D�B�|��o��",
  "[/i]nvite <id>", "�ܽ� <id> �[�J�ͤѫ�",
  "[/kick] <id>", "�N <id> ��X�ͤѫ�",
  "[/o]p <id>", "�N Op ���v�O�ಾ�� <id>",
  "[/topic] <text>", "���Ӹ��D",
  "[/w]all", "�s�� (�����M��)",
  NULL
};


static void
chat_help(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char **table, *str;

  if (str_equal(nextword(&msg), "op"))
  {
    send_to_user(cu, "�ͤѫǺ޲z���M�Ϋ��O", 0, MSG_MESSAGE);
    table = room_msg;
  }
  else
  {
    table = chat_msg;
  }

  while (str = *table++)
  {
    sprintf(chatbuf, "  %-20s- %s", str, *table++);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
  }
}


static void
chat_private(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *recipient;
  ChatUser *xuser;
  int userno;

  userno = 0;
  recipient = nextword(&msg);
  xuser = (ChatUser *) fuzzy_cuser_by_chatid(recipient);
  if (xuser == NULL)
  {                             /* Thor.0724: �� userid�]�i�Ǯ����� */
    xuser = cuser_by_userid(recipient);
  }
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, recipient);
  }
  else if (xuser == FUZZY_USER)
  {                             /* ambiguous */
    strcpy(chatbuf, "�� �Ы�����ѥN��");
  }
  else if (*msg)
  {
    userno = cu->userno;
    sprintf(chatbuf, "[1m*%s*[m ", cu->chatid);
    msg[79] = 0;                /* Thor:����Ӫ� */
    strncat(chatbuf, msg, 80);
    send_to_user(xuser, chatbuf, userno, MSG_MESSAGE);

    if (xuser->clitype)
    {                           /* Xshadow: �p�G���O�� client �W�Ӫ� */
      sprintf(chatbuf, "%s %s ", cu->userid, cu->chatid);
      msg[79] = 0;
      strncat(chatbuf, msg, 80);
      send_to_user(xuser, chatbuf, userno, MSG_PRIVMSG);
    }
    if (cu->clitype)
    {
      sprintf(chatbuf, "%s %s ", xuser->userid, xuser->chatid);
      msg[79] = 0;
      strncat(chatbuf, msg, 80);
      send_to_user(cu, chatbuf, 0, MSG_MYPRIVMSG);
    }

    sprintf(chatbuf, "%s> ", xuser->chatid);
    strncat(chatbuf, msg, 80);
  }
  else
  {
    sprintf(chatbuf, "�� �z�Q�� %s ������ܩO�H", xuser->chatid);
  }
  send_to_user(cu, chatbuf, userno, MSG_MESSAGE);       /* Thor: userno �n�令 0
                                                         * ��? */
}


static void
chat_cloak(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (CHATSYSOP(cu))
  {
    cu->uflag ^= PERM_CLOAK;
    sprintf(chatbuf, "�� %s", CLOAK(cu) ? MSG_CLOAKED : MSG_UNCLOAK);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
  }
}



/* ----------------------------------------------------- */


static void
arrive_room(cuser, room, mode)
  ChatUser *cuser;
  ChatRoom *room;
  int mode;
{
  int count;
  char *rname;

    sprintf(chatbuf, "+ %s %s %s %s", cuser->userid, cuser->chatid, room->name, cuser->lasthost);
    if (ROOMOP(cuser))
       strcat(chatbuf, " Op");
    send_to_room(room, chatbuf, 0, MSG_USERNOTIFY);

  cuser->room = room;
  room->occupants++;
  rname = room->name;

    room_changed(room);

  sprintf(currchatroom, "%s\0", cuser->room->name);
  count=chat_count_by_room(currchatroom);
  board_chatman(currchatroom, count, NULL);

    if (cuser->clitype)
    {
      send_to_user(cuser, rname, 0, MSG_ROOM);
      send_to_user(cuser, room->topic, 0, MSG_TOPIC);
    }
    else
    {
      sprintf(chatbuf, "/r%s", rname);
      send_to_user(cuser, chatbuf, 0, 0);
      sprintf(chatbuf, "/t%s", room->topic);
      send_to_user(cuser, chatbuf, 0, 0);
    }

    sprintf(chatbuf, "�� [1;32m%s[37m �i�J [33m[%s][37m �]�[�A�H�ơG[36m[%d][m",
      cuser->chatid, rname, count);
                             /* Thor: ��ѫ������N */
//    if (!CLOAK(cuser))
  if(mode)
      send_to_room(room, chatbuf, cuser->userno, MSG_MESSAGE);


}


static int
enter_room(cuser, rname, msg, mode)
  ChatUser *cuser;
  char *rname;
  char *msg;
  int mode;
{
  ChatRoom *room;
  int create;
  char buf[64], title[BTLEN+1]="�o�O�@�ӷs�Ѧa\0";

  create = 0;
  room = croom_by_roomid(rname);
  if (room == NULL)
  {
    /* new room */

//#ifdef  MONITOR_
    sprintf(buf, "create new room [%s]\0", rname);
    logit("monitor:", cuser->userid, buf);
//#endif

    MYDOG;

    room = (ChatRoom *) malloc(sizeof(ChatRoom));
    MYDOG;
    if (room == NULL)
    {
      send_to_user(cuser, "�� �L�k�A�s�P�]�[�F", 0, MSG_MESSAGE);
      return 0;
    }

    memset(room, 0, sizeof(ChatRoom));
    memcpy(room->name, rname, IDLEN - 1);

    if(board_chatman(rname, -1, &title))
      strcpy(room->topic, title);
    else
      strcpy(room->topic, "�o�O�@�ӷs�Ѧa");

    sprintf(chatbuf, "+ %s 1 0 %s", room->name, room->topic);
    send_to_room(ROOM_ALL, chatbuf, 0, MSG_ROOMNOTIFY);

    if (mainroom.next != NULL)
      mainroom.next->prev = room;
    room->next = mainroom.next;
    mainroom.next = room;
    room->prev = &mainroom;

    create = 1;
  }
  else
  {
    if (cuser->room == room)
    {
      sprintf(chatbuf, "�� �z���ӴN�b [%s] ��ѫ��o :)", rname);
      send_to_user(cuser, chatbuf, 0, MSG_MESSAGE);
      return 0;
    }

    if (!CHATSYSOP(cuser) && LOCKED(room) && !list_belong(room->invite, cuser->userno))
    {
      send_to_user(cuser, "�� �����c���A�D�в��J", 0, MSG_MESSAGE);
      return 0;
    }
  }

  exit_room(cuser, EXIT_LOGOUT, msg, mode);
  arrive_room(cuser, room, 1);

  if (create)
    cuser->uflag |= PERM_ROOMOP;

  return 0;
}

int
chat_count_by_room(roomname)  //�Y��ѫǤH�ƭp��
  char *roomname;
{
  int curr = 0;
  ChatUser *user;
  ChatRoom *room, *whichroom;

  if ((whichroom = croom_by_roomid(roomname)) == NULL)
      return 0;

  for (user = mainuser; user; user = user->unext)
  {
//    MYDOG;
    room = user->room;
    if(!room) continue;
    if(room != whichroom) continue;
    curr++;
  }
  return curr;
}


int
board_chatman(char *brd, int man, char *title)  //�ݪO��ѫǤH�Ʀs��
{
    boardheader bh;
    int bid;

    bid = getbnum(brd);
    if (rec_get(FN_BOARD, &bh, sizeof(bh), bid) == -1)
       return 0;
    if(title!=NULL)
      strcpy(title, &bh.title[7]);
    if(man>=0)
    {
//      bh.chatman = man;
//      if(bh.chatman<0) bh.chatman=0;
      substitute_record(FN_BOARD, &bh, sizeof(bh), bid);
      touch_boards();
    }
    return 1;
}

static void
logout_user(cuser)
  ChatUser *cuser;
{
  int sock;
  ChatUser *xuser, *prev;
  int count;

#ifdef  DEBUG
  logit("logout:", cuser->userid, "1");
//  debug_user();
#endif

  if(currchatroom[0])
  {
    count=chat_count_by_room(currchatroom);
//    logit("count:", currchatroom, count);
    board_chatman(currchatroom, count, NULL);
  }
  sock = cuser->sock;
  shutdown(sock, 2);
  close(sock);

  MYDOG;

  FD_CLR(sock, &mainfds);

#if 0   /* Thor: �]�\���t�o�@�� */
   if (sock >= maxfds)
     maxfds = sock - 1;
#endif

  list_free(cuser->ignore);

#ifdef DEBUG
//  debug_user();
#endif

  xuser = mainuser;
  if (xuser == cuser)
  {
    mainuser = cuser->unext;
  }
  else
  {
    do
    {
      prev = xuser;
      xuser = xuser->unext;
      if (xuser == cuser)
      {
        prev->unext = cuser->unext;
        break;
      }
    } while (xuser);
  }

  MYDOG;

#ifdef DEBUG
  sprintf(chatbuf, "%p", cuser);
//  logit("free cuser", chatbuf, "");
#endif

  free(cuser);

#ifdef  DEBUG
//  logit("after", "logout", "");
//  debug_user();
#endif

#if 0
  next = cuser->next;
  prev = cuser->prev;
  prev->next = next;
  if (next)
    next->prev = prev;

  if (cuser)
    free(cuser);
  MYDOG;

#endif

  totaluser--;
}


static void
print_user_counts(cuser)
  ChatUser *cuser;
{
  ChatRoom *room;
  int num, userc, suserc, roomc, number;

  userc = suserc = roomc = 0;

  room = &mainroom;
  do
  {
    MYDOG;

    num = room->occupants;
    if (SECRET(room))
    {
      suserc += num;
      if (CHATSYSOP(cuser))
        roomc++;
    }
    else
    {
      userc += num;
      roomc++;
    }
  } while (room = room->next);

  number = (cuser->clitype) ? MSG_MOTD : MSG_MESSAGE;

  sprintf(chatbuf,
    "�� �w����{�A�ثe�u�W�� [1;31m%d[m ���]�[�C", roomc);
  send_to_user(cuser, chatbuf, 0, number);

//  sprintf(chatbuf, "�� �@�� [1;36m%d[m �H���\\�s���}", userc);
  if (suserc)
  {
    sprintf(chatbuf + strlen(chatbuf), " [%d �H�b���K��ѫ�]", suserc);
    send_to_user(cuser, chatbuf, 0, number);
  }
}


static int
login_user(cu, msg)
  ChatUser *cu;
  char *msg;
{
  int utent;
  int level;
//  char buf[32];
  char *userid;
  char *chatid, *passwd, *board;
  struct sockaddr_in from;
  int fromlen;
  struct hostent *hp;

#ifndef STAND_ALONE
  ACCT acct;
#endif

  /*
   * Thor.0819: SECURED_CHATROOM : /! userid chatid passwd , userno
   * el �bcheck��passwd����o
   */
  /* Xshadow.0915: common client support : /-! userid chatid password */

  /* �ǰѼơGuserlevel, userid, chatid */

  /* client/server �����̾� userid �� .PASSWDS �P�_ userlevel */

  userid = nextword(&msg);
  chatid = nextword(&msg);
  passwd = nextword(&msg);
  board = nextword(&msg);

#ifdef  DEBUG
//  logit("ENTER", userid, chatid);
#endif

#ifndef STAND_ALONE

  /* Thor.0813: ���L�@�Ů�Y�i, �]���ϥ��p�Gchatid���Ů�, �K�X�]���� */
  /* �N��K�X��, �]���|����:p */
  /* �i�O�p�G�K�X�Ĥ@�Ӧr�O�Ů�, �����Ӧh�Ů�|�i����... */
  if (*passwd == ' ')
    passwd++;

  if (!*userid || (acct_load(&acct, userid) < 0))
  {

#ifdef  DEBUG
    logit("noexist", chatid, "");
#endif

    if (cu->clitype)
      send_to_user(cu, "���~���ϥΪ̥N��", 0, ERR_LOGIN_NOSUCHUSER);
    else
      send_to_user(cu, CHAT_LOGIN_INVALID, 0, 0);

    return -1;
  }
  else if(strncmp(passwd, acct.passwd, PASSLEN) &&
          !chkpasswd(acct.passwd, passwd))
  {

#ifdef  DEBUG
    logit("fake", chatid, "");
#endif

    if (cu->clitype)
      send_to_user(cu, "�K�X���~", 0, ERR_LOGIN_PASSERROR);
    else
      send_to_user(cu, CHAT_LOGIN_INVALID, 0, 0);
    return -1;
  }
  else
  {
    level = acct.userlevel;
    utent = searchuser(acct.userid);
  }
#else                           /* STAND_ALONE */
  {
    userec muser;
    int uid;

    nextword(&msg);
    uid = atoi(nextword(&msg));
    rec_get(BBSHOME"/.PASSWDS", &muser, sizeof(muser), uid);
    level = muser.userlevel;
    utent = ++userno_inc;
  }
#endif                          /* STAND_ALONE */

  if (!valid_chatid(chatid))
  {

#ifdef  DEBUG
    logit("enter:", chatid, "ok");
#endif

    if (cu->clitype)
      send_to_user(cu, "���X�k����ѫǥN�� !!", 0, ERR_LOGIN_NICKERROR);
    else
      send_to_user(cu, CHAT_LOGIN_INVALID, 0, 0);
    return 0;
  }

#ifdef  DEBUG
//  debug_user();
#endif

  if (cuser_by_chatid(chatid) != NULL)
  {
    /* chatid in use */

#ifdef  DEBUG
//    logit("enter", "duplicate", "");
#endif

    if (cu->clitype)
      send_to_user(cu, "�o�ӥN���w�g���H�ϥ�", 0, ERR_LOGIN_NICKINUSE);
    else
      send_to_user(cu, CHAT_LOGIN_EXISTS, 0, 0);
    return 0;
  }

  cu->userno = utent;
//  cu->uflag = level & ~(PERM_ROOMOP | PERM_CLOAK | PERM_HANDUP | PERM_SAY);
  cu->uflag = level;
  strcpy(cu->userid, userid);
  cu->userid[IDLEN]='\0';
  strcpy(cu->chatid, chatid);
  cu->chatid[IDLEN]='\0';
//  memcpy(cu->chatid, chatid, 8);
//  cu->chatid[8] = '\0';

//  sprintf(buf, "%d\0", cu->uflag);
//  logit("level", buf);
//  sprintf(buf, "%d\0", ROOMOP(cu));
//  logit("roomop", buf);
//  sprintf(buf, "%s\0", board);
//  logit("roomname", buf);

  /* Xshadow: ���o client ���ӷ� */

  fromlen = sizeof(from);
  if (!getpeername(cu->sock, (struct sockaddr *) & from, &fromlen))
  {
    if ((hp = gethostbyaddr((char *) &from.sin_addr, sizeof(struct in_addr), from.sin_family)))
    {
      strcpy(cu->lasthost, hp->h_name);
    }
    else
      strcpy(cu->lasthost, (char *) inet_ntoa(from.sin_addr));

  }
  else
  {
    strcpy(cu->lasthost, "[�~�Ӫ�]");
  }

  if (cu->clitype)
    send_to_user(cu, "���Q", 0, MSG_LOGINOK);
  else
    send_to_user(cu, CHAT_LOGIN_OK, 0, 0);


  if(*board)
     arrive_room(cu, &mainroom, 0);
  else
     arrive_room(cu, &mainroom, 1);

  send_to_user(cu, "", 0, MSG_MOTDSTART);

  if(! *board)
     print_user_counts(cu);

  send_to_user(cu, "", 0, MSG_MOTDEND);

#ifdef  DEBUG
  logit("enter:", cu->userid, "OK");
#endif

  while(cu->color == 0 || cu->color == 4)
    cu->color = rand()%8;

  if(*board)
  {
    enter_room(cu, board, NULL, 0);
  }
  return 0;
}


static void
chat_act(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (*msg && (!RHANDUP(cu->room) || SAY(cu) || ROOMOP(cu)))
  {
    sprintf(chatbuf, "%s [36m%s[m", cu->chatid, msg);
    send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);
  }
}


static void
chat_ignore(cu, msg)
  ChatUser *cu;
  char *msg;
{

  if (RESTRICTED(cu))
  {
    strcpy(chatbuf, "�� �z�S�� ignore �O�H���v�Q");
  }
  else
  {
    char *ignoree;

    ignoree = nextword(&msg);
    if (*ignoree)
    {
      ChatUser *xuser;

      xuser = cuser_by_userid(ignoree);

      if (xuser == NULL)
      {

        sprintf(chatbuf, msg_no_such_id, ignoree);

#if 0
        sprintf(chatbuf, "�� �ͤѫǲ{�b�S�� [%s] �o���H��", ignoree);
#endif
      }
      else if (xuser == cu || CHATSYSOP(xuser) ||
        (ROOMOP(xuser) && (xuser->room == cu->room)))
      {
        sprintf(chatbuf, "�� ���i�H ignore [%s]", ignoree);
      }
      else
      {

        if (list_belong(cu->ignore, xuser->userno))
        {
          sprintf(chatbuf, "�� %s �w�g�Q�ᵲ�F", xuser->chatid);
        }
        else
        {
          list_add(&(cu->ignore), xuser);
          sprintf(chatbuf, "�� �N [%s] ���J�N�c�F :p", xuser->chatid);
        }
      }
    }
    else
    {
      UserList *list;

      if (list = cu->ignore)
      {
        int len;
        char buf[16];

        send_to_user(cu, "�� �o�ǤH�Q���J�N�c�F�G", 0, MSG_MESSAGE);
        len = 0;
        do
        {
          sprintf(buf, "%-13s", list->userid);
          strcpy(chatbuf + len, buf);
          len += 13;
          if (len >= 78)
          {
            send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
            len = 0;
          }
        } while (list = list->next);

        if (len == 0)
          return;
      }
      else
      {
        strcpy(chatbuf, "�� �z�ثe�èS�� ignore ����H");
      }
    }
  }

  send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_unignore(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *ignoree;

  ignoree = nextword(&msg);

  if (*ignoree)
  {
    sprintf(chatbuf, (list_delete(&(cu->ignore), ignoree)) ?
      "�� [%s] ���A�Q�A�N���F" :
      "�� �z�å� ignore [%s] �o���H��", ignoree);
  }
  else
  {
    strcpy(chatbuf, "�� �Ы��� user ID");
  }
  send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_join(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (RESTRICTED(cu))
  {
    send_to_user(cu, "�� �z�S���[�J��L��ѫǪ��v��", 0, MSG_MESSAGE);
  }
  else
  {
    char *roomid = nextword(&msg);

    if (*roomid)
      enter_room(cu, roomid, msg, 1);
    else
      send_to_user(cu, "�� �Ы��w��ѫǪ��W�r", 0, MSG_MESSAGE);
  }
}


static void
chat_kick(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *twit;
  ChatUser *xuser;
  ChatRoom *room;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0, MSG_MESSAGE);
    return;
  }

  twit = nextword(&msg);
  xuser = cuser_by_chatid(twit);

  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, twit);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  room = cu->room;
  if (room != xuser->room || CLOAK(xuser))
  {                             /* Thor: ��ѫ������N */
    sprintf(chatbuf, msg_not_here, twit);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  if (CHATSYSOP(xuser))
  {                             /* Thor: �𤣨� CHATSYSOP */
    sprintf(chatbuf, "�� ���i�H kick [%s]", twit);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  exit_room(xuser, EXIT_KICK, (char *) NULL, 1);

  logit("debug:", xuser->userid, "");

  if (room == &mainroom)
    logout_user(xuser);
  else
    enter_room(xuser, MAIN_NAME, (char *) NULL, 1);
}


static void
chat_makeop(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *newop;
  ChatUser *xuser;
  ChatRoom *room;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0, MSG_MESSAGE);
    return;
  }

  newop = nextword(&msg);
  xuser = cuser_by_chatid(newop);

  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, newop);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  if (cu == xuser)
  {
    sprintf(chatbuf, "�� �z���N�w�g�O Op �F��");
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  room = cu->room;

  if (room != xuser->room || CLOAK(xuser))
  {                             /* Thor: ��ѫ������N */
    sprintf(chatbuf, msg_not_here, xuser->chatid);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  cu->uflag &= ~PERM_ROOMOP;
  xuser->uflag |= PERM_ROOMOP;

  user_changed(cu);
  user_changed(xuser);

  sprintf(chatbuf, "�� %s �N Op �v�O�ಾ�� %s",
    cu->chatid, xuser->chatid);
  if (!CLOAK(cu))               /* Thor: ��ѫ������N */
    send_to_room(room, chatbuf, 0, MSG_MESSAGE, MSG_MESSAGE);
}



static void
chat_invite(cu, msg)
  ChatUser *cu;
  char *msg;
{
  char *invitee;
  ChatUser *xuser;
  ChatRoom *room;
  UserList **list;

  if (!ROOMOP(cu))
  {
    send_to_user(cu, msg_not_op, 0, MSG_MESSAGE);
    return;
  }

  invitee = nextword(&msg);
  xuser = cuser_by_chatid(invitee);
  if (xuser == NULL)
  {
    sprintf(chatbuf, msg_no_such_id, invitee);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }

  room = cu->room;              /* Thor: �O�_�n check room �O�_ NULL ? */
  list = &(room->invite);

  if (list_belong(*list, xuser->userno))
  {
    sprintf(chatbuf, "�� %s �w�g�����L�ܽФF", xuser->chatid);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
    return;
  }
  list_add(list, xuser);

  sprintf(chatbuf, "�� %s �ܽбz�� [%s] ��ѫ�",
    cu->chatid, room->name);
  send_to_user(xuser, chatbuf, 0, MSG_MESSAGE); /* Thor: �n���n�i�H ignore? */
  sprintf(chatbuf, "�� %s ����z���ܽФF", xuser->chatid);
  send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_broadcast(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (!CHATSYSOP(cu))
  {
    send_to_user(cu, "�� �z�S���b��ѫǼs�����v�O!", 0, MSG_MESSAGE);
    return;
  }
  if (*msg == '\0')
  {
    send_to_user(cu, "�� �Ы��w�s�����e", 0, MSG_MESSAGE);
    return;
  }
  sprintf(chatbuf, "[1m�� " BOARDNAME "�ͤѫǼs���� [%s].....[m",
    cu->chatid);
  send_to_room(ROOM_ALL, chatbuf, 0, MSG_MESSAGE);
  sprintf(chatbuf, "�� %s", msg);
  send_to_room(ROOM_ALL, chatbuf, 0, MSG_MESSAGE);
}


static void
chat_goodbye(cu, msg)
  ChatUser *cu;
  char *msg;
{
  exit_room(cu, EXIT_LOGOUT, msg, 1);
  /* Thor: �n���n�[ logout_user(cu) ? */
  logout_user(cu);
}


/* --------------------------------------------- */
/* MUD-like social commands : action             */
/* --------------------------------------------- */

struct ChatAction
{
  char *verb;                   /* �ʵ� */
  char *chinese;                /* ����½Ķ */
  char *part1_msg;              /* ���� */
  char *part2_msg;              /* �ʧ@ */
};


static ChatAction party_data[] =
{
  {
    "aluba", "���|��", "��", "�[�W�W�l���|��!!"
  },
  {
    "bearhug", "���", "�ΤO��", "�ꪺ�֭n�����F"
  },
  {
    "bearnod", "�ΤO�I�Y","���", "���R���I�Y�p�o�["
  },
  {
    "blade", "�@�M", "�@�M�ҵ{��", "�e�W���"
  },                            /* Thor.0729:�[���n�D */
  {
    "bless", "����", "�`�`������", "�@�����Q"
  },
  {
    "board", "�D���O", "��", "��h���D���O"
  },                            /* Thor.0730: �[���n�D */
  {
    "bow", "���`", "���`���q����", "���`"
  },
  {
    "boy", "������", "�q�I�᮳�X�F������A��", "�V���F"
  },
  {
    "bye", "�T�T", "�V", "���T�T!!"
  },
  {
    "call", "�I��", "�j�n���I��,��~~", "��~~~�A�b���̰ڰڰڰ�~~~~"
  },
  {
    "caress", "����", "���������N��", ""
  },
  {
    "catleg", "�߻L", "�j�ۡG�u�U�઺", "�н�P�گ��_���O�q�v"
  },
  {
    "clap", "���x", "��", "�ΤO�����x����o�~"
  },
  {
    "claw", "���", "�򳥿߭ɨӿߤ��A��", "���y�W��X�@�D�@�D���嵷"
  },
  {
    "comfort", "�w��", "�Ũ��w��", "���G�u�����A�����A�ڷ|�t�d���C�v"
  },
  {
    "cong", "����", "�q�I�᮳�X�F�Ԭ��A��I��I����", ""
  },
  {
    "cpr", "�f��f", "���", "���f��f�H�u�I�l"
  },
  {
    "cringe", "�^��", "�V", "���`�}���A�n���^��"
  },
  {
    "cry", "�j��", "���˦b", "���h��"
  },
  {
    "dance", "���R", "�Ե�", "����ּ֪����_�R��"
  },
  {
    "destroy", "����", "���_�F�y���j�����G��z�A�F�V", ""
  },
  {
    "dogleg", "���L", "��", "�������A���p�ʷʦ����A�ä���K"
  },
  {
    "drivel", "�y�f��", "�ݵ�", "�éê��o�b�A�f�����w�U�ӤF"
  },
  {
    "envy", "�r�}", "���", "�y�S�X�r�}������"
  },
  {
    "eye", "�e��i", "��", "�W�e��i"
  },
  {
    "fire", "�R��", "���ۤ������K�Ψ��V", ""
  },
  {
    "forgive", "���", "��", "���G�u�O�Ӧb�N�աA�p�Ƥ@�Ρv"
  },
  {
    "french", "�k���k", "�M", "�����Y����b�@�_�������˧k��"
  },
  {
    "gag", "�ʼL", "�ν��a��", "���L�ڶK�_�Ӥ����L����"
  },
  {
    "giggle", "�Y��", "���", "�Y�Y�������F�X�n"
  },
  {
    "glue", "�ɤ�", "�ΤT�����A��", "�����H�F�_��"
  },                            /* Thor.0731:���[���n�D */
  {
    "goodbye", "�i�O", "�\\���L�L���V", "�i�O"
  },
  {
    "grin", "�l��", "��", "�S�X���c�����e"
  },
  {
    "growl", "�H��", "��", "���㪺�H�����w"
  },
  {
    "hand", "����", "�M", "�ΤO������"
  },
  {
    "hide", "��", "�`�Ȫ����b", "�I��"
  },
  {
    "hospitl", "�e��|", "��", "�w�g�a�a�@���A���֧�L�e�h��|"
  },
  {
    "hug", "�֩�", "�ŬX���N", "�֤J�h��"
  },
  {
    "jab", "�W�H", "���_�B�p�A��", "�����̨�F�U�h"
  },
  {
    "judo", "�L�ӺL", "����F", "�����̡A���L�@�O�L�ӺL�I"
  },
  {
    "kickout", "��X�h", "��", "��^�D��歱����L"
  },
  {
    "kick", "��H", "�@�}��", "��i�H���e��"
  },
  {
    "kiss", "���k", "��", "�@�ӷŬX���k"
  },
  {
    "laugh", "�J��", "�j�n���J��", "�G�u�u�O�§o�v"
  },
  {
    "levis", "����", "���G����", "�I��l�K�͡I"
  },
  {
    "lick", "�Q", "�Q��", "�������O�f��"
  },
  {
    "lorelie", "����", "�I��X�����A�κq�n��", "�~���F�I"
  },
  {
    "love", "����", "�`������", "���G�u�ڲĤ@���J��A�A�N���i�۩ު��R�W�A�F....�v"
  },
  {
    "marry", "�D�B", "���ۤE�ʤE�Q�E�������V", "�D�B"
  },
  {
    "monica", "�p��", "�ܨ����l��k�A��", "����l�r�F�U�h"
  },
  {
    "no", "���n��", "���R���", "�n�Y~~~~���n��~~~~"
  },
  {
    "nod", "�I�Y", "���", "�������I�F�@�U�Y"
  },
  {
    "nudge", "���{�l", "�Τ�y��", "����s�{���U�h"
  },
  {
    "pad", "��ӻH", "����", "���ӻH�A���G�u�n�p�l�I�F���n�I�v"
  },
  {
    "pettish", "���b", "��", "���n�ݮ�a���b"
  },
  {
    "pili", "�R�E", "�ϥX �g�l�� �Ѧa�� ��Y�b �T���X�@���V", "~~~~~~"
  },
  /* Thor.0729: ���[���n�D */
  {
    "pinch", "���H", "�ΤO����", "�����������«C"
  },
  {
    "roll", "���u", "��X�h���O������,", "�b�a�W�u�Ӻu�h"
  },
  {
    "protect", "�O�@", "��", "���G�u�S���Y�A�ڷ|�O�@�A���v"
  },
  {
    "pull", "��", "���R�a�Ԧ�", "����"
  },
  {
    "punch", "�~�H", "�����~�F", "�@�y"
  },
  {
    "rascal", "�A��", "��", "�A��"
  },
  {
    "recline", "�J�h", "�p��", "���h�̺εۤF�K�K"
  },
  {
    "saw", "�q��", "���X�q���A��", "�����̿��F�U�h"
  },
  {
    "shrug", "�q��", "�L�`�a�V", "�q�F�q�ӻH"
  },
  {
    "sigh", "�ۮ�", "���", "�`�`���ۤF�@�f��"
  },
  {
    "slap", "���ե�", "�԰Ԫ��ڤF", "�@�y�ե�"
  },
  {
    "smooch", "�֧k", "�M", "��򪺩�b�@�_���k"
  },
  {
    "snicker", "�ѯ�", "�K�K�K..����", "�ѯ�"
  },
  {
    "sniff", "���h", "��", "�ᤧ�H��"
  },

  /* Thor.0729: �����Ѥ�-----�[���ץ��� */
  {
    "sob", "�F�y", "��", "�������� You S...un Of B...each�I"
  },

  {
    "spank", "������", "�Τڴx��", "���v��"
  },
  /* {"spank", "�οߤ���", "���v��","���v��"}, */
  {
    "thank", "�P��", "�ѰJ����", "���ܷP�¤��N"
  },
  {
    "tickle", "�k�o", "�B�T!�B�T!�k", "���ުϺ�"
  },
  {
    "wake", "�n��", "���_�@���N���A��", "���Y�W��F�U�h"
  },
  {
    "wave", "����", "���", "���R���⻡�A��"
  },
  {
    "welcome", "�w��", "�w��", "�i�ӤK���@�f"
  },
  {
    "what", "����", "��", "���G�u��p�A�ڬ�M���h�O�ФF�I�v"
  },
  {
    "whip", "�@�l", "��W��������A���@�l�h��", ""
  },
  {
    "wink", "�w��", "��", "�������w�w����"
  },
  {
    "zap", "�r��", "���X���b���A��", "�F�F�L�h"
  },

  {
    NULL, NULL, NULL, NULL
  }
};


static int
party_action(cu, cmd, party)
  ChatUser *cu;
  char *cmd;
  char *party;
{
  ChatAction *cap;
  char *verb;

  for (cap = party_data; verb = cap->verb; cap++)
  {
    MYDOG;

    if (str_equal(cmd, verb))
    {
      if (*party == '\0')
      {
        party = "�j�a";
      }
      else
      {
        ChatUser *xuser;

        xuser = fuzzy_cuser_by_chatid(party);
        if (xuser == NULL)
        {                       /* Thor.0724: �� userid�]���q */
          xuser = cuser_by_userid(party);
        }

        if (xuser == NULL)
        {
          sprintf(chatbuf, msg_no_such_id, party);
          send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
          return 0;
        }
        else if (xuser == FUZZY_USER)
        {
          sprintf(chatbuf, "�� �Ы�����ѥN��");
          send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
          return 0;
        }
        else if (cu->room != xuser->room || CLOAK(xuser))
        {
          sprintf(chatbuf, msg_not_here, party);
          send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
          return 0;
        }
        else
        {
          party = xuser->chatid;
        }
      }
      sprintf(chatbuf, "[1;32m%s [31m%s[33m %s [31m%s[m",
        cu->chatid, cap->part1_msg, party, cap->part2_msg);
      send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);
      return 0;                 /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 1;
}


/* --------------------------------------------- */
/* MUD-like social commands : speak              */
/* --------------------------------------------- */


static ChatAction speak_data[] =
{

  {
    "ask", "�߰�", "�ݻ�", NULL
  },
  {
    "chant", "�q�|", "���n�q�|", NULL
  },
  {
    "cheer", "�ܪ�", "�ܪ�", NULL
  },
  {
    "chuckle", "����", "����", NULL
  },
  {
    "curse", "�t�F", "�t�F", NULL
  },
  /* {"curse", "�G�|", NULL}, */
  {
    "demand", "�n�D", "�n�D", NULL
  },
  {
    "frown", "�K���Y", "�٬�", NULL
  },
  {
    "groan", "�D�u", "�D�u", NULL
  },
  {
    "grumble", "�o�c��", "�o�c��", NULL
  },
  {
    "guitar", "�u��", "��u�ۦN�L�A��۵�", NULL
  },
  {
    "hum", "���", "���ۻy", NULL
  },
  {
    "moan", "���", "���", NULL
  },
  {
    "notice", "�j��", "�j��", NULL
  },
  {
    "order", "�R�O", "�R�O", NULL
  },
  {
    "ponder", "�H��", "�H��", NULL
  },
  {
    "pout", "���L", "���ۼL��", NULL
  },
  {
    "pray", "��ë", "��ë", NULL
  },
  {
    "request", "���D", "���D", NULL
  },
  {
    "shout", "�j�|", "�j�|", NULL
  },
  {
    "sing", "�ۺq", "�ۺq", NULL
  },
  {
    "smile", "�L��", "�L��", NULL
  },
  {
    "smirk", "����", "����", NULL
  },
  {
    "swear", "�o�}", "�o�}", NULL
  },
  {
    "tease", "�J��", "�J��", NULL
  },
  {
    "whimper", "��|", "��|����", NULL
  },
  {
    "yawn", "����", "�䥴�����仡", NULL
  },
  {
    "yell", "�j��", "�j��", NULL
  },
  {
    NULL, NULL, NULL, NULL
  }
};


static int
speak_action(cu, cmd, msg)
  ChatUser *cu;
  char *cmd;
  char *msg;
{
  ChatAction *cap;
  char *verb;

  for (cap = speak_data; verb = cap->verb; cap++)
  {
    MYDOG;

    if (str_equal(cmd, verb))
    {
      sprintf(chatbuf, "[1;32m%s [31m%s�G[33m %s[m",
        cu->chatid, cap->part1_msg, msg);
      send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);
      return 0;                 /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 1;
}


/* -------------------------------------------- */
/* MUD-like social commands : condition          */
/* -------------------------------------------- */


static ChatAction condition_data[] =
{
  {
    "applaud", "���", "�ΤO���A���x�s�n", NULL
  },
  {
    "ayo", "�����", "�G�s�G�u�����~~~�v", NULL
  },
  {
    "back", "�^��", "�ŻŪ����G�uI'm back�I�v", NULL
  },
  {
    "blood", "�b�夤", "�G�s�@�n�A�˦b��y����", NULL
  },
  {
    "blush", "�y��", "���n�N�䪺�y�����F", NULL
  },
  {
    "broke", "�߸H", "���߯}�H���@���@����", NULL
  },                            /* Thor.0731:���[���n�D */
  {
    "careles", "�S�H�z", "��_�G���㳣�S���H�z�� :~~~~", NULL
  },
  {
    "chew", "�ߥʤl", "�ܱy�����߰_�ʤl�ӤF", NULL
  },
  {
    "climb", "���s", "�ۤv�C�C���W�s�ӡK�K", NULL
  },
  {
    "cold", "�P�_�F", "�P�_�F,���������ڥX�h�� :~~~(", NULL
  },
  {
    "cough", "�y��", "�y�F�X�n�A�n���b�t�ܤ���", NULL
  },
  {
    "die", "����", "�˦b�a�W�A��������", NULL
  },
  {
    "faint", "����", "ť��o�Ӯ����A��������", NULL
  },
  {
    "flop", "�^��", "�P�q�@�n�A��M�^�ˡI", NULL
  },
  {
    "fly", "�a��", "�j�ۡG�uI'm flying�I�v", NULL
  },
  {
    "frown", "�٬�", "���Y������]�l�@��", NULL
  },
  {
    "gold", "�����P", "�۵ۡG�y���|�������|����  �X�����! �o�a�x�A�����P�A���a�˾H�ӡI�z", NULL
  },
  {
    "gulu", "�{�l�j", "���{�l�o�X�B�P~~~�B�P~~~���n��", NULL
  },
  {
    "haha", "�z����", "�g���G�z������.....^o^", NULL
  },
  {
    "helpme", "�D��", "�j��~~~�ϩR��~~~~", NULL
  },
  {
    "hoho", "������", "�o�X ������������ �����n", NULL
  },
  {
    "happy", "����", "�����o�b�a�W�u�Ӻu�h", NULL
  },
  /* {"happy", "����", "��ϡI *^_^*", NULL}, */
  /* {"happy", "", "r-o-O-m....ť�F�u�n�I", NULL}, */
  /* {"hurricane", "�֢�---��B��--�٢���I�I�I", NULL}, */
  {
    "idle", "�o�b", "���b��a�o�b", NULL
  },
  {
    "jacky", "�̮�", "�l�l�몺�̨Ӯ̥h", NULL
  },
  {
    "lag", "�����C", "�j�ۡG�u�����t�t�� LowNet �ڡI�I�I�v", NULL
  },
  {
    "luck", "���B", "�j�ۡG�u�ڡI�֮�աI�v", NULL
  },
  {
    "macarn", "�@�ػR", "�}�l���_�F��a��a��e��a�����", NULL
  },
  {
    "miou", "�p�p", "�}�l�g�s�G�p�p�f�]�f�]������", NULL
  },
  {
    "mouth", "��L", "�L�ڹʪ��i�H�����פF", NULL
  },
  {
    "nani", "���|", "�G�s�G�`���ڮ�??", NULL
  },
  {
    "nose", "�y���", "����M�Q�X�ӤF....", NULL
  },
  {
    "puke", "�æR", "�M�ռM�ժ��R�F���a���O�C", NULL
  },
  {
    "rest", "��", "�𮧤��A�Фť��Z", NULL
  },
  {
    "reverse", "½�{", "½�ը{�F", NULL
  },
  {
    "room", "�}�ж�", "�D�u�Gr-o-O-m-r-O-��-Mmm-rR��........", NULL
  },
  {
    "shake", "�n�Y", "�n�F�n�Y", NULL
  },
  {
    "sleep", "�ε�", "�w�b��L�W�εۤF�A�f���y�i�h�A�̭��������~�o���ַh�a", NULL
  },
  {
    "so", "�N��l", "���G�u���N����a!!�v", NULL
  },
  {
    "sorry", "�D�p", "�I�q�G���!!�ڹ藍�_�j�a,�ڹ藍�_��a���|~~~~~~���~~~~~", NULL
  },
  {
    "story", "���j", "�}�l���j�F", NULL
  },
  {
    "strut", "�n�\\��", "�䧾���j�n�j�\\���Ө��h", NULL
  },
  {
    "suicide", "�۱�", "���X�Z�h�M����", NULL
  },
  {
    "tea", "�w��", "�w�F���n���A�~�򫢩ԤU�h", NULL
  },
  {
    "think", "���", "�n���Y�Q�F�@�U", NULL
  },
  {
    "tongue", "�R��", "�R�F�R���Y", NULL
  },
  {
    "wall", "����", "�]�h����", NULL
  },
  {
    "wawa", "�z�z", "���n�j���G�z�z�z~~~~~!!!!!  ~~~>_<~~~", NULL
  },
  {
    "www", "�L�L", "�L�L�L....�s�Ӥ���", NULL
  },
  {
    "ya", "�ӧQ", "��F�@�ӳӧQ��աG�uYa!�v", NULL
  },
  {
    "zzz", "���I", "�I�P~~~~ZZzZz�C��ZZzzZzzzZZ...", NULL
  },

  {
    NULL, NULL, NULL, NULL
  }
};


static int
condition_action(cu, cmd)
  ChatUser *cu;
  char *cmd;
{
  ChatAction *cap;
  char *verb;

  for (cap = condition_data; verb = cap->verb; cap++)
  {
    MYDOG;

    if (str_equal(cmd, verb))
    {
      sprintf(chatbuf, "[1;32m%s [31m%s[m",
        cu->chatid, cap->part1_msg);
      send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);
      return 1;                 /* Thor: cu->room �O�_�� NULL? */
    }
  }
  return 0;
}


/* --------------------------------------------- */
/* MUD-like social commands : help               */
/* --------------------------------------------- */


static char *dscrb[] =
{
  "[1;37m�i Verb + Nick�G   �ʵ� + ���W�r �j[36m   �ҡG//kick piggy[m",
  "[1;37m�i Verb + Message�G�ʵ� + �n������ �j[36m   �ҡG//sing �ѤѤ���[m",
  "[1;37m�i Verb�G�ʵ� �j    �����G�¸ܭ���[m", NULL
};
ChatAction *catbl[] =
{
  party_data, speak_data, condition_data, NULL
};

static void
chat_partyinfo(cu, msg)
  ChatUser *cu;
  char *msg;
{
  if (!common_client_command)
    return;                     /* only allow common client to retrieve it */

  sprintf(chatbuf, "3 �ʧ@  ���  ���A");
  send_to_user(cu, chatbuf, 0, MSG_PARTYINFO);
}

static void
chat_party(cu, msg)
  ChatUser *cu;
  char *msg;
{
  int kind, i;
  ChatAction *cap;

  if (!common_client_command)
    return;

  kind = atoi(nextword(&msg));
  if (kind < 0 || kind > 2)
    return;

  sprintf(chatbuf, "%d  %s", kind, kind == 2 ? "I" : "");

  /* Xshadow: �u�� condition �~�O immediate mode */
  send_to_user(cu, chatbuf, 0, MSG_PARTYLISTSTART);

  cap = catbl[kind];
  for (i = 0; cap[i].verb; i++)
  {
    sprintf(chatbuf, "%-10s %-20s", cap[i].verb, cap[i].chinese);
    /* for (j=0;j<1000000;j++); */
    send_to_user(cu, chatbuf, 0, MSG_PARTYLIST);
  }

  sprintf(chatbuf, "%d", kind);
  send_to_user(cu, chatbuf, 0, MSG_PARTYLISTEND);
}


#define SCREEN_WIDTH    80
#define MAX_VERB_LEN    8
#define VERB_NO         10

static void
view_action_verb(cu, cmd)       /* Thor.0726: �s�[�ʵ�������� */
  register ChatUser *cu;
  char cmd;
{
  register int i;
  register char *p, *q, *data, *expn;
  register ChatAction *cap;

  send_to_user(cu, "/c", 0, MSG_CLRSCR);

  data = chatbuf;

  if (cmd < '1' || cmd > '3')
  {                             /* Thor.0726: �g�o���n, �Q��k��i... */
    for (i = 0; p = dscrb[i]; i++)
    {
      sprintf(data, "  [//]help %d          - MUD-like ����ʵ�   �� %d ��", i + 1, i + 1);
      MYDOG;
      send_to_user(cu, data, 0, MSG_MESSAGE);
      send_to_user(cu, p, 0, MSG_MESSAGE);
      send_to_user(cu, " ", 0, MSG_MESSAGE);    /* Thor.0726: ����, �ݭn " "
                                                 * ��? */
    }
  }
  else
  {
    i = cmd - '1';

    send_to_user(cu, dscrb[i], 0, MSG_MESSAGE);

    expn = chatbuf + 100;       /* Thor.0726: ���Ӥ��|overlap�a? */

    *data = '\0';
    *expn = '\0';

    cap = catbl[i];

    for (i = 0; p = cap[i].verb; i++)
    {
      MYDOG;
      q = cap[i].chinese;

      strcat(data, p);
      strcat(expn, q);

      if (((i + 1) % VERB_NO) == 0)
      {
        send_to_user(cu, data, 0, MSG_MESSAGE);
        send_to_user(cu, expn, 0, MSG_MESSAGE); /* Thor.0726: ��ܤ������ */
        *data = '\0';
        *expn = '\0';
      }
      else
      {
        strncat(data, "        ", MAX_VERB_LEN - strlen(p));
        strncat(expn, "        ", MAX_VERB_LEN - strlen(q));
      }
    }
    if (i % VERB_NO)
    {
      send_to_user(cu, data, 0, MSG_MESSAGE);
      send_to_user(cu, expn, 0, MSG_MESSAGE);   /* Thor.0726: ��ܤ������ */
    }
  }
  /* send_to_user(cu, " ",0); *//* Thor.0726: ����, �ݭn " " ��? */
}

#if 0
static void
view_action_verb(cu, cmd)
  register ChatUser *cu;
  char cmd;
{
  register int i, j;
  register char *p, *data;
  register ChatAction *cap;

  send_to_user(cu, "", 0, MSG_CLRSCR);

  data = chatbuf;

  for (i = 0; p = dscrb[i]; i++)
  {
    MYDOG;

    send_to_user(cu, p, 0, MSG_MESSAGE);
    *data = '\0';
    j = 0;
    cap = catbl[i];
    while (p = cap[j++].verb)
    {
      MYDOG;

      strcat(data, p);
      if ((j % VERB_NO) == 0)
      {
        send_to_user(cu, data, 0, MSG_MESSAGE);
        *data = '\0';
      }
      else
      {
        strncat(data, "        ", MAX_VERB_LEN - strlen(p));
      }
    }
    if (j % VERB_NO)
      send_to_user(cu, data, 0);
    send_to_user(cu, " ", 0);
  }
}
#endif


/* ----------------------------------------------------- */
/* chat user service routines                            */
/* ----------------------------------------------------- */


static ChatCmd chatcmdlist[] =
{
  "/", chat_help, 0,
  "act", chat_act, 0,
  "bye", chat_goodbye, 0,
  "chatroom", chat_chatroom, 1, /* Xshadow: for common client */
  "clear", chat_clear, 0,
  "cloak", chat_cloak, 2,
  "color", chat_color, 0,
  "date", chat_date, 0,
  "flags", chat_setroom, 0,
  "help", chat_help, 0,
  "ignore", chat_ignore, 1,
  "invite", chat_invite, 0,
  "join", chat_join, 0,
  "kick", chat_kick, 1,
  "msg", chat_private, 0,
  "nick", chat_nick, 0,
  "operator", chat_makeop, 0,
  "party", chat_party, 1,       /* Xshadow: party data for common client */
  "partyinfo", chat_partyinfo, 1,       /* Xshadow: party info for common
                                         * client */

#ifndef STAND_ALONE
  "query", chat_query, 0,
#endif

  "room", chat_list_rooms, 0,
  "unignore", chat_unignore, 1,
  "whoin", chat_list_by_room, 1,
  "wall", chat_broadcast, 2,

  "who", chat_map_chatids_thisroom, 0,
  "list", chat_list_users, 0,
  "topic", chat_topic, 1,
  "version", chat_version, 1,

  NULL, NULL, 0
};

/* Thor: 0 ���� exact, 1 �n exactly equal, 2 ���K���O */


static int
command_execute(cu)
  ChatUser *cu;
{
  char *cmd, *msg;
  ChatCmd *cmdrec;
  int match, ch;

  msg = cu->ibuf;
  match = *msg;

  /* Validation routine */

  if (cu->room == NULL)
  {
    /* MUST give special /! or /-! command if not in the room yet */

    if (match == '/' && ((ch = msg[1]) == '!' || (ch == '-' && msg[2] == '!')))
    {
      cu->clitype = (ch == '-') ? 1 : 0;
      return (login_user(cu, msg + 2 + cu->clitype));
    }
    else
      return -1;
  }

  /* If not a /-command, it goes to the room. */

  if (match != '/')
  {
    if (match)
    {
      char buf[16];

      sprintf(buf, "%s:", cu->chatid);
      sprintf(chatbuf, "[1;3%dm%-10s%s[m", cu->color,buf, msg);

                                 /* Thor: ��ѫ������N */
//      if (!CLOAK(cu))
        send_to_room(cu->room, chatbuf, cu->userno, MSG_MESSAGE);

    }
    return 0;
  }

  msg++;
  cmd = nextword(&msg);
  match = 0;

  if (*cmd == '/')
  {
    cmd++;
    if (!*cmd || str_equal(cmd, "help"))
    {
      /* Thor.0726: �ʵ����� */
      cmd = nextword(&msg);
      view_action_verb(cu, *cmd);
      match = 1;
    }
    else if (party_action(cu, cmd, msg) == 0)
      match = 1;
    else if (speak_action(cu, cmd, msg) == 0)
      match = 1;
    else
      match = condition_action(cu, cmd);
  }
  else
  {
    char *str;

    common_client_command = 0;
    if (*cmd == '-')
      if (cu->clitype)
      {
        cmd++;                  /* Xshadow: ���O�q�U�@�Ӧr���~�}�l */

        common_client_command = 1;
      }
      else;                     /* ���O common client ���e�X common client
                                 * ���O -> ���˨S�ݨ� */

    for (cmdrec = chatcmdlist; str = cmdrec->cmdstr; cmdrec++)
    {
      MYDOG;

      switch (cmdrec->exact)
      {
      case 1:                   /* exactly equal */
        match = str_equal(cmd, str);
        break;
      case 2:                   /* Thor: secret command */
        if (CHATSYSOP(cu))
          match = str_equal(cmd, str);
        break;
      default:                  /* not necessary equal */
        match = str_match(cmd, str) >= 0;
        break;
      }

      if (match)
      {
        cmdrec->cmdfunc(cu, msg);
        break;
      }
    }
  }

  if (!match)
  {
    sprintf(chatbuf, "�� ���O���~�G/%s", cmd);
    send_to_user(cu, chatbuf, 0, MSG_MESSAGE);
  }
  return 0;
}


/* ----------------------------------------------------- */
/* serve chat_user's connection                          */
/* ----------------------------------------------------- */


static int
cuser_serve(cu)
  ChatUser *cu;
{
  register int ch, len, isize;
  register char *str, *cmd;
  static char buf[80];
//  char inbuf[64];

  str = buf;
  len = recv(cu->sock, str, sizeof(buf) - 1, 0);
  if (len <= 0)
  {
    /* disconnected */

    exit_room(cu, EXIT_LOSTCONN, (char *) NULL, 1);
    return -1;
  }

//  sprintf(inbuf, "%.63s (%s) \0", buf, cu->userid);
//  logit("recv:", inbuf, "");

  isize = cu->isize;
  cmd = cu->ibuf + isize;
  while (len--)
  {
    MYDOG;

    ch = *str++;

    if (ch == '\r' || !ch)
      continue;
    if (ch == '\n')
    {
      *cmd = '\0';

      isize = 0;
      cmd = cu->ibuf;

      if (command_execute(cu) < 0)
        return -1;

      continue;
    }
    if (isize < 79)
    {
      *cmd++ = ch;
      isize++;
    }
  }
  cu->isize = isize;
  return 0;
}


/* ----------------------------------------------------- */
/* chatroom server core routines                         */
/* ----------------------------------------------------- */

static int
start_daemon()
{
  int fd, value;
  char buf[80];
  struct sockaddr_in fsin;
  struct linger ld;
  struct rlimit limit;
  time_t dummy;
  struct tm *dummy_time;

  /*
   * More idiot speed-hacking --- the first time conversion makes the C
   * library open the files containing the locale definition and time zone.
   * If this hasn't happened in the parent process, it happens in the
   * children, once per connection --- and it does add up.
   */

  time(&dummy);
  dummy_time = gmtime(&dummy);
  dummy_time = localtime(&dummy);
  strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

  /* --------------------------------------------------- */
  /* speed-hacking DNS resolve                           */
  /* --------------------------------------------------- */

  gethostname(buf, sizeof(buf));

  /* Thor: �U�@server�|������connection, �N�^�h����, client �Ĥ@���|�i�J���� */
  /* �ҥH���� listen �� */

  /* --------------------------------------------------- */
  /* detach daemon process                               */
  /* --------------------------------------------------- */

  close(0);
  close(1);
  close(2);

  if (fork())
    exit(0);

  chdir(BBSHOME);

  setsid();

  /* --------------------------------------------------- */
  /* adjust the resource limit                           */
  /* --------------------------------------------------- */

  getrlimit(RLIMIT_NOFILE, &limit);
  limit.rlim_cur = limit.rlim_max;
  setrlimit(RLIMIT_NOFILE, &limit);

#if 0
  while (fd)
  {
    close(--fd);
  }

  value = getpid();
  setpgrp(0, value);

  if ((fd = open("/dev/tty", O_RDWR)) >= 0)
  {
    ioctl(fd, TIOCNOTTY, 0);    /* Thor : �������٭n��  tty? */
    close(fd);
  }
#endif

  fd = open(CHAT_PIDFILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd >= 0)
  {
    /* sprintf(buf, "%5d\n", value); */
    sprintf(buf, "%5d\n", getpid());
    write(fd, buf, 6);
    close(fd);
  }

#if 0
  /* ------------------------------ */
  /* trap signals                   */
  /* ------------------------------ */

  for (fd = 1; fd < NSIG; fd++)
  {

    signal(fd, SIG_IGN);
  }
#endif

  fd = socket(PF_INET, SOCK_STREAM, 0);

#if 0
  value = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, value | O_NDELAY);
#endif

  value = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &value, sizeof(value));

#if 0
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &value, sizeof(value));

  value = 81920;
  setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &value, sizeof(value));
#endif

  ld.l_onoff = ld.l_linger = 0;
  setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));

  memset((char *) &fsin, 0, sizeof(fsin));
  fsin.sin_family = AF_INET;
  fsin.sin_port = htons(CHATPORT);
  fsin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *) & fsin, sizeof(fsin)) < 0)
    exit(1);

  listen(fd, SOCK_QLEN);

  return fd;
}


static void
free_resource(fd)
  int fd;
{
  static int loop = 0;
  register ChatUser *user;
  register int sock, num;

  num = 0;
  for (user = mainuser; user; user = user->unext)
  {
    MYDOG;

    num++;
    sock = user->sock;
    if (fd < sock)
      fd = sock;
  }

  sprintf(chatbuf, "%d, %d user (%d -> %d)", ++loop, num, maxfds, fd);
//  logit("LOOP", chatbuf, "");

  maxfds = fd + 1;
}


#ifdef  SERVER_USAGE
static void
server_usage()
{
  struct rusage ru;
  char buf[2048];

  if (getrusage(RUSAGE_SELF, &ru))
    return;

  sprintf(buf, "\n[Server Usage]\n\n"
    "user time: %.6f\n"
    "system time: %.6f\n"
    "maximum resident set size: %lu P\n"
    "integral resident set size: %lu\n"
    "page faults not requiring physical I/O: %d\n"
    "page faults requiring physical I/O: %d\n"
    "swaps: %d\n"
    "block input operations: %d\n"
    "block output operations: %d\n"
    "messages sent: %d\n"
    "messages received: %d\n"
    "signals received: %d\n"
    "voluntary context switches: %d\n"
    "involuntary context switches: %d\n"
    "gline: %d\n\n",

    (double) ru.ru_utime.tv_sec + (double) ru.ru_utime.tv_usec / 1000000.0,
    (double) ru.ru_stime.tv_sec + (double) ru.ru_stime.tv_usec / 1000000.0,
    ru.ru_maxrss,
    ru.ru_idrss,
    ru.ru_minflt,
    ru.ru_majflt,
    ru.ru_nswap,
    ru.ru_inblock,
    ru.ru_oublock,
    ru.ru_msgsnd,
    ru.ru_msgrcv,
    ru.ru_nsignals,
    ru.ru_nvcsw,
    ru.ru_nivcsw,
    gline);

  write(flog, buf, strlen(buf));
}
#endif


static void
abort_server()
{
  log_close();
  exit(1);
}


static void
reaper()
{
  int state;

  while (waitpid(-1, &state, WNOHANG | WUNTRACED) > 0)
  {
    MYDOG;
  }
}


int
main()
{
  register int msock, csock, nfds;
  register ChatUser *cu;
  register fd_set *rptr, *xptr;
  fd_set rset, xset;
  struct timeval tv;
  time_t uptime, tmaintain;

  msock = start_daemon();

  setgid(BBSGID);
  setuid(BBSUID);

  log_init();

  signal(SIGBUS, SIG_IGN);
  signal(SIGSEGV, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGURG, SIG_IGN);

  signal(SIGCHLD, reaper);
  signal(SIGTERM, abort_server);

#ifdef  SERVER_USAGE
  signal(SIGPROF, server_usage);
#endif

  /* ----------------------------- */
  /* init variable : rooms & users */
  /* ----------------------------- */

  mainuser = NULL;
  memset(&mainroom, 0, sizeof(mainroom));
  strcpy(mainroom.name, MAIN_NAME);
  strcpy(mainroom.topic, MAIN_TOPIC);

  /* ----------------------------------- */
  /* main loop                           */
  /* ----------------------------------- */

#if 0
  /* Thor: �blisten ��~�^client, �C���i�ӴN�|���\ */
  if (fork())
    exit(0);
#endif

  FD_ZERO(&mainfds);
  FD_SET(msock, &mainfds);
  rptr = &rset;
  xptr = &xset;
  maxfds = msock + 1;

  tmaintain = time(0) + CHAT_INTERVAL;

  for (;;)
  {
    uptime = time(0);
    if (tmaintain < uptime)
    {
      tmaintain = uptime + CHAT_INTERVAL;

      /* client/server �����Q�� ping-pong ��k�P�_ user �O���O�٬��� */
      /* �p�G client �w�g�����F�A�N����� resource */

      free_resource(msock);
    }

    MYDOG;

    memcpy(rptr, &mainfds, sizeof(fd_set));
    memcpy(xptr, &mainfds, sizeof(fd_set));

    /* Thor: for future reservation bug */

    tv.tv_sec = CHAT_INTERVAL;
    tv.tv_usec = 0;

    MYDOG;

    nfds = select(maxfds, rptr, NULL, xptr, &tv);

    MYDOG;
    /* free idle user & chatroom's resource when no traffic */

    if (nfds == 0)
    {
      continue;
    }

    /* check error condition */

    if (nfds < 0)
    {
      csock = errno;
      if (csock != EINTR)
        logit("select", sys_errlist[csock], "");
      continue;
    }

    /* accept new connection */

    if (FD_ISSET(msock, rptr))
    {
      for (;;)
      {
        MYDOG;                          /* Thor: check for endless */
        csock = accept(msock, NULL, NULL);

        if (csock >= 0)
        {
          MYDOG;
          if (cu = (ChatUser *) malloc(sizeof(ChatUser)))
          {
            memset(cu, 0, sizeof(ChatUser));
            cu->sock = csock;

            cu->unext = mainuser;
            mainuser = cu;

#if 0
            if (mainuser.next)
              mainuser.next->prev = cu;
            cu->next = mainuser.next;
            mainuser.next = cu;
            cu->prev = &mainuser;
#endif

            totaluser++;
            FD_SET(csock, &mainfds);
            if (csock >= maxfds)
              maxfds = csock + 1;

#ifdef  DEBUG
//            logit("accept:", "OK", "1");
#endif
          }
          else
          {
            close(csock);
            logit("accept:", "malloc fail", "2");
          }
          MYDOG;

          break;
        }

        csock = errno;
        if (csock != EINTR)
        {
          logit("accept", sys_errlist[csock], "3");
          break;
        }
      }

      FD_CLR(msock, rptr);

      if (--nfds <= 0)
        continue;
    }

    for (cu = mainuser; cu; cu = cu->unext)
    {
      MYDOG;

      csock = cu->sock;

      if (FD_ISSET(csock, xptr))
      {
        logout_user(cu);
        FD_CLR(csock, xptr);
      }
      else if (FD_ISSET(csock, rptr))
      {
        if (cuser_serve(cu) < 0)
          logout_user(cu);
      }
      else
      {
        continue;
      }

      FD_CLR(csock, rptr);
      if (--nfds <= 0)
        break;
    }

    /* end of main loop */
  }


}