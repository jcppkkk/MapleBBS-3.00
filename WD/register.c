/*-------------------------------------------------------*/
/* register.c   ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user register routines                       */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"


/* ------------------- */
/* password encryption */
/* ------------------- */

char *crypt();
static char pwbuf[14];

int
chkpasswd(passwd, test)
  char *passwd, *test;
{
  char *pw;
  strncpy(pwbuf, test, 14);
  pw = crypt(pwbuf, passwd);
  return (!strncmp(pw, passwd, 14));
}

/* ------------------ */
/* �ˬd user ���U���p */
/* ------------------ */


int
bad_user_id(userid)
  char *userid;
{
  register char ch;

  if (belong("etc/baduser",userid))
    return 1;

  if (strlen(userid) < 2)
    return 1;

  if (not_alpha(*userid))
    return 1;

  if (!ci_strcmp(userid, str_new))
    return 1;

  while (ch = *(++userid))
  {
    if (not_alnum(ch))
      return 1;
  }
  return 0;
}


/* -------------------------------- */
/* New policy for allocate new user */
/* (a) is the worst user currently  */
/* (b) is the object to be compared */
/* -------------------------------- */

#undef VACATION     // �O�_���H�����O�d�b������

static int
compute_user_value(urec, clock)
  userec *urec;
  time_t clock;
{
  int value;

  /* if (urec) has XEMPT permission, don't kick it */
  if ((urec->userid[0] == '\0') || (urec->userlevel & PERM_XEMPT))
    return 9999;

  value = (clock - urec->lastlogin) / 60;       /* minutes */

  /* new user should register in 60 mins */
  if (strcmp(urec->userid, str_new) == 0)
    return (60 - value);

#ifdef  VACATION
  return 180 * 24 * 60 - value; /* �H�����O�s�b�� 180 �� */
#else
  if (!urec->numlogins)         /* �� login ���\�̡A���O�d */
    return -1;
  else if (urec->numlogins <= 3)     /* #login �֩�T�̡A�O�d 30 �� */
    return 30 * 24 * 60 - value;

  /* ���������U�̡A�O�d 30 �� */
  /* �@�뱡�p�A�O�d 180 �� */
  else
    return (urec->userlevel & PERM_LOGINOK ? 180 : 30) * 24 * 60 - value;
#endif
}


static int
getnewuserid()
{
  static char *fn_fresh = ".fresh";
  extern struct UCACHE *uidshm;
  userec utmp, zerorec;
  time_t clock;
  struct stat st;
  int fd, val, i;
  char genbuf[200];
  char genbuf2[200];

  memset(&zerorec, 0, sizeof(zerorec));
  clock = time(NULL);

  /* -------------------------------------- */
  /* Lazy method : ����M�w�g�M�����L���b�� */
  /* -------------------------------------- */

  if ((i = searchnewuser(0)) == 0)
  {

    /* ------------------------------- */
    /* �C 1 �Ӥp�ɡA�M�z user �b���@�� */
    /* ------------------------------- */

    if ((stat(fn_fresh, &st) == -1) || (st.st_mtime < clock - 3600))
    {
      if ((fd = open(fn_fresh, O_RDWR | O_CREAT, 0600)) == -1)
        return -1;
      write(fd, ctime(&clock), 25);
      close(fd);
      log_usies("CLEAN", "dated users");

      printf("�M��s�b����, �еy�ݤ���...\n\r");
      if ((fd = open(fn_passwd, O_RDWR | O_CREAT, 0600)) == -1)
        return -1;
      i = 0;  /* Ptt�ѨM�Ĥ@�ӱb���ѬO�Q����D */
      while (i < MAXUSERS)
      {
        i++;
        if (read(fd, &utmp, sizeof(userec)) != sizeof(userec))
          break;
	if(i==1) continue;
/*
woju
*/
        if ((val = compute_user_value(&utmp, clock)) < 0) {
           sprintf(genbuf, "#%d %-12s %15.15s %d %d %d",
             i, utmp.userid, ctime(&(utmp.lastlogin)) + 4,
             utmp.numlogins, utmp.numposts, val);
           if (val > -1 * 60 * 24 * 365)
           {
             log_usies("CLEAN", genbuf);
             sprintf(genbuf, "home/%s", utmp.userid);
             sprintf(genbuf2, "tmp/%s", utmp.userid);
// wildcat : ���� mv , ���ζ] rm home/userid
             if (dashd(genbuf))
               f_mv(genbuf, genbuf2);
             lseek(fd, (off_t)((i - 1) * sizeof(userec)), SEEK_SET);
             write(fd, &zerorec, sizeof(utmp));
           }
           else
              log_usies("DATED", genbuf);
        }
      }
      close(fd);
      time(&(uidshm->touchtime));
    }
  }
  if ((fd = open(fn_passwd, O_RDWR | O_CREAT, 0600)) == -1)
    return -1;
  flock(fd, LOCK_EX);

  i = searchnewuser(1);
  if ((i <= 0) || (i > MAXUSERS))
  {
    flock(fd, LOCK_UN);
    close(fd);
    if (more("etc/user_full", NA) == -1)
      printf("��p�A�ϥΪ̱b���w�g���F�A�L�k���U�s���b��\n\r");
    val = (st.st_mtime - clock + 3660) / 60;
    printf("�е��� %d ������A�դ@���A���A�n�B\n\r", val);
    sleep(2);
    exit(1);
  }

  sprintf(genbuf, "uid %d", i);
  log_usies("APPLY", genbuf);

  strcpy(zerorec.userid, str_new);
  zerorec.lastlogin = clock;
  if (lseek(fd, (off_t)(sizeof(zerorec) * (i - 1)), SEEK_SET) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  write(fd, &zerorec, sizeof(zerorec));
  setuserid(i, zerorec.userid);
  flock(fd, LOCK_UN);
  close(fd);
  return i;
}

#ifdef REG_FORM
int u_register();
#endif

void
new_register()
{
  userec newuser;
  rpgrec newrpg;
  char passbuf[STRLEN];
  int allocid, try;

  memset(&newuser, 0, sizeof(newuser));
  memset(&newrpg, 0, sizeof(newrpg));

  more("etc/register", NA);
  try = 0;
  while (1)
  {
    if (++try >= 6)
    {
      refresh();

      pressanykey("�z���տ��~����J�Ӧh�A�ФU���A�ӧa");
      oflush();
      exit(1);
    }
    getdata(16, 0, msg_uid, newuser.userid, IDLEN + 1, DOECHO,0);

    if (bad_user_id(newuser.userid))
      outs("�L�k�����o�ӥN���A�Шϥέ^��r���A�åB���n�]�t�Ů�\n");
    else if (searchuser(newuser.userid))
      outs("���N���w�g���H�ϥ�\n");
    else
      break;
  }

  try = 0;
  while (1)
  {
    if (++try >= 6)
    {
      pressanykey("�z���տ��~����J�Ӧh�A�ФU���A�ӧa");
      oflush();
      exit(1);
    }
    if ((getdata(17, 0, "�г]�w�K�X�G", passbuf, PASSLEN, PASS,0) < 4) ||
      !strcmp(passbuf, newuser.userid))
    {
      pressanykey("�K�X��²��A���D�J�I�A�ܤ֭n 4 �Ӧr�A�Э��s��J");
      continue;
    }
    strncpy(newuser.passwd, passbuf, PASSLEN);
    getdata(18, 0, "���ˬd�K�X�G", passbuf, PASSLEN, PASS,0);
    if (strncmp(passbuf, newuser.passwd, PASSLEN))
    {
      outs("�K�X��J���~, �Э��s��J�K�X.\n");
      continue;
    }
    passbuf[8] = '\0';
    strncpy(newuser.passwd, genpasswd(passbuf), PASSLEN);
    break;
  }
  newuser.userlevel = PERM_DEFAULT;
  newuser.pager = 1;
  newuser.uflag = COLOR_FLAG | BRDSORT_FLAG | MOVIE_FLAG;
  newuser.firstlogin = newuser.lastlogin = time(NULL);
  srandom(time(0));
  newuser.silvermoney = 10000;
  newuser.habit = HABIT_NEWUSER;	/* user.habit */
  strcpy(newrpg.userid,newuser.userid);
  allocid = getnewuserid();
  if (allocid > MAXUSERS || allocid <= 0)
  {
    fprintf(stderr, "�����H�f�w�F���M�I\n");
    exit(1);
  }
  

  if (substitute_record(fn_passwd, &newuser, sizeof(userec), allocid) == -1)
  {
    fprintf(stderr, "�Ⱥ��F�A�A���I\n");
    exit(1);
  }

  setuserid(allocid, newuser.userid);
  if (!dosearchuser(newuser.userid))
  {
    fprintf(stderr, "�L�k�إ߱b��\n");
    exit(1);
  }
  rpg_rec(newuser.userid,newrpg);
}

/* origin: SOB & Ptt              */
/* modify: wildcat/980909         */
/* �T�{user�O�_�q�L���U�B��ƥ��T */
check_register()
{
  char *ptr;
  char genbuf[200],buf[100];

//  if(!HAS_PERM(PERM_POST) && (cuser.lastlogin - cuser.firstlogin >= 3 * 86400))
//    cuser.userlevel |= PERM_POST;  

  stand_title("�иԲӶ�g�ӤH���");

  while (strlen(cuser.username) < 2)
    getdata(2, 0, "�︹�ʺ١G", cuser.username, 24, DOECHO,0);
  strcpy(currutmp->username, cuser.username);

  while (strlen(cuser.feeling) < 2)
    getdata(3, 0, "�߱����A�G", cuser.feeling, 5, DOECHO,0);
  cuser.feeling[4] = '\0';
  strcpy(currutmp->feeling, cuser.feeling);

  for (ptr = cuser.username; *ptr; ptr++)
  {
    if (*ptr == 9)              /* TAB convert */
      strcpy(ptr, " ");
  }
  while (strlen(cuser.realname) < 4)
    getdata(4, 0, "�u��m�W�G", cuser.realname, 20, DOECHO,0);

//  while (!cuser.month || !cuser.day || !cuser.year)
  while (cuser.month > 12 || cuser.month < 1 || 
   cuser.day > 31 || cuser.day < 1) 
  {
      sprintf(genbuf, "%02i/%02i/%02i",
      cuser.year,cuser.month, cuser.day);
      getdata(6, 0, "�X�ͦ~�� �褸 19", buf, 3, DOECHO,0);
//      cuser.year = (buf[0] - '0') * 10 + (buf[1] - '0');
      cuser.year = atoi (buf);
      getdata(7, 0, "�X�ͤ��", buf, 3, DOECHO,0);
//      cuser.month = (buf[0] - '0') * 10 + (buf[1] - '0');
      cuser.month = atoi (buf);
      getdata(8, 0, "�X�ͤ��", buf, 3, DOECHO,0);
//      cuser.day = (buf[0] - '0') * 10 + (buf[1] - '0');
      cuser.day = atoi (buf);
//      if (cuser.month > 12 || cuser.month < 1 ||
//        cuser.day > 31 || cuser.day < 1 || cuser.year > 90 || cuser.year < 40)
//        continue;
//      break;
    }

  while (strlen(cuser.address) < 8)
    getdata(9, 0, "�p���a�}�G", cuser.address, 50, DOECHO,0);

  while (cuser.sex > 7)
  {
    char buf[10];
    getdata(10, 0, 
      "�ʧO (1)���� (2)�j�� (3)���} (4)���� (5)���� (6)���� (7)�Ӫ� (8)�q��",
      buf , 3, DOECHO, 0);
    if (buf[0] >= '1' && buf[0] <= '8')
      cuser.sex = buf[0] - '1';
  }

  if (belong_spam(BBSHOME"/etc/spam-list",cuser.email))
  {
    strcpy(cuser.email,"NULL");
    pressanykey("��p,�����������A�� E-Mail �H�c��m");
  }
  
  if (!strchr(cuser.email, '@'))
  {
    bell();
    move(t_lines - 4, 0);
    prints("\
�� ���F�z���v�q�A�ж�g�u�ꪺ E-mail address�A �H��T�{�դU�����A\n\
   �榡�� [44muser@domain_name[0m �� [44muser@\\[ip_number\\][0m�C\n\n\
�� �p�G�z�u���S�� E-mail�A�Ъ����� [return] �Y�i�C");

    do
    {
      getdata(12, 0, "�q�l�H�c�G", cuser.email, 50, DOECHO,0);
      if (!cuser.email[0])
        sprintf(cuser.email, "%s%s", cuser.userid, str_mail_address);
      if(belong_spam(BBSHOME"/etc/spam-list",cuser.email))
      {
        strcpy(cuser.email, "NULL");
        pressanykey("��p,�����������A�� E-Mail �H�c��m");
      }
    } while (!strchr(cuser.email, '@'));

#ifdef  REG_MAGICKEY   
    mail_justify(cuser);
#endif

  }

  cuser.userlevel |= PERM_DEFAULT;
  if (!HAS_PERM(PERM_SYSOP) && !(cuser.userlevel & PERM_LOGINOK))
  {
    /* �^�йL�����{�ҫH��A�δ��g E-mail post �L */

    setuserfile(genbuf, "email");
    if (dashf(genbuf))
      cuser.userlevel |= ( PERM_POST );

//#ifdef  STRICT
#ifdef REG_EMAIL
    else
    {
      cuser.userlevel &= ~PERM_POST;
      more("etc/justify", YEA);
    }
#endif

  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
/* wildcat 981218 */
    clear();
    update_data(); 

#ifdef REG_FORM
//    if (HAS_PERM(PERM_POST) && !HAS_PERM(PERM_LOGINOK)
    if (HAS_PERM(PERM_BASIC) && !HAS_PERM(PERM_LOGINOK)
      && answer("�O�_�n��g���U�� (y/N)") == 'y')
    u_register();
#endif
    
//    u_habit();
  }
  if (HAS_PERM(PERM_DENYPOST) && !HAS_PERM(PERM_SYSOP))
    cuser.userlevel &= ~PERM_POST;
}
