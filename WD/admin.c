/*-------------------------------------------------------*/
/* admin.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : administration routines                      */
/* create : 95/03/29                                     */
/* update : 00/08/03					 */
/*-------------------------------------------------------*/
 
#define _ADMIN_C_

#include "bbs.h"

int
x_reg ()			/* �ѨM�L�k�{�� */
{
  if (dashf ("register.new.tmp"))
    {
      system ("cat register.new.tmp>>register.new");
      system ("rm register.new.tmp");
      pressanykey ("�˦n�F�A�U���p���I�աI");
    }
  return RC_FULL;
}

/* ----------------------------------------------------- */
/* �ݪO�޲z                                              */
/* ----------------------------------------------------- */

extern int cmpbnames ();

int
invalid_brdname (brd)		/* �w�q���~�ݪO�W�� */
     char *brd;
{
  register char ch;

  ch = *brd++;
  if (not_alnum (ch))
    return 1;
  while (ch = *brd++)
    {
      if (not_alnum (ch) && ch != '_' && ch != '-' && ch != '.')
	return 1;
    }
  return 0;
}


unsigned
setperms (pbits)
     unsigned pbits;
{
  register int i;
  char choice[4];

  move (4, 0);
  for (i = 0; i < NUMPERMS / 2; i++)
    {
      prints ("%c. %-20s %-15s %c. %-20s %s\n"
	      ,'A' + i, permstrings[i], ((pbits >> i) & 1 ? "��" : "  ")
	      ,i < 10 ? 'Q' + i : '0' + i - 10,
	      permstrings[i + 16], ((pbits >> i + 16) & 1 ? "��" : "  "));
    }
  clrtobot ();
  while (getdata (b_lines - 1, 0, "�Ы� [A-5] �����]�w�A�� [Return] �����G",
		  choice, 3, LCECHO, 0))
    {
      i = choice[0] - 'a';
      if (i < 0)
	i = choice[0] - '0' + 26;
      if(i >= 15 && !HAS_PERM(PERM_BBSADM))
      {
        bell();
        pressanykey("Ur not BBS-Administrator");
        return (pbits);
      }
      if (i >= NUMPERMS)
	bell ();
      else
	{
	  pbits ^= (1 << i);
	  move (i % 16 + 4, i <= 15 ? 24 : 64);
	  prints ((pbits >> i) & 1 ? "��" : "  ");
	}
    }
  return (pbits);
}


/* -------------- */
/* �۰ʳ]�ߺ�ذ� */
/* -------------- */


static void
setup_man (board)
     boardheader *board;
{
  char genbuf[200];

  setapath (genbuf, board->brdname);
  mkdir (genbuf, 0755);
}


static void
bperm_msg (board)
     boardheader *board;
{
  prints ("\n�]�w [%s] �ݪO��(%s)�v���G", board->brdname,
	  board->brdattr & BRD_POSTMASK ? "�o��" : "�\\Ū");
}
static char *classname[] =
{
  "1.����", "2.�ӤH", "3.�T��",
  "4.��", "5.�B��", "6.����",
  "7.�ն�", "8.�a��", "9.����",
  "[�Φۦ��J]",
  NULL
};

int
m_newbrd ()
{
  boardheader newboard;
  char ans[4];
  int bid, classnum = 0;
  char genbuf[200];
  extern char *boardprefix;

  stand_title ("�إ߷s�O");
  memset (&newboard, 0, sizeof (newboard));
  newboard.opentime = time(0);

  do
    {
      if (!getdata (3, 0, msg_bid, newboard.brdname, IDLEN + 1, DOECHO, 0))
	return -1;
    }
  while (invalid_brdname (newboard.brdname));

  if (HAS_PERM (PERM_SYSOP))
    while (classname[classnum] != NULL)
      outs (classname[classnum++]);
  do
    {
      getdata (6, 0, "�ݪO���O�G", genbuf, 5, DOECHO, 0);
      if (HAS_PERM (PERM_SYSOP) ||
	  ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	break;
      outs ("�u��}�ҰϤ����O����");
    }
  while (1);

  if (genbuf[0] >= '1' && genbuf[0] <= '9')
    {
      strncpy (newboard.title, classname[*genbuf - '1'] + 2, 4);
    }
  else if (strlen (genbuf) >= 4)
    {
      strncpy (newboard.title, genbuf, 4);
    }
  newboard.title[4] = ' ';
  getdata (7, 0, "1.��H �� 2.����H �� 3.�l��� �U 6.�����ݪO", genbuf, 3
	   ,LCECHO, 0);
//  if((genbuf[0] == '3')&& !HAS_PERM(PERM_SYSOP)) genbuf[0] = '2';

  newboard.brdattr = 0;
  newboard.totaltime = 0;
  newboard.totalvisit = 0;
  if (genbuf[0])
    {
      strncpy (newboard.title + 5, ((*genbuf == '1') ? "��" : 
                                 (*genbuf == '2') ? "��" :
                                 (*genbuf == '3') ? "�U" : 
                                 (*genbuf == '4') ? "��" : 
                                 (*genbuf == '5') ? "��" : "��"), 2);
      switch (*genbuf)
	{
	case '6':
	  newboard.brdattr |= BRD_CLASS;
	  newboard.brdattr &= ~BRD_GROUPBOARD;
	  newboard.brdattr |= BRD_NOCOUNT;

	case '5':
	  newboard.brdattr |= BRD_NOTRAN;
	  newboard.brdattr |= BRD_GOOD;
	  break;
	case '4':
	  newboard.brdattr &= ~BRD_NOTRAN;
	  newboard.brdattr |= BRD_GOOD;
	  break;
	case '3':
	  newboard.brdattr &= ~BRD_CLASS;
	  newboard.brdattr |= BRD_GROUPBOARD;
	  newboard.brdattr |= BRD_NOCOUNT;
	case '2':
	  newboard.brdattr |= BRD_NOTRAN;
	  break;
	case '1':
	  newboard.brdattr &= ~BRD_NOTRAN;
	  break;
	}
    }

  getdata (8, 0, "�ݪO�D�D�G", genbuf, BTLEN + 1, DOECHO, 0);
  if (genbuf[0])
    {
      strcpy (newboard.title + 7, genbuf);
    }
  setbpath (genbuf, newboard.brdname);
  mkdir (genbuf, 0755);
  if (getbnum (newboard.brdname) > 0)
    {
      pressanykey (err_bid);
      return -1;
    }
  getdata (9, 0, "�O�D�W��G", newboard.BM, IDLEN * 3 + 3, DOECHO, 0);


  if (HAS_PERM (PERM_SYSOP))
    {
      getdata (10, 0, "�i�HZAP(Y/N)�H", ans, 4, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOZAP;
      getdata (11, 0, "�i�H�ΦW(Y/N)�H", ans, 4, LCECHO, "N");
      if (*ans == 'y')
	newboard.brdattr |= BRD_ANONYMOUS;
    }
  newboard.level = 0;
  if (!(newboard.brdattr & BRD_GROUPBOARD || newboard.brdattr & BRD_CLASS))
    {
      getdata (12, 0, "�O�_���ӤH�O (Y/N)�H", ans, 4, LCECHO, 0);
      if (*ans == 'y' || *ans == 'Y')
        newboard.brdattr |= BRD_PERSONAL;
      getdata (13, 0, HAS_PERM (PERM_SYSOP) ?
	       "�ݪO����/�p�H/���}(Y/P/N)�H" : "�ݪO�p�H/���}(P/N)�H"
	       ,ans, 4, LCECHO, "N");
      if (*ans == 'y' && HAS_PERM (PERM_SYSOP))
	{
	  newboard.brdattr |= BRD_HIDE;
	  newboard.brdattr |= BRD_POSTMASK;
	}
      else if (*ans == 'p')
	{
	  newboard.brdattr |= BRD_HIDE;
	}

      getdata (14, 0, "�C�J�έp/�Ʀ�(Y/N)�H", ans, 4, LCECHO, "Y");
      if (*ans == 'n')
	newboard.brdattr |= BRD_NOCOUNT;

      if (HAS_PERM (PERM_SYSOP) && !(newboard.brdattr & BRD_HIDE))
	{
	  getdata (15, 0, "�]�wŪ�g�v��(Y/N)�H", ans, 4, LCECHO, "Y");
	  if (*ans == 'y')
	    {
	      getdata (16, 0, "���� [R]�\\Ū (P)�o��H", ans, 4, LCECHO, "P");
	      if (*ans == 'p')
		newboard.brdattr |= BRD_POSTMASK;
	      move (1, 0);
	      clrtobot ();
	      bperm_msg (&newboard);
	      newboard.level = setperms (newboard.level);
	      clear ();
	    }
	}
      newboard.maxpost=DEF_MAXP;
      newboard.maxtime=DEF_MAXT;
    }
  else
    {
      if (!newboard.BM[0] || newboard.BM[0] == ' ')
	strcpy (newboard.BM, "[�ؿ�]");
      newboard.brdattr |= BRD_POSTMASK;
      newboard.level |= (1 << 15);
    }
  if ((bid = getbnum ("")) > 0)
    {
      substitute_record (fn_board, &newboard, sizeof (newboard), bid);
    }
  else if (rec_add (fn_board, &newboard, sizeof (newboard)) == -1)
    {
      pressanykey (NULL);
      return -1;
    }
  setup_man (&newboard);
  touch_boards ();
  log_usies ("NewBoard", newboard.title);
  pressanykey ("�s�O����");
  return 0;
}


int
m_mod_board (char *bname)
{
  boardheader bh, newbh;
  int bid, classnum = 0, avg;
  char genbuf[200];
  extern char *boardprefix;

  bid = getbnum (bname);
  if (rec_get (fn_board, &bh, sizeof (bh), bid) == -1)
    {
      pressanykey (err_bid);
      return -1;
    }
  move (1, 0);
  clrtobot ();
  prints ("�ݪO�W�١G%-15s�O�D�W��G%s\n�ݪO�����G%s\n",
	  bh.brdname, bh.BM, bh.title);
  {
    int t = bh.totaltime;
    int day = t / 86400, hour = (t / 3600) % 24, min = (t / 60) % 60, sec = t % 60;
    avg = bh.totalvisit != 0 ? bh.totaltime / bh.totalvisit : 0;
    prints ("�}�O�ɶ��G%s\n", Ctime(&bh.opentime));
    prints ("���X�H�ơG%-15d���X�`�ɡG%d �� %d �p�� %d �� %d �� �������d�G%d ��\n",
	    bh.totalvisit, day, hour, min, sec, avg);
  }
  prints ("�̷s�X�ȡG%-15s�̷s�ɶ��G%s\n�峹�W���G%-5d �g       �O�s�ɶ��G%-4d ��"
	  ,bh.lastvisit, Ctime (&bh.lastime),bh.maxpost,bh.maxtime);
  bperm_msg (&bh);
  prints ("%s�]��\n"
	  "�izap:%s �C�J�έp:%s ��H:%s �s�ժO:%s �p�H:%s ����:%s �ΦW:%s �u�}:%s �ӤH:%s",
	  bh.level ? "��" : "��",
	  bh.brdattr & BRD_NOZAP ? "��" : "��",
	  bh.brdattr & BRD_NOCOUNT ? "��" : "��",
	  bh.brdattr & BRD_NOTRAN ? "��" : "��",
	  bh.brdattr & BRD_GROUPBOARD ? "��" : "��",
	  bh.brdattr & BRD_HIDE ? "��" : "��",
	  (bh.brdattr & BRD_HIDE) && (bh.brdattr & BRD_POSTMASK) ? "��" : "��",
	  bh.brdattr & BRD_ANONYMOUS ? "��" : "��",
	  bh.brdattr & BRD_GOOD ? "��" : "��",
	  bh.brdattr & BRD_PERSONAL ? "��" : "��");
  if (!HAS_PERM (PERM_BOARD) || !strcmp(bname,"Security"))
    {
      pressanykey (NULL);
      return 0;
    }

  getdata (9, 0, "�ݪO (D)�R�� (E)�]�w (L)�W�� (B)�O�D (C)�y�z [Q]�����H", genbuf, 3, LCECHO, 0);

  switch (*genbuf)
    {
    case 'd':
      if (!HAS_PERM (PERM_SYSOP))
	break;
      getdata (10, 0, msg_sure_ny, genbuf, 3, LCECHO, "N");
      if (genbuf[0] != 'y')
	{
	  outs (MSG_DEL_CANCEL);
	}
      else
	{
	if (*bname)
	 {
	  strcpy (bname, bh.brdname);
	  sprintf (genbuf, "/bin/rm -fr boards/%s man/boards/%s", bname, bname);
	  system (genbuf);
	  memset (&bh, 0, sizeof (bh));
	  sprintf (bh.title, "[%s] deleted by %s", bname, cuser.userid);
	  substitute_record (fn_board, &bh, sizeof (bh), bid);
	  touch_boards ();
	  log_usies ("DelBoard", bh.title);
	  outs ("�R�O����");
	 }
	}
      break;

    case 'e':

      move (5, 0);
      clrtobot();
      outs ("������ [Return] ���ק�Ӷ��]�w");
      memcpy (&newbh, &bh, sizeof (bh));

      while (getdata (11, 0, "�s�ݪO�W�١G", genbuf, IDLEN + 1, DOECHO, 0))
	{
	  if (getbnum (genbuf))
	    pressanykey ("���~! �O�W�p�P");
	  else if (!invalid_brdname (genbuf))
	    {
	      strcpy (newbh.brdname, genbuf);
	      break;
	    }
	}
      if (HAS_PERM (PERM_SYSOP))
	while (classname[classnum] != NULL)
	  outs (classname[classnum++]);
      do
	{
	  getdata (12, 0, "�ݪO���O�G", genbuf, 5, DOECHO, bh.title);
	  if (HAS_PERM (PERM_SYSOP) ||
	   ((strstr (boardprefix, genbuf) != NULL) && strlen (genbuf) == 4))
	    break;
	  outs ("�u��}�ҰϤ����O����");
	}
      while (1);

      if (genbuf[0] >= '1' && genbuf[0] <= '9')
	strncpy (newbh.title, classname[*genbuf - '1'] + 2, 4);
      else if (strlen (genbuf) >= 4)
	strncpy (newbh.title, genbuf, 4);

      newbh.title[4] = ' ';
      getdata (13, 0, "1.��H �� 2.����H �� [3.�l��� �U]  4.�u�}�ݪO(��H) ��  5.�u�}�ݪO(����) ��", genbuf, 3
	       ,LCECHO, 0);
      if (genbuf[0])
	{
	  strncpy (newbh.title + 5, ((*genbuf == '1') ? "��" : (*genbuf == '2') ? "��" :
				     (*genbuf == '3') ? "�U" : (*genbuf == '4') ? "��" : (*genbuf == '5') ? "��": "��"), 2);
	  switch (*genbuf)
	    {
	    case '6':
	      newbh.brdattr |= BRD_CLASS;
	      newbh.brdattr &= ~BRD_GROUPBOARD;
	      newbh.brdattr |= BRD_NOCOUNT;

	    case '5':
	      newbh.brdattr &= ~BRD_GROUPBOARD;
	      newbh.brdattr |= BRD_NOTRAN;
	      newbh.brdattr |= BRD_GOOD;
	      break;
	    case '4':
	      newbh.brdattr &= ~BRD_GROUPBOARD;
	      newbh.brdattr &= ~BRD_NOTRAN;
	      newbh.brdattr |= BRD_GOOD;
	      break;
	    case '3':
     	      newbh.brdattr &= ~BRD_CLASS;
	      newbh.brdattr |= BRD_GROUPBOARD;
	      newbh.brdattr |= BRD_NOCOUNT;
	      newbh.brdattr |= BRD_NOTRAN;
	      break;
	    case '2':
	      newbh.brdattr &= ~BRD_GROUPBOARD;
	      newbh.brdattr |= BRD_NOTRAN;
	      break;
	    case '1':
	      newbh.brdattr &= ~BRD_GROUPBOARD;
	      newbh.brdattr &= ~BRD_NOTRAN;
	      break;
	    }
	}
      getdata (14, 0, "�ݪO�D�D�G", genbuf, BTLEN + 1, DOECHO, bh.title + 7);
      if (genbuf[0])
	{
	  strcpy (newbh.title + 7, genbuf);
	}

//      sprintf(genbuf,"%d",bh.totaltime);
//wildcat tmp
//      getdata (14, 0, "�ݪO�ɶ��G", genbuf, BTLEN + 1, DOECHO, genbuf);
//      if (atol(genbuf) >= 0)
//        newbh.totaltime = atol(genbuf);

      if (getdata (15, 0, "�s�O�D�W��G", genbuf, IDLEN * 3 + 3,
		   DOECHO, bh.BM))
	{
	  str_trim (genbuf);
	  strcpy (newbh.BM, genbuf);
	}
      if (HAS_PERM (PERM_SYSOP))
	{
	  getdata (16, 0, "�i�HZAP(Y/N)�H", genbuf, 4, LCECHO,
		   newbh.brdattr & BRD_NOZAP ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOZAP;
	  else
	    newbh.brdattr &= ~BRD_NOZAP;
	  getdata (17, 0, "�i�H�ΦW(Y/N)�H", genbuf, 4, LCECHO,
		   newbh.brdattr & BRD_ANONYMOUS ? "Y" : "N");
	  if (*genbuf == 'y')
	    newbh.brdattr |= BRD_ANONYMOUS;
	  else
	    newbh.brdattr &= ~BRD_ANONYMOUS;

	}
      if (!(newbh.brdattr & BRD_GROUPBOARD || newbh.brdattr & BRD_CLASS))
	{
	  char ans[4];
          getdata (18, 0, "�O�_���ӤH�O (Y/N)�H", ans, 4, LCECHO, bh.brdattr & BRD_PERSONAL ? "y" : "n");
          if (*ans == 'y' || *ans == 'Y')
            newbh.brdattr |= BRD_PERSONAL;
          else
            newbh.brdattr &= ~BRD_PERSONAL;
	  getdata (19, 0, HAS_PERM (PERM_SYSOP) ? "�ݪO����/�p�H/���}(Y/P/N)�H"
		   : "�ݪO�p�H/���}(P/N)�H", ans, 4, LCECHO,
		   !(newbh.brdattr & BRD_HIDE) ? "N" :
		   (newbh.brdattr & BRD_POSTMASK) ? "Y" : "P");
	  if (*ans == 'y' && HAS_PERM (PERM_SYSOP))
	    {
	      newbh.brdattr |= BRD_HIDE;
	      newbh.brdattr |= BRD_POSTMASK;
	    }
	  else if (*ans == 'p')
	    {
	      newbh.brdattr |= BRD_HIDE;
	      newbh.brdattr &= ~BRD_POSTMASK;
	    }
	  else
	    newbh.brdattr &= ~BRD_HIDE;

	  getdata (20, 0, "�C�J�έp/�Ʀ�(Y/N)�H", genbuf, 4, LCECHO,
		   newbh.brdattr & BRD_NOCOUNT ? "N" : "Y");
	  if (*genbuf == 'n')
	    newbh.brdattr |= BRD_NOCOUNT;
	  else
	    newbh.brdattr &= ~BRD_NOCOUNT;
	  if (HAS_PERM (PERM_SYSOP) && !(newbh.brdattr & BRD_HIDE))
	    {
	      getdata (21, 0, "�O�_���s���v��(Y/N)�H[N] ", genbuf, 4,
		       LCECHO, 0);
	      if (*genbuf == 'y')
		{
		  getdata (22, 0, "���� (R)�\\Ū (P)�o��H ", genbuf, 4, LCECHO,
			   (newbh.brdattr & BRD_POSTMASK ? "P" : "R"));
		  if (newbh.brdattr & BRD_POSTMASK)
		    {
		      if (*genbuf == 'r')
			newbh.brdattr &= ~BRD_POSTMASK;
		    }
		  else
		    {
		      if (*genbuf == 'p')
			newbh.brdattr |= BRD_POSTMASK;
		    }

		  move (1, 0);
		  clrtobot ();
		  bperm_msg (&newbh);
		  newbh.level = setperms (newbh.level);
		}
	    }
	}
      else
	{
	  newbh.brdattr |= BRD_POSTMASK;
	  newbh.level |= (1 << 15);
	}
      getdata (b_lines - 1, 0, msg_sure_ny, genbuf, 4, LCECHO, "Y");

      if ((*genbuf == 'y') && memcmp (&newbh, &bh, sizeof (bh)))
	{
	  if (strcmp (bh.brdname, newbh.brdname))
	    {
	      char src[60], tar[60];

	      setbpath (src, bh.brdname);
	      setbpath (tar, newbh.brdname);
	      f_mv (src, tar);

	      setapath (src, bh.brdname);
	      setapath (tar, newbh.brdname);
	      f_mv (src, tar);
	    }
	  setup_man (&newbh);
	  substitute_record (fn_board, &newbh, sizeof (newbh), bid);
	  touch_boards ();
	  log_usies ("SetBoard", newbh.brdname);
	}
      break;
    case 'b':
      memcpy (&newbh, &bh, sizeof (bh));
      if (getdata (11, 0, "�s�O�D�W��G", genbuf, IDLEN * 3 + 3, DOECHO, bh.BM))
	{
	  str_trim (genbuf);
	  strcpy (newbh.BM, genbuf);
	}
      substitute_record (fn_board, &newbh, sizeof (newbh), bid);
      touch_boards ();
      log_usies ("SetBoardBM", newbh.brdname);
      break;
    case 'c':
      memcpy (&newbh, &bh, sizeof (bh));
      outs("�糧�ݪO���y�z (�@�T��) :");
      getdata (11, 0, ":", genbuf, 79, DOECHO, bh.desc[0]);
      strcpy(newbh.desc[0],genbuf);
      getdata (12, 0, ":", genbuf, 79, DOECHO, bh.desc[1]);
      strcpy(newbh.desc[1],genbuf);
      getdata (13, 0, ":", genbuf, 79, DOECHO, bh.desc[2]);
      strcpy(newbh.desc[2],genbuf);
      substitute_record (fn_board, &newbh, sizeof (newbh), bid);
      touch_boards ();
      log_usies ("SetBoardDesc", newbh.brdname);
      break;
    case 'l':
    {
      char maxd[10],maxp[10];

      memcpy (&newbh, &bh, sizeof (bh));
      sprintf(maxd,"%d",bh.maxtime);
      sprintf(maxp,"%d",bh.maxpost);
      if (getdata (11, 0, "�ݪO�̤j�g�ơG", maxp, 6, DOECHO, maxp))
      {
        if(atoi(maxp) >= 0)
          newbh.maxpost = atoi(maxp);
      }
      if (getdata (13, 0, "�峹�O�d�ѼơG", maxd, 6, DOECHO, maxd))
      {
        if(atoi(maxd) >= 0)
          newbh.maxtime = atoi(maxd);
      }
      substitute_record (fn_board, &newbh, sizeof (newbh), bid);
      touch_boards ();
      log_usies ("SetBLimit", newbh.brdname);
    }
    break;
  }
  return 0;
}

int
m_board ()
{
  char bname[20];
  stand_title ("�ݪO�]�w");
  make_blist ();
  namecomplete (msg_bid, bname);
  if (!*bname)
    return 0;
  m_mod_board (bname);
}

/* ----------------------------------------------------- */
/* �ϥΪ̺޲z                                            */
/* ----------------------------------------------------- */

int
m_user ()
{
  userec muser;
  int id;
  char genbuf[200];

  stand_title ("�ϥΪ̳]�w");
  usercomplete (msg_uid, genbuf);
  if (*genbuf)
    {
      move (2, 0);
      if (id = getuser (genbuf))
	{
	  memcpy (&muser, &xuser, sizeof (muser));
	  uinfo_query (&muser, 1, id);
	}
      else
	{
	  pressanykey (err_uid);
	}
    }
  return 0;
}


#ifdef HAVE_TIN
int
post_in_tin (username)
     char *username;
{
  char buf[256];
  FILE *fh;
  int counter = 0;

  sethomefile (buf, username, ".tin/posted");
  fh = fopen (buf, "r");
  if (fh == NULL)
    return 0;
  else
    {
      while (fgets (buf, 255, fh) != NULL)
	{
	  if (buf[9] != 'd' && strncmp (&buf[11], "csie.bbs.test", 13))
	    counter++;
	  if (buf[9] == 'd')
	    counter--;
	}
      fclose (fh);
      return counter;
    }

}
#endif


/* ----------------------------------------------------- */
/* �M���ϥΪ̫H�c                                        */
/* ----------------------------------------------------- */


#ifdef  HAVE_MAILCLEAN
FILE *cleanlog;
char curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

static int
domailclean (fhdrp)
     fileheader *fhdrp;
{
  static int newcnt, savecnt, deleted, idc;
  char buf[STRLEN];

  if (!fhdrp)
    {
      fprintf (cleanlog, "new = %d, saved = %d, deleted = %d\n",
	       newcnt, savecnt, deleted);
      newcnt = savecnt = deleted = idc = 0;
      if (delcnt)
	{
	  sethomedir (buf, curruser);
	  while (delcnt--)
	    rec_del (buf, sizeof (fileheader), delmsgs[delcnt]);
	}
      delcnt = 0;
      return 1;
    }
  idc++;
  if (!(fhdrp->filemode & FILE_READ))
    newcnt++;
  else if (fhdrp->filemode & FILE_MARKED)
    savecnt++;
  else
    {
      deleted++;
      sethomefile (buf, curruser, fhdrp->filename);
      unlink (buf);
      delmsgs[delcnt++] = idc;
    }
  return 0;
}


static int
cleanmail (urec)
     userec *urec;
{
  struct stat statb;
  char genbuf[200];

  if (urec->userid[0] == '\0' || !strcmp (urec->userid, str_new))
    return;
  sethomedir (genbuf, urec->userid);
  fprintf (cleanlog, "%s�G", urec->userid);
  if (stat (genbuf, &statb) == -1 || statb.st_size == 0)
    fprintf (cleanlog, "no mail\n");
  else
    {
      strcpy (curruser, urec->userid);
      delcnt = 0;
      rec_apply (genbuf, domailclean, sizeof (fileheader));
      domailclean (NULL);
    }
  return 0;
}


int
m_mclean ()
{
  char ans[4];

  getdata (b_lines - 1, 0, msg_sure_ny, ans, 4, LCECHO, "N");
  if (ans[0] != 'y')
    return XEASY;

  cleanlog = fopen ("mailclean.log", "w");
  outmsg ("This is variable msg_working!");

  move (b_lines - 1, 0);
  if (rec_apply (fn_passwd, cleanmail, sizeof (userec)) == -1)
    {
      outs (ERR_PASSWD_OPEN);
    }
  else
    {
      fclose (cleanlog);
      outs ("�M������! �O���� mailclean.log.");
    }
  return XEASY;
}
#endif /* HAVE_MAILCLEAN */

/* ----------------------------------------------------- */
/* �B�z Register Form                                    */
/* ----------------------------------------------------- */

static int
scan_register_form (regfile)
     char *regfile;
{
  char genbuf[200];
  static char *logfile = "register.log";
  static char *field[] =
  {"num", "uid", "name", "howto", "email",
   "addr", "phone", "career", NULL};
  static char *finfo[] =
  {"�b���s��", "�ӽХN��", "�u��m�W", "�q��B�o��",
   "�q�l�l��H�c", "�ثe��}", "�s���q��", "�A�ȳ��", "��Ŧ�", NULL};
  static char *reason[] =
  {"��J�u��m�W", "�Զ�Ǯլ�t�P�~��",
   "��g���㪺��}���", "�Զ�s���q��", "�T���g���U�ӽЪ�",
   "�Τ����g�ӽг�", "�ж�g�q��B�o������", NULL};

  userec muser;
  FILE *fn, *fout, *freg;
  char fdata[8][STRLEN];
  char fname[STRLEN], buf[STRLEN];
  char ans[4], *ptr, *uid;
  int n, unum;

  uid = cuser.userid;
  sprintf (fname, "%s.tmp", regfile);
  if (dashf (fname))
    {
      pressanykey ("��L SYSOP �]�b�f�ֵ��U�ӽг�");
      return -1;
    }
  f_mv (regfile, fname);
  if ((fn = fopen (fname, "r")) == NULL)
    {
      pressanykey ("�t�ο��~�A�L�kŪ�����U�����: %s", fname);
      return -1;
    }

  memset (fdata, 0, sizeof (fdata));
  while (fgets (genbuf, STRLEN, fn))
    {
      if (ptr = (char *) strstr (genbuf, ": "))
	{
	  *ptr = '\0';
	  for (n = 0; field[n]; n++)
	    {
	      if (strcmp (genbuf, field[n]) == 0)
		{
		  strcpy (fdata[n], ptr + 2);
		  if (ptr = (char *) strchr (fdata[n], '\n'))
		    *ptr = '\0';
		}
	    }
	}
      else if ((unum = getuser (fdata[1])) == 0)
	{
	  move (2, 0);
	  clrtobot ();
	  for (n = 0; field[n]; n++)
	    prints ("%s     : %s\n", finfo[n], fdata[n]);
	  pressanykey ("�t�ο��~�A�d�L���H");
	}
      else
	{
	  memcpy (&muser, &xuser, sizeof (muser));
	  move (1, 0);
	  prints ("�b����m    �G%d\n", unum);
	  user_display (&muser, 1);
	  move (13, 0);
	  prints ("[1;32m---- �Я����Y��f�֨ϥΪ̸�� - �o�O�� User �� %d �����U ---[m\n", muser.rtimes);
	  for (n = 0; field[n]; n++)
	    prints ("%-12s�G%s\n", finfo[n], fdata[n]);
	  if (muser.userlevel & PERM_LOGINOK)
	    {
	      getdata (b_lines - 1, 0, "���b���w�g�������U, ��s(Y/N/Skip)�H[N] ",
		       ans, 3, LCECHO, "Y");
	      if (ans[0] != 'y' && ans[0] != 's')
		ans[0] = 'd';
	    }
	  else
	    getdata (b_lines - 1, 0, "�O�_���������(Y/N/Q/Del/Skip)�H[S] ",
		     ans, 3, LCECHO, "S");
	  move (2, 0);
	  clrtobot ();

	  switch (ans[0])
	    {
	    case 'y':

	      prints ("�H�U�ϥΪ̸�Ƥw�g��s:\n");
	      mail2user (muser,"[���U���\\�o]",BBSHOME"/etc/registered");
	      muser.userlevel |= (PERM_LOGINOK | PERM_POST | PERM_PAGE | PERM_CHAT);
	      sprintf (genbuf, "%s:%s:%s", fdata[6], fdata[7], uid);
	      strncpy (muser.justify, genbuf, REGLEN);
	      sethomefile (buf, muser.userid, "justify");
	      if (fout = fopen (buf, "a"))
		{
		  fprintf (fout, "%s\n", genbuf);
		  fclose (fout);
		}
	      substitute_record (fn_passwd, &muser, sizeof (muser), unum);

	      if (fout = fopen (logfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (fout, "%s: %s\n", field[n], fdata[n]);
		  n = time (NULL);

		  fprintf (fout, "Date: %s\n", Etime (&n));
		  fprintf (fout, "Approved: %s\n", uid);
		  fprintf (fout, "----\n");
		  fclose (fout);
		}
	      break;

	    case 'q':		/* �Ӳ֤F�A������ */

	      if (freg = fopen (regfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (freg, "%s: %s\n", field[n], fdata[n]);
		  fprintf (freg, "----\n");
		  while (fgets (genbuf, STRLEN, fn))
		    fputs (genbuf, freg);
		  fclose (freg);
		}

	    case 'd':
	      break;

	    case 'n':

	      for (n = 0; field[n]; n++)
		prints ("%s: %s\n", finfo[n], fdata[n]);
	      move (9, 0);
	      prints ("�д��X�h�^�ӽЪ��]�A�� <enter> ����\n");
	      for (n = 0; reason[n]; n++)
		prints ("%d) ��%s\n", n, reason[n]);

/*
   woju
 */
	      if (getdata (10 + n, 0, "�h�^��]�G", buf, 60, DOECHO, 0))
		{
		  int i;
		  fileheader mhdr;
		  char title[128], buf1[80];
		  FILE *fp;

		  i = buf[0] - '0';
		  if (i >= 0 && i < n)
		    strcpy (buf, reason[i]);
		  sprintf (genbuf, "[�h�^��]] ��%s", buf);

		  sethomepath (buf1, muser.userid);
		  stampfile (buf1, &mhdr);
		  strcpy (mhdr.owner, cuser.userid);
		  strncpy (mhdr.title, "[���U����]", TTLEN);
		  mhdr.savemode = 0;
		  mhdr.filemode = 0;
		  sethomedir (title, muser.userid);
		  if (rec_add (title, &mhdr, sizeof (mhdr)) != -1)
		    {
		      fp = fopen (buf1, "w");
		      fprintf (fp, "%s\n", genbuf);
		      fclose (fp);
		    }
/*
   strncpy(muser.address, genbuf, NAMELEN);
   substitute_record(fn_passwd, &muser, sizeof(muser), unum);
 */
		  if (fout = fopen (logfile, "a"))
		    {
		      for (n = 0; field[n]; n++)
			fprintf (fout, "%s: %s\n", field[n], fdata[n]);
		      n = time (NULL);
		      fprintf (fout, "Date: %s\n", Etime (&n));
		      fprintf (fout, "Rejected: %s [%s]\n----\n", uid, buf);
		      fclose (fout);
		    }
		  break;
		}
	      move (10, 0);
	      clrtobot ();
	      prints ("�����h�^�����U�ӽЪ�");

	    default:		/* put back to regfile */

	      if (freg = fopen (regfile, "a"))
		{
		  for (n = 0; field[n]; n++)
		    fprintf (freg, "%s: %s\n", field[n], fdata[n]);
		  fprintf (freg, "----\n");
		  fclose (freg);
		}
	    }
	}
    }
  fclose (fn);
  unlink (fname);
  return (0);
}


int
m_register ()
{
  FILE *fn;
  int x, y, wid, len;
  char ans[4];
  char genbuf[200];

  if ((fn = fopen (fn_register, "r")) == NULL)
    {
      outmsg ("�ثe�õL�s���U���");
      return XEASY;
    }

  stand_title ("�f�֨ϥΪ̵��U���");
  y = 2;
  x = wid = 0;

  while (fgets (genbuf, STRLEN, fn) && x < 65)
    {
      if (strncmp (genbuf, "uid: ", 5) == 0)
	{
	  move (y++, x);
	  outs (genbuf + 5);
	  len = strlen (genbuf + 5);
	  if (len > wid)
	    wid = len;
	  if (y >= t_lines - 3)
	    {
	      y = 2;
	      x += wid + 2;
	    }
	}
    }
  fclose (fn);
  getdata (b_lines - 1, 0, "�}�l�f�ֶ�(Y/N)�H[Y] ", ans, 3, LCECHO, "Y");
  if (ans[0] == 'y')
    scan_register_form (fn_register);

  return 0;
}

/* Ptt */
/*
extern int bad_user_id(char userid[]);
  int
search_bad_id()
{
  userec user;
  char ch;
  int coun=0;
  FILE *fp1=fopen(".PASSWDS","r");
  char buf[100];
  clear();
  while( (fread( &user, sizeof(user), 1, fp1))>0 ) {
    coun ++;
    move(0,0);
    sprintf(buf,"�M��l��id\n�� [%d] �����\n",coun);
    outs(buf);
    refresh();
    if(bad_user_id(user.userid))
    {
      uinfo_query(&user, 1, coun);
      outs("[44m               ���@��[37m:�j�M�U�@��          [33m Q[37m: ���}                         [m");
      ch=igetch();
      if(ch=='q' || ch =='Q') return 0;
      clear();
    }
  }
  fclose(fp1);
  return 0;
}
*/

int
search_key_user ()
{
  userec user;
  char ch;
  int coun = 0;
  FILE *fp1;
  char buf[100], key[22];

  ch = answer("�}�� �Q��(y) ����(t) ��L(o) ������");
  if(ch == 't')
    fp1 = fopen (BBSHOME"/.PASSWDS", "r");
  else if(ch == 'o')
    fp1 = fopen (BBSHOME"/PASSWDS", "r");
  else
    fp1 = fopen (BBSHOME"/.PASSWDS.yes","r");
  clear ();
  getdata (0, 0, "�п�J�ϥΪ�����r [�m�W|email|ID|�q��|�a�}]:", key, 21, DOECHO, 0);
  while ((fread (&user, sizeof (user), 1, fp1)) > 0)
    {
      coun++;
      move (1, 0);
      sprintf (buf, "�� [%d] �����\n", coun);
      outs (buf);
      refresh ();
      if (strstr (user.userid, key) || strstr (user.realname, key) ||
	  strstr (user.username, key) || strstr (user.lasthost, key) ||
	  strstr (user.email, key) || strstr (user.address, key) ||
	  strstr (user.justify, key))
	{
	  uinfo_query (&user, 1, coun);
	  outs (
"[1;44;33m       ���@��[37m:�j�M�U�@��          [33m Q[37m: ���}                        [m ");
	  ch = igetch ();
	  if (ch == 'q' || ch == 'Q')
	    return 0;
	  clear ();
	  move (0, 0);
	  outs ("�п�J�ϥΪ�����r [�m�W|email|ID|�q��|�a�}]:");
	  outs (key);
	}
    }
  fclose (fp1);
  return 0;
}

adm_givegold()
{
   int money;
   char id[IDLEN+1],buf[256],reason[60];
   FILE *fp=fopen("tmp/givebonus","w");
   fileheader mymail;

   time_t now;
   time(&now);
   move(12,0);
   update_data();
   usercomplete("��J��誺ID�G", id);
   if (!id[0] || !getdata(14, 0, "�n�o�h�ֿ������H", buf, 5, LCECHO,0)) return;
   money = atoi(buf);
   if(money > 0 && (inugold(id, money) != -1))
   {
     sprintf(buf,"�@��: %s \n"
                 "���D:[�o�����] �o���A %d ������I\n"
                 "�ɶ�: %s\n",cuser.userid,money,ctime(&now));
     fputs(buf,fp);
     while(!getdata(15,0,"�п�J�z�ѡG",reason,60,DOECHO ,"���� xxx �O��ذ�"));
     sprintf(buf,"[1;32m%s[37m �o���A [33m%d [37m�������C\n"
                 "�L���z�ѬO�G[33m %s [m",cuser.userid,money,reason);
     fputs(buf,fp);
     fclose(fp);
     sprintf(buf,"home/%s", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givebonus",buf);
     sprintf(mymail.title,"[�o�����] �e�A %d ��������I",money);
     sprintf(buf,"home/%s/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m����� [33m%d �� [37m�o��[33m %s[37m",
     Etime(&now),cuser.userid,money,id);
     f_cat("log/bonus.log",buf);
   }
   update_data();
   return;
}  

reload_cache ()
{
  reload_ucache ();
  reload_bcache ();
  reload_filmcache ();
  reload_fcache ();
  log_usies ("CACHE", "SYSOP Reload ALL CACHE!");
}

