/************************************************
*  pnote.c                                      *
*  �d�ܾ�                                       *
*                                1998.4.14      *
*                                    by herb    *
*************************************************/

#include        "bbs.h"
#define MAX_PNOTE        (10)            /* �������O�d�q�� */
#define MAXHINTLINES     (10)            /* �������D�H�d������ */
#define MAXPATHLEN	1024

static char *fn_note_tmp      = "pnote.tmp";
static char *fn_note_dat      = "pnote.dat";
static char *fn_note_dat_save = "pnote.dat.save";
static char *fn_pnote_ans      = "p_note.ans";
static char *fn_pnote_ans_save = "p_note.ans.save";

char *fn_pnote_hint = "p_note.hint";    /* �������D�H�d�� */

Del(char *dst) {
 char cmd[200];

 sprintf(cmd, "/bin/rm -f %s", dst);
 return system(cmd);
}

void
rebuild_pnote_ansi(int newflag) {
  char fname[MAXPATHLEN];
  char fpath[MAXPATHLEN];
  char buf[256], buf2[80];
  int total, len, i;
  int fd;
  struct stat st;
  notedata myitem;
  FILE *fp;

  if (newflag) {
    setuserfile(fname, fn_pnote_ans);
    setuserfile(fpath, fn_note_dat);
  }
  else {
    setuserfile(fname, fn_pnote_ans_save);
    setuserfile(fpath, fn_note_dat_save);
  }

  if ((fp = fopen(fname, "w")) == NULL) {
    return;
  }

  if ((fd = open(fpath, O_RDONLY)) == -1) {
    Del(fname);
    return;
  }
  else if (fstat(fd, &st) != -1) {
    total = st.st_size / sizeof(struct notedata);
    if (total > MAX_PNOTE)
      total = MAX_PNOTE;
  }
  fputs("\t\t\t[1;32m �� [37m�� �� �� �� �� �d ��[32m�� \n\n", fp);

  while (total) {
    if (total--)
      read(fd, (char *) &myitem, sizeof(myitem));
    sprintf(buf, "[1;33m���� [32m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf, " [33m" + (len & 1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "��");
    sprintf(buf2, "��[32m %.14s [33m����[m\n",
      Cdate(&(myitem.date)));
    strcat(buf, buf2);
    fputs(buf, fp);

    sprintf(buf, "%s\n%s\n%s\n", &myitem.buf[0][1], &myitem.buf[1][1], &myitem.buf[2][1]);
    fputs(buf, fp);

  }
  fclose(fp);
  close(fd);
}

void
do_pnote(char *userid) {
  int total, i, collect, len;
  struct stat st;
  char buf[256], buf2[80];
  char fname[MAXPATHLEN], fpath[MAXPATHLEN], fname2[MAXPATHLEN];
  int fd, fx;
  FILE *fp;
  notedata myitem;
  uschar mode0 = currutmp->mode;

  clrtobot();
  move(13, 0);
  prints("[1;33m�f������I[m");
  do
  {
    myitem.buf[0][0] = myitem.buf[1][0] = myitem.buf[2][0] = '\0';
    move(14,0);
    clrtobot();
    outs("\n�Яd�� (�ܦh�T�y)�A��[Enter]����");
    for (i = 0; (i < 3) &&
           getdata(16 + i, 0, ":", &myitem.buf[i][1], 77, DOECHO, 0); i++);

    getdata(b_lines - 1, 0, "(S)�x�s (E)���s�ӹL (Q)�����H[S] ", buf, 3, LCECHO, 0);
    if (buf[0] == 'q' || i == 0 && *buf != 'e') {
      currutmp->mode = mode0;
      return;
    }
  } while (buf[0] == 'e');

  setutmpmode(PRECORD);
  strcpy(myitem.userid, cuser.userid);
  strncpy(myitem.username, cuser.username, 18);
  myitem.username[18] = '\0';
  time(&(myitem.date));

  /* begin load file */

  sethomepath(fpath, userid);

  sprintf(fname, "%s/%s", fpath, fn_pnote_ans);
  if ((fp = fopen(fname, "w")) == NULL) {
    currutmp->mode = mode0;
    return;
  }

  sprintf(fname, "%s/%s", fpath, fn_note_tmp);
  if ((fx = open(fname, O_WRONLY | O_CREAT, 0644)) <= 0) {
    currutmp->mode = mode0;
    return;
  }

  sprintf(fname2, "%s/%s", fpath, fn_note_dat);
  if ((fd = open(fname2, O_RDONLY)) == -1)
  {
    total = 1;
  }
  else if (fstat(fd, &st) != -1)
  {
    total = st.st_size / sizeof(struct notedata) + 1;
    if (total > MAX_PNOTE)
      total = MAX_PNOTE;
  }

  fputs("\t\t\t[1;32m �� [37m�z �� �� �� �� !!! [32m�� \n\n", fp);
  collect = 1;
  while (total)
  {
    sprintf(buf, "[1;33m���� [32m%s[37m(%s)",
      myitem.userid, myitem.username);
    len = strlen(buf);
    strcat(buf, " [33m" + (len & 1));

    for (i = len >> 1; i < 36; i++)
      strcat(buf, "��");
    sprintf(buf2, "��[32m %.14s [33m����[m\n",
      Cdate(&(myitem.date)));
    strcat(buf, buf2);
    fputs(buf, fp);

    sprintf(buf, "%s\n%s\n%s\n", &myitem.buf[0][1], &myitem.buf[1][1], &myitem.buf[2][1]);
    fputs(buf, fp);

    write(fx, &myitem, sizeof(myitem));

    if (--total)
      read(fd, (char *) &myitem, sizeof(myitem));
  }
  fclose(fp);
  close(fd);
  close(fx);
  f_mv(fname, fname2);
  currutmp->mode = mode0;
}

void
show_pnote(notedata *pitem) {
//  char msg[STRLEN];
  char str[200];

  clrchyiuan(2,6);
  move(2, 0);
  sprintf(str, "[1;36m�z�w�w�w [37m%s(%s)�b [33m%s[37m �d���� [m", pitem->userid, pitem->username,
Cdate(&(pitem->date)));
//  cut_ansistr(msg, str, 78);
  prints(str);
  prints("\n[1;37m  %s\n  %s\n  %s\n[0m", &pitem->buf[0][1], &pitem->buf[1][1], &pitem->buf[2][1], "");
  prints("                 [1;36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�}[m\n");
  pitem->buf[0][0] = 1;
}

void
del_pnote(notedata *pitem, int newflag) {
 char fpath[MAXPATHLEN];
 char fold[MAXPATHLEN];
 FILE *fp1, *fp2;
 notedata item;

 if (newflag)
   setuserfile(fpath, fn_note_dat);
 else
   setuserfile(fpath, fn_note_dat_save);

 sprintf(fold, "%s.tmp", fpath);
 f_mv(fpath, fold);
 if ((fp1 = fopen(fold, "r")) != NULL) {
   if ((fp2 = fopen(fpath, "w")) != NULL) {
     while (fread(&item, sizeof(item), 1, fp1) != 0) {
       if (memcmp(pitem, &item, sizeof(item))) {
         fwrite(&item, sizeof(item), 1, fp2);
       }
     }
     fclose(fp2);
   }
   fclose(fp1);
 }
 unlink(fold);
}

/*                                                              *
 *  show_pnote_hint()���showplan(), �i�Hshow�X�D�H�ۻs�������� *
 *  �d��, �Y�L�d���h�|show�w�]�d��                              *
 *                                          - add by wisely -   */

void
show_pnote_hint(uid)
  char *uid;
{
  FILE *hintfile;
  int i;
  char genbuf[256];

  sethomefile(genbuf, uid, fn_pnote_hint);
  if (hintfile = fopen(genbuf, "r"))
  {
    prints("[1;34m���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n", uid);
    i = 0;
    while (i++ < MAXHINTLINES && fgets(genbuf, 256, hintfile))
    {
      outs(Ptt_prints(genbuf));
    }
    prints("[1;34m���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n", uid);
    fclose(hintfile);
  }
  else
    prints("�z�n�A�o�O %s ���q�ܵ������A", uid);
    prints("\n�Цbť��u�͡v�n��A�}�l�d���A���¡C");
}

/*                                                              *
 *  p_edithint()���u_editplan(), �i�H�ק�D�H�d��              *
 *                                      - add by wisely         */

int
p_edithint()
{
  char genbuf[200];

  log_usies("PEDIT",NULL);
  sprintf(genbuf, "�D�H�d��(�̦h%d��) (D)�R�� (E)���s [Q]�����H[Q]", MAXHINTLINES);
  getdata(b_lines - 1, 0, genbuf, genbuf, 3, LCECHO, 0);

  if (genbuf[0] == 'e')
  {
    int aborted;

    setutmpmode(PRECORD);
    setuserfile(genbuf, fn_pnote_hint);
    aborted = vedit(genbuf, NA);
    if (!aborted)
      outs("�d�����s����");
    pressanykey(NULL);
    return 0;
  }
  else if (genbuf[0] == 'd')
  {
    setuserfile(genbuf, fn_pnote_hint);
    unlink(genbuf);
    outmsg("�d���R������");
  }
  return 0;
}


int
get_pnote(notedata *pitem, int newflag) {
 FILE *fp;
 int  total = 0;
 notedata myitem;
 char fpath[MAXPATHLEN];

 if (newflag)
   setuserfile(fpath, fn_note_dat);
 else
   setuserfile(fpath, fn_note_dat_save);

 if ((fp = fopen(fpath, "r")) != NULL) {
   while (fread(&myitem, sizeof(myitem), 1, fp) == 1) {
     memcpy(&pitem[total++], &myitem, sizeof(notedata));
   }
   fclose(fp);
   pitem[total].userid[0] = '\0';
   return total;
 }
 return 0;
}


void
Pnote(int newflag) {
  int offset, num, num1, i;
  char ans[4], prompt[STRLEN];
  notedata item_array[MAX_PNOTE + 1];
  uschar mode0 = currutmp->mode;
  char fpath[MAXPATHLEN];
  FILE *fp;

  if ((num = get_pnote(item_array, newflag)) == 0)
    return;

  setutmpmode(POWERBOOK);
  rebuild_pnote_ansi(newflag);
  offset = 1;

  while (offset <= num) {
    move(1, 0);
    clrtobot();
    prints("[1;36m�������̪�[37m%2d/%2d[36m�q%s�d��[0m\n", offset, num, newflag ? "�s��" : "");

    show_pnote(&item_array[offset - 1]);
    sprintf(prompt, "(N/P)���e/�� (A)���� (Q)�d�� (M)�s��H�c (R)�^�q %s(X)�R������ (E)���}:", newflag ? "(S)�O�d ": "(D)�R�� ");
    getdata(t_lines - 2, 0, prompt, ans, 2, DOECHO, 0);

    if (ans[0] == 'r' || ans[0] == 'R') {
        do_pnote(item_array[offset - 1].userid);
        offset++;
    }
    else if (ans[0] == 'p' || ans[0] == 'P') {
        if (offset <= 1) offset = 1;
        else offset--;
    }
    else if (ans[0] == 'q' || ans[0] == 'Q') {
        my_query(item_array[offset - 1].userid);
        offset++;
    }
    else if (ans[0] == 'a' || ans[0] == 'A') {
        for (i = 0; i <num; i++) {
          item_array[i].buf[0][0] = 0;
        }
        if (newflag) {
          setuserfile(fpath, fn_pnote_ans);
        }
        else {
          setuserfile(fpath, fn_pnote_ans_save);
        }
        more(fpath, YEA);
        break;
    }
    else if ((ans[0] == 'd' || ans[0] == 'D')&& !newflag) {
        for (i= offset-1;i<num;i++)
          memcpy(&item_array[i], &item_array[i+1], sizeof(notedata));

        num--;
        if (num == 0)
          break;
        if (offset > num)
          offset = num;
    }
    else if ((ans[0] == 's' || ans[0] == 'S') && newflag) {
        setuserfile(fpath, fn_note_dat_save);
        if((num1=get_pnote(item_array,0)) >= MAX_PNOTE) {  /* wisely */
          move(t_lines-3, 0);
          pressanykey("�������w�g���쩳�o�A�S��k�O�s�F�A�O�o�־�z��z��...");
          break;
        } else if((num1=get_pnote(item_array,0)) >= MAX_PNOTE-3) {
          move(t_lines-3, 0);
          pressanykey("�������ֺ��F, �O�o�M�z�M�z��...");
        }
	// shakalaca patch [�쥻��ܪ����e���ܦ��Ĥ@�g]
        get_pnote(item_array, 1);
        if ((fp = fopen(fpath, "a")) != NULL) {
          if (fwrite(&item_array[offset - 1], sizeof(notedata), 1, fp) != 1)
            break;

          fclose(fp);
        }
        for (i= offset-1;i<num;i++)
          memcpy(&item_array[i], &item_array[i+1], sizeof(notedata));

        num--;
        if (num == 0)
          break;
        if (offset > num)
          offset = num;

    }
    else if (ans[0] == 'm'  || ans[0] == 'M') {
        fileheader mymail;
        char title[128], buf[80];

        if (newflag)
          setuserfile(fpath, fn_pnote_ans);
        else
          setuserfile(fpath, fn_pnote_ans_save);

        sethomepath(buf, cuser.userid);
        stampfile(buf, &mymail);

        mymail.savemode = 'H';        /* hold-mail flag */
        mymail.filemode = FILE_READ;
        strcpy(mymail.owner, "[��.��.��]");
        strcpy(mymail.title, "�d��[37;41m�O��[m");
        sethomedir(title, cuser.userid);
        rec_add(title, &mymail, sizeof(mymail));
        f_mv(fpath, buf);
    }
    else if (ans[0] == 'x' || ans[0] == 'X') {
        item_array[0].userid[0] = '\0';
        break;
    }
    else if (ans[0] == 'e' || ans[0] == 'E') {
        break;
    }
    else {
        offset++;
    }
    if (offset > num)
      offset = num;
  }

  if (newflag)
    setuserfile(fpath, fn_note_dat);
  else
    setuserfile(fpath, fn_note_dat_save);

  offset = 0;
  if ((fp = fopen(fpath, "w")) != NULL) {
    while (item_array[offset].userid[0] != '\0') {
      if (newflag && item_array[offset].buf[0][0] != 0) {
        offset++;
        continue;
      }

      if (fwrite(&item_array[offset], sizeof(notedata), 1, fp) != 1)
        break;

      offset++;
    }
    fclose(fp);
  }
  rebuild_pnote_ansi(newflag);
  currutmp->mode = mode0;
  return;
}

/*                                                      *
 *  p_call(), �@��²�檺�d������, �[�bmenu���N�i�H�ΤF  *
 *  ���[�J�䥦���ǩI�\��i�Ѧ��[�J.                     *
 *                          - add by wisely 5/5/98 -    */

int
p_call() {
  char genbuf[200];
  
  log_usies("PCALL",NULL);
  stand_title("�d����.....");
  usercomplete("�A�Q�d������? ", genbuf);

  if(genbuf[0])
  {
    clear();
    move(1, 0);
    show_pnote_hint(genbuf);
    do_pnote(genbuf);
  }
  return;
}

/*
 * p_read() �O�]�p�Ψӥi�H�\�b menu ����. �u�n��L�[�b�Q�[��menu��
 * �N�i�H�ϥ��o.
 */
int
p_read() {
  char ans[4];
  char prompt[STRLEN];

  log_usies("PREAD",NULL);
  sprintf(prompt, "(N)�s���d��/(S)�Q�O�s���d�� [%c]", check_personal_note(1, NULL) ? 'N' : 'S');
  getdata(b_lines, 0, prompt, ans, 2, DOECHO, 0);
  if (ans[0] == 'n') {
    Pnote(1);
  }
  else if (ans[0] == 's') {
    Pnote(0);
  }
  else if (check_personal_note(1, NULL)) {
    Pnote(1);
  }
  else
    Pnote(0);

  return 0;
}

#include<stdarg.h>
void 
va_do_pnote(va_list pvar)
{
  char *userid;
  
  userid = va_arg(pvar, char *);
  clear();
  move(1 ,0);
  show_pnote_hint(userid);
  do_pnote(userid);
}
