/*-------------------------------------------------------*/
/* read.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : board/mail interactive reading routines      */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#include <sys/mman.h>
#define MSG_POSTER      \
COLOR2"  �峹��Ū  "COLOR1"   [33;200;442;721m(y)[37m�^�H   "HB_FIND"   [33;200;442;647m(/)[37m�j�M���D   \
[33;200;442;697m(a)[37m�j�M�@��   [33;200;442;720m(x)[37m���    [201;0m"

struct keeploc
{
  char *key;
  int top_ln;
  int crs_ln;
  struct keeploc *next;
};
typedef struct keeploc keeploc;


char currdirect[64];
static fileheader *headers = NULL;
/* static */ 
/* shakalaca.000117: unmarked for list.c */
int last_line;
static int hit_thread;

extern int search_num();
#ifdef HAVE_NEWGEM
extern char gem_mode;
#endif
#define FHSZ    sizeof(fileheader)


/* ----------------------------------------------------- */
/* cursor & reading record position control              */
/* ----------------------------------------------------- */

keeploc *
getkeep(s, def_topline, def_cursline)
  char *s;
{
  static struct keeploc *keeplist = NULL;
  struct keeploc *p;
  void *malloc();

  if (def_cursline >= 0)
     for (p = keeplist; p; p = p->next)
     {
       if (!strcmp(s, p->key))
       {
         if (p->crs_ln < 1)
           p->crs_ln = 1;
         return p;
       }
     }
  else
     def_cursline = -def_cursline;
  p = (keeploc *) malloc(sizeof(keeploc));
  p->key = (char *) malloc(strlen(s) + 1);
  strcpy(p->key, s);
  p->top_ln = def_topline;
  p->crs_ln = def_cursline;
  p->next = keeplist;
  return (keeplist = p);
}


void
fixkeep(s, first)
  char *s;
  int first;
{
  keeploc *k;

  k = getkeep(s, 1, 1);
  if (k->crs_ln >= first)
  {
    k->crs_ln = (first == 1 ? 1 : first - 1);
    k->top_ln = (first < 11 ? 1 : first - 10);
  }
}


/* calc cursor pos and show cursor correctly */

static int
cursor_pos(locmem, val, from_top)
  struct keeploc *locmem;
  int val;
  int from_top;
{
  int top;

  if (val > last_line)
  {
//    bell();
    if(HAS_HABIT(HABIT_CYCLE) || (currstat == ANNOUNCE))
      val = 1;
    else
      val = last_line;
  }
  if (val <= 0)
  {
//    bell();
    if(HAS_HABIT(HABIT_CYCLE) || (currstat == ANNOUNCE))
      val = last_line;
    else
      val = 1;
  }

  top = locmem->top_ln;
  if (val >= top && val < top + p_lines)
  {
    cursor_clear(3 + locmem->crs_ln - top, 0);
    locmem->crs_ln = val;
    cursor_show(3 + val - top, 0);
    return RC_NONE;
  }
  locmem->top_ln = val - from_top;
  if (locmem->top_ln <= 0)
    locmem->top_ln = 1;
  locmem->crs_ln = val;
  return RC_BODY;
}


static int
move_cursor_line(locmem, mode)
  keeploc *locmem;
  int mode;
{
  int top, crs;
  int reload = 0;

  top = locmem->top_ln;
  crs = locmem->crs_ln;
  if (mode == RS_PREV)
  {
    if (crs <= top)
    {
      top -= p_lines - 1;
      if (top < 1)
        top = 1;
      reload = 1;
    }
    if (--crs < 1)
    {
      crs = 1;
      reload = -1;
    }
  }
  else if (mode == RS_NEXT)
  {
    if (crs >= top + p_lines - 1)
    {
      top += p_lines - 1;
      reload = 1;
    }
    if (++crs > last_line)
    {
      crs = last_line;
      reload = -1;
    }
  }
  locmem->top_ln = top;
  locmem->crs_ln = crs;
  return reload;
}

/* ----------------------------------------------------- */
/* Tag List ����					 */
/* ----------------------------------------------------- */


int TagNum;			/* tag's number */
TagItem TagList[MAXTAGS];	/* ascending list */


int
Tagger(chrono, recno, mode )
  time_t chrono;
  int recno;
  int mode;
{
  int head, tail, posi, comp;

  for (head = 0, tail = TagNum - 1, comp = 1; head <= tail;)
  {
    posi = (head + tail) >> 1;
    comp = TagList[posi].chrono - chrono;
    if (!comp)
      break;
    else if (comp < 0)
      head = posi + 1;
    else
      tail = posi - 1;
  }

  if (mode == TAG_COMP)
  {
    if (!comp && recno)		/* �����Y�ԡG�s recno �@�_��� */
      comp = recno - TagList[posi].recno;
    return comp;
  }

  if (!comp)
  {
    if (mode != TAG_TOGGLE)
      return NA;

    TagNum--;
    memcpy(&TagList[posi], &TagList[posi + 1],
      (TagNum - posi) * sizeof(TagItem));
  }
  else if (TagNum < MAXTAGS)
  {
    TagItem *tagp, buf[MAXTAGS];

    tail = (TagNum - head) * sizeof(TagItem);
    tagp = &TagList[head];
    memcpy(buf, tagp, tail);
    tagp->chrono = chrono;
    tagp->recno = recno;
    memcpy(++tagp, buf, tail);
    TagNum++;
  }
  else
  {
    bell();
    return 0;			/* full */
  }
  return YEA;
}


void
EnumTagName(fname, locus)
  char *fname;
  int locus;
{
  sprintf(fname, "M.%d.A", TagList[locus].chrono);
}


void
EnumTagFhdr(fhdr, direct, locus)
  fileheader *fhdr;
  char *direct;
  int locus;
{
  rec_get(direct, fhdr, sizeof(fileheader), TagList[locus].recno);
}


/* -1 : ���� */
/* 0 : single article */
/* ow: whole tag list */

int
AskTag(msg)
  char *msg;
{
  char buf[80];
  int num;

  num = TagNum;
  sprintf(buf, "�� %s (A)�峹 (T)�аO (Q)�S�ơH[%c] ", msg, num ? 'T' : 'A');
  switch (getans(buf))
  {
  case 'q':
    num = -1;
    break;
  case 'a':
    num = 0;
  }
  return num;
}                       

#define	BATCH_SIZE	65536

static int
TagThread(direct, search, type)
  char *direct;
  char *search;
  int type;     /* 0: title, 1: owner */
{
  caddr_t fimage, tail;
  off_t off;

  int fd, fsize, count;
  struct stat stbuf;
  fileheader *head;
  char *title;

  if ((fd = open(direct, O_RDONLY)) < 0)
    return RC_NONE;

  fstat(fd, &stbuf);
  fsize = stbuf.st_size;

  fimage = NULL;
  off = count = 0;
  do
  {
    fimage = mmap(fimage, BATCH_SIZE, PROT_READ, MAP_SHARED, fd, off);
    if (fimage == (char *) -1)
    {
      outs("MMAP ERROR!!!!");
      abort_bbs();
    }

    tail = fimage + BMIN(BATCH_SIZE, fsize - off);

    for (head = (fileheader *) fimage; (caddr_t) head < tail; head++)
    {
      int tmplen;

      count++;

      if (type == 1)
      {
        title = head->owner;
        tmplen = IDLEN+1;
      }
      else
      {
        title = str_ttl(head->title);
        tmplen = TTLEN;
      }

      if (!strncmp(search, title, tmplen))
      {
	if (!Tagger(atoi(head->filename + 2) , count, TAG_INSERT))
	{
	  off = fsize;
	  break;
	}
      }
    }

    off += BATCH_SIZE;
// wildcat : �|�V�Y�V�h�O����?
  munmap(fimage, BATCH_SIZE);
  } while (off < fsize);
  close(fd);
  return RC_DRAW;
}


/* ----------------------------------------------------- */
/* �D�D���\Ū						 */
/* ----------------------------------------------------- */

static int
thread(locmem, stype)
  keeploc *locmem;
  int stype;
{
  static char a_ans[32], t_ans[32];
  char ans[32], s_pmt[64];
  register char *tag, *query;
  register int now, pos, match, near;
  fileheader fh;
  int circulate_flag = 1; /* circulate at end or begin */


  match = hit_thread = 0;
  now = pos = locmem->crs_ln;

/*
woju
*/
  if (stype == 'A') {
     if (!*currowner)
        return RC_NONE;
     str_lower(a_ans, currowner);
     query = a_ans;
     circulate_flag = 0;
     stype = 0;
  }
  else if (stype == 'a') {
     if (!*currowner)
        return RC_NONE;
     str_lower(a_ans, currowner);
     query = a_ans;
     circulate_flag = 0;
     stype = RS_FORWARD;
  }
  else if (stype == '/') {
     if (!*t_ans)
        return RC_NONE;
     query = t_ans;
     circulate_flag = 0;
     stype = RS_TITLE | RS_FORWARD;
  }
  else if (stype == '?') {
     if (!*t_ans)
        return RC_NONE;
     circulate_flag = 0;
     query = t_ans;
     stype = RS_TITLE;
  }
  else 

  if (stype & RS_RELATED)
  {
    tag = headers[pos - locmem->top_ln].title;
    if (stype & RS_CURRENT)
    {
      if (stype & RS_FIRST)
      {
        if (!strncmp(currtitle, tag, 40))
          return RC_NONE;
        near = 0;
      }
      query = currtitle;
    }
    else
    {
      query = str_ttl(tag);
      if (stype & RS_FIRST)
      {
        if (query == tag)
          return RC_NONE;
        near = 0;
      }
    }
  }
  else if (!(stype & RS_THREAD))
  {
    query = (stype & RS_TITLE) ? t_ans : a_ans;
    if (!*query && query == a_ans)
       if (*currowner)
          strcpy(a_ans, currowner);
       else if (*currauthor)
          strcpy(a_ans, currauthor);
    sprintf(s_pmt, "%s�j�M%s [%s] ",(stype & RS_FORWARD) ? "����":"���e",
       (stype & RS_TITLE) ? "���D" : "�@��", query);
    getdata(b_lines - 1, 0, s_pmt, ans, 30, DOECHO);
    if (*ans)
    {
      strcpy(query, ans);
    }
    else
    {
      if (*query == '\0')
        return RC_NONE;
    }
    str_lower(s_pmt, query);
    query = s_pmt;
  }

  tag = fh.owner;

  do
  {
/*
woju
*/
    if (!circulate_flag || stype & RS_RELATED)
    {
      if (stype & RS_FORWARD)
      {
        if (++now > last_line)
          return RC_NONE;
      }
      else
      {
        if (--now <= 0)
        {
          if ((stype & RS_FIRST) && (near))
          {
            hit_thread = 1;
            return cursor_pos(locmem, near, 10);
          }
          return RC_NONE;
        }
      }
    }
    else
    {
      if (stype & RS_FORWARD)
      {
        if (++now > last_line)
          now = 1;
      }
      else
      {
        if (--now <= 0)
          now = last_line;
      }
    }

    rec_get(currdirect, &fh, sizeof(fileheader), now);

    if (fh.owner[0] == '-')
      continue;

    if (stype & RS_THREAD)
    {
      if (strncasecmp(fh.title, str_reply, 3))
      {
        hit_thread = 1;
        return cursor_pos(locmem, now, 10);
      }
      continue;
    }

    if (stype & RS_TITLE)
      tag = str_ttl(fh.title);

    if (((stype & RS_RELATED) && !strncmp(tag, query, 40)) ||
      (!(stype & RS_RELATED) && ((query == currowner) ? !strcmp(tag, query) : strstr_lower(tag, query))))
    {
      if (stype & RS_FIRST)
      {
        if (tag != fh.title)
        {
          near = now;
          continue;
        }
      }
      hit_thread = 1;
      match = cursor_pos(locmem, now, 10);
      if ((!(stype & RS_CURRENT)) && (stype & RS_RELATED) &&
        strncmp(currtitle, query, 40))
      {
        strncpy(currtitle, query, 40);
        match = RC_BODY;
      }
      break;
    }
  }
  while (now != pos);

  return match;
}


/* SiE: �t�C���\Ū....�ϥ�mmap�ӥ[�t�j�M */

static int
select_read(locmem,sr_mode)
  keeploc *locmem;
  int sr_mode;
{
  register char *tag,*query;
  fileheader fh;
  char fpath[80], genbuf[512];
  char static t_ans[TTLEN+1]="";
  char static a_ans[IDLEN+1]="";
  int fd, fr, size = sizeof(fileheader);
  int fsize;
  char *fimage;
  fileheader *tail, *head;
  struct stat st;

  if( currmode & MODE_SELECT)
    return -1;
  if(sr_mode == RS_TITLE)
    query = str_ttl(headers[locmem->crs_ln - locmem->top_ln].title);
  else if (sr_mode == RS_FIRST) {
    sprintf(genbuf,"SR.%s",cuser.userid);
    if(currstat == RMAIL)
      sethomefile(fpath,cuser.userid,genbuf);
    else      
      sprintf(fpath, "boards/%s/SR.thread", currboard);
    if (stat(fpath, &st) == 0 && st.st_mtime > time(0) - 60 * 30) {
       currmode ^= (MODE_SELECT | MODE_TINLIKE);
       strcpy(currdirect,fpath);
       return st.st_size;
    }
    query = "Re:";
  }
  else if (sr_mode == RS_CURRENT)
     query = ".";
  else if (sr_mode == RS_THREAD)
     query = "m";
  else if (sr_mode == RS_SCORE)
     query = "0";
  else {
    query = (sr_mode == RS_RELATED) ? t_ans : a_ans;
    sprintf(fpath, "�j�M%s [%s] ",
        (sr_mode == RS_RELATED) ? "���D" : "�@��", query);
    if(getdata(b_lines, 0, fpath, fpath, 30, DOECHO,0))
    {
      char buf[64];
      str_lower(buf,fpath);
      strcpy(query,buf);
    }
    if(!(*query))
     return RC_NONE;
  }
  outmsg("�j�M��,�еy��...");refresh();
  if ((fd = open(currdirect,  O_RDONLY)) >= 0)
  {
    if ( !fstat(fd, &st) && S_ISREG(st.st_mode) && (fsize = st.st_size) > 0)
    {
      fimage = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
    }
    close(fd);
  }
  else fimage = (char *) -1;
  if (fimage == (char *) -1)
  {
    outmsg("�����ɶ}�ҥ���");
    return RC_NONE;
  }

  head = (fileheader *) fimage;
  tail = (fileheader *) (fimage + fsize);

  sprintf(genbuf,"SR.%s",cuser.userid);

  if(currstat==RMAIL)
    sethomefile(fpath,cuser.userid,genbuf);
  else if (sr_mode == RS_FIRST)
  {
    fileheader *hdr, fhdr, *tmp;
    int num, j, count, current;
    FILE* fp;

    num = fsize / sizeof(fileheader);
    tmp = head;
    if(currstat==RMAIL)
      sethomefile(fpath,cuser.userid,genbuf);
    else
      sprintf(fpath, "boards/%s/SR.thread", currboard);

    current = 0;
    if (fp = fopen(fpath, "w")) 
    {
      do
      {
        if (strncmp(head->title, "Re:", 3))
        {
          memcpy(&fhdr, head, sizeof(fileheader));
          hdr = tmp;
          for (count = j = 0; j < num; j++)
          {
            if (!strncmp(fhdr.title, str_ttl(hdr->title), 40))
            {
              if (j < current)	/* �e���w�����g, skip */
                break;
              ++count;
            }
            hdr++;
          }
          if (j > current)
          {
            sprintf(fhdr.date, "%5d", count);
            fwrite(&fhdr, sizeof(fileheader), 1, fp);
          }
        }
        current++;
      } while (++head < tail);
      fclose(fp);
    }                                             
    currmode ^= (MODE_SELECT | MODE_TINLIKE);
    strcpy(currdirect,fpath);
    return num;
  }
  else
    setbfile(fpath, currboard, genbuf);

  if(((fr = open(fpath,O_WRONLY | O_CREAT | O_TRUNC,0600)) != -1))
  {
    do{
      fh = *head;
      switch(sr_mode)
      {
        case RS_TITLE:
          tag = str_ttl(fh.title);
          if(!strncmp(tag, query, 40))
            write(fr,&fh,size);
          break;
        case RS_RELATED:
          tag = fh.title;
          if(str_str(tag,query))
            write(fr,&fh,size);
          break;
        case RS_AUTHOR:
          tag = fh.owner;
          if(str_str(tag,query))
            write(fr,&fh,size);
          break;
        case RS_CURRENT:
          tag = fh.owner;
          if(!strchr(tag, '.'))
            write(fr,&fh,size);
          break;
        case RS_THREAD:
          if(fh.filemode & (FILE_MARKED |  FILE_DIGEST))
            write(fr,&fh,size);
          break;
        case RS_SCORE:
         if(fh.score != 0)
           write(fr,&fh,size);
          break;
        }
      } while (++head < tail);
      fstat(fr,&st);
      close(fr);
    }
    munmap(fimage, fsize);

    if(st.st_size)
    {
      currmode ^= MODE_SELECT;
      strcpy(currdirect, fpath);
    }
  return st.st_size;
}


static int
i_read_key(rcmdlist, locmem, ch)
  struct one_key *rcmdlist;
  struct keeploc *locmem;
  int ch;
{
  int i, mode = RC_NONE;
  static thread_title;


  switch (ch)
  {
    case KEY_LEFT:
//      ch = 'q';
    case 'q':
    case 'e':
    if ((currstat == RMAIL) || (currstat == READING) || (currstat == ANNOUNCE))
    {
#ifdef HAVE_NEWGEM
      if (currstat == ANNOUNCE)
        return gem_quit() ? RC_NEWDIR : QUIT;
      else
#endif
      {
        if (thread_title) 
        {
          --thread_title;
          if (thread_title) 
          {
            sprintf(currdirect, "boards/%s/SR.thread", currboard);
            return RC_NEWDIR;
          }
        }
        return (currmode & MODE_SELECT) ? board_select() :
        (currmode & MODE_DIGEST) ? board_digest() : QUIT;
      }
    }
    else
      return QUIT;
    
  case 'k':
  case KEY_UP:
    return cursor_pos(locmem, locmem->crs_ln - 1, p_lines - 2);

  case 'j':
  case KEY_DOWN:
    return cursor_pos(locmem, locmem->crs_ln + 1, 1);

  case ' ':
  case KEY_PGDN:
  case 'N':
  case Ctrl('F'):
    if (last_line >= locmem->top_ln + p_lines)
    {
      if (last_line > locmem->top_ln + p_lines)
        locmem->top_ln += p_lines;
      else
        locmem->top_ln += p_lines - 1;
      locmem->crs_ln = locmem->top_ln;
      return RC_BODY;
    }
    cursor_clear(3 + locmem->crs_ln - locmem->top_ln, 0);
    locmem->crs_ln = last_line;
    cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
    break;

  case KEY_PGUP:
  case Ctrl('B'):
    if (locmem->top_ln > 1)
    {
      locmem->top_ln -= p_lines;
      if (locmem->top_ln <= 0)
        locmem->top_ln = 1;
      locmem->crs_ln = locmem->top_ln;
      return RC_BODY;
    }
    break;

  case KEY_END:
  case '$':
    if (last_line >= locmem->top_ln + p_lines)
    {
      locmem->top_ln = last_line - p_lines + 1;
      if (locmem->top_ln <= 0)
        locmem->top_ln = 1;
      locmem->crs_ln = last_line;
      return RC_BODY;
    }
    cursor_clear(3 + locmem->crs_ln - locmem->top_ln, 0);
    locmem->crs_ln = last_line;
    cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
    break;
 
  case '\n':
  case '\r':
  case KEY_RIGHT:
    if ((thread_title == 1) && ((currstat == RMAIL) || (currstat == READING))) 
    {
      ++thread_title;
      currmode &= ~(MODE_SELECT | MODE_TINLIKE);
      setbdir(currdirect, currboard);
      select_read(locmem,RS_TITLE);
      return RC_NEWDIR;
    } 
    ch = 'r';

  default:
    for (i = 0; rcmdlist[i].fptr; i++)
    {
      if (rcmdlist[i].key == ch)
      {
      /* shakalaca.000215: currdirect ���ثe�Ҧb root directory,
         �p�ൽ�[�ק�, mail, post, anno �N�i�X�Ӭ��@�F, �o�T�̩Үt
         �u�b�� root dir. 
         �ۦP: ��H (F), ��C (S), �j�M (/,A), �s�� (r), 
               ���� (c,C,^C,P), �Ȧs (T), �d�� (^Q),  
               ���� (�W�U���k��), tin-like read (u), 
         ����(not in mail):�s�� (E), �o�� (^p)
       */
        return (*(rcmdlist[i].fptr)) (locmem->crs_ln,
          &headers[locmem->crs_ln - locmem->top_ln], currdirect);
      }
    }
  }

  if ((currstat == RMAIL) || (currstat == READING) || (currstat == ANNOUNCE))
  {
    switch (ch)
    {
/* wildcat */
    case Ctrl('E'):
      if (HAS_PERM(PERM_SYSOP))
      {
        m_user();
        return RC_FULL;
      }
      break;

    case 'w':
      if (HAS_PERM(PERM_LOGINOK))
      {
        user_info *owneronline = (user_info *)searchowner(headers[locmem->crs_ln - locmem->top_ln].owner);

        if (owneronline == NULL)
          return RC_NONE;

        my_write(owneronline->pid, "���u Call-In�G");
        return RC_FULL;
      }
      break;

    case '/':
    case '?':
      if (select_read(locmem,RS_RELATED))
        return RC_NEWDIR;
      else
        return RC_FOOT;

    case 'S':
      if (select_read(locmem,RS_TITLE))
        return RC_NEWDIR;
      else
        return RC_FOOT;

    case 'L':
    if (currstat != ANNOUNCE)
      if (select_read(locmem,RS_CURRENT)) /* local articles */
        return RC_NEWDIR;
      else
        return RC_FOOT;

    case 'u':
    if (currstat != ANNOUNCE)
      if (!thread_title && select_read(locmem,RS_FIRST)) 
      {
        thread_title = 1;
        return RC_NEWDIR;
      }
      else 
      {
        bell();
        return RC_NONE;
      }

    case '=':
      return thread(locmem, RELATE_FIRST);

    case '\\':
      return thread(locmem, CURSOR_FIRST);
 
    /* quick search title forword */
    case ']':
      return thread(locmem, RELATE_NEXT);

    case '+':
      return thread(locmem, CURSOR_NEXT);

    /* quick search title backword */
    case '[':
      return thread(locmem, RELATE_PREV);

    case '-':
      return thread(locmem, CURSOR_PREV);

    case '<':
    case ',':
      return thread(locmem, THREAD_PREV);

    case '.':
    case '>':
      return thread(locmem, THREAD_NEXT);

    case 'F':
    case 'U':
      if (HAS_PERM(PERM_FORWARD))
      {
        char fname[512];
        fileheader *fhdr = &headers[locmem->crs_ln - locmem->top_ln];
       
        setdirpath(fname, currdirect, fhdr->filename);
#ifdef HAVE_NEWGEM
        if (gem_perm(fname, fhdr) < 1)
        {
          pressanykey("�S����H�v��");
          break;
        }
#endif        
        if (dashf(fname))
          mail_forward(&headers[locmem->crs_ln - locmem->top_ln],
            currdirect, ch == 'U');
        return RC_FULL;
      }
      break;

    case Ctrl('Q'):
       return my_query(headers[locmem->crs_ln - locmem->top_ln].owner);

#ifdef HAVE_NEWGEM
    case 't':
      if ((currmode & MODE_TAG) && (currmode & MODE_BOARD))
      {
        int now;
        fileheader *fhdr = &headers[locmem->crs_ln - locmem->top_ln];
        
        fhdr->filemode ^= FILE_TAGED;
        now = getindex(currdirect, fhdr->filename);
        substitute_record(currdirect, fhdr, sizeof(fileheader), now);
        return POS_NEXT;
      }
      else 
      {
        if (gem_mode & GEM_TAG)
          pressanykey("�p�������O���峹, �Х��� Ctrl-C �N�O�����ҧR��");
        else if (Tagger(atol(headers[locmem->crs_ln - locmem->top_ln].filename + 2), locmem->crs_ln, TAG_TOGGLE))
          return POS_NEXT;
      }
      return RC_NONE; 

    case 'l':	/* �N '*' �ର 't' */
      if (currmode & MODE_BOARD)
      {
        char genbuf[80];
        
        sprintf(genbuf, "(1)��w * �аO (2)�ϥ� %c �аO [Q]uit ?",
          (currmode & MODE_TAG) ? '*' : 't');
        switch(getans(genbuf))
        {
        case '1':
          if (TagNum > 0)
          {
            int locus = 0, now;
            fileheader fh;
  
            do
            {
              EnumTagFhdr(&fh, currdirect, locus);
              locus++;
              fh.filemode |= FILE_TAGED;
              now = getindex(currdirect, fh.filename);
              substitute_record(currdirect, &fh, sizeof(fileheader), now);
            } while (locus < TagNum);
            return RC_CHDIR;
          }
          return RC_FOOT;
          
        case '2':
          currmode ^= MODE_TAG;
        
        default:
          return RC_FOOT;
        }
      }
      return RC_NONE;
      
    case Ctrl('C'):
      if (TagNum)
      {
        TagNum = 0;
        gem_mode &= ~GEM_TAG;
        return RC_DRAW;
      }
      return RC_NONE;  
#endif

    case 'V':
      if (currstat != ANNOUNCE)
        DL_func("SO/vote.so:b_vote");
      return RC_FULL;

    case 'R':
      if (currstat != ANNOUNCE)
        DL_func("SO/vote.so:b_results");
      return RC_FULL;

    case Ctrl('X'):		/* terminator */
      if ((currstat == READING) && (HAS_PERM(PERM_ALLBOARD)) && (currstat != ANNOUNCE))
      {
        char buf[512], ans[4],mode[3];
        boardheader *bp;
        extern boardheader *bcache;
        extern int numboards;
  
        getdata(b_lines, 0,"�R���� (1)�@�� (2)���D (q)����",mode,2,LCECHO,0);
        if(mode[0] == '1')
          sprintf(buf, "�׵��Ҧ��ݪO���� [%.40s] ��(Y/N)�H[N] ",
            headers[locmem->crs_ln - locmem->top_ln].owner);
        else if(mode[0] == '2')
          sprintf(buf, "�׵��Ҧ��ݪO���� [%.40s] ��(Y/N)�H[N] ", 
            headers[locmem->crs_ln - locmem->top_ln].title);
        else
          return RC_FOOT; 
        getdata(b_lines, 0, buf, ans, 4, LCECHO,0);
        if (ans[0] != 'y')
          return RC_FOOT;
  
        resolve_boards();
        for (bp = bcache, i = 0; i < numboards; i++, bp++)
        {
          TagNum = 0;
          setbdir(buf, bp->brdname);
          outmsg(buf);
          refresh();
          if(mode[0] != '1') 
            TagThread(buf,headers[locmem->crs_ln - locmem->top_ln].title, 0);
          else
            TagThread(buf,headers[locmem->crs_ln - locmem->top_ln].owner, 1);
          if (TagNum)
          {
            delete_range2(buf, 0, 0); 
          }
        }
        if(mode[0] != '1') 
          log_usies("SPAM title ", currtitle);
        else
          log_usies("SPAM user", currtitle);
        TagNum = 0;
        return RC_CHDIR;
      }
      return RC_NONE;
    }
  }

  switch(ch)
  {
  case 'A':
  case 'a':
    if (select_read(locmem,RS_AUTHOR))
      return RC_NEWDIR;
    else
      return RC_FOOT;
  case 'p':
    return cursor_pos(locmem, locmem->crs_ln - 1, p_lines - 2);

  case 'n':
    return cursor_pos(locmem, locmem->crs_ln + 1, 1);

  case 'X':
    if (select_read(locmem,RS_SCORE))
      return RC_NEWDIR;
    else
      return RC_FOOT;

  case 'G':
    if (select_read(locmem,RS_THREAD)) /* marked articles */
      return RC_NEWDIR;
    else
      return RC_FOOT;

  case 'P':
    if (locmem->top_ln > 1)
    {
      locmem->top_ln -= p_lines;
      if (locmem->top_ln <= 0)
        locmem->top_ln = 1;
      locmem->crs_ln = locmem->top_ln;
      return RC_BODY;
    }
    break;

  }
  return mode;
}


void
i_read(cmdmode, direct, dotitle, doentry, rcmdlist, num_record)
  char *direct;
  void (*dotitle) ();
  void *(*doentry) ();
  int *num_record;
struct one_key *rcmdlist;
{
  keeploc *locmem;
  int recbase, mode, ch;
  int num, entries;
  int i;
  int jump = 0;
  char genbuf[4];
  char currdirect0[64];
  int last_line0 = last_line;
  int hit_thread0 = hit_thread;
  fileheader* headers0 = headers;
  strcpy(currdirect0 ,currdirect);

  headers = (fileheader *) calloc(p_lines, FHSZ);
  strcpy(currdirect, direct);
  mode = RC_NEWDIR;

  do
  {
    /* -------------------------------------------------------- */
    /* �̾� mode ��� fileheader                                 */
    /* -------------------------------------------------------- */

    setutmpmode(cmdmode);

    switch (mode)
    {
    case RC_NEWDIR:             /* �Ĥ@�����J���ؿ� */
    case RC_CHDIR:
      last_line = rec_num(currdirect, FHSZ);
      if (num_record != NULL)
	{
         *num_record = last_line;
	 num_record = NULL;
	}
      if (mode == RC_NEWDIR)
      {
        if (last_line == 0)
        {
          if (currstat == VOTING)
          {
            if (currmode & MODE_BOARD)
            {
              getdata(b_lines - 1, 0, "�|�����벼 (V)�|��벼 (Q)���}�H[Q] ",
                genbuf, 4, LCECHO, 0);
              if (genbuf[0] == 'v')
                DL_func("SO/vote.so:make_vote");
            }
            else
            {
              pressanykey("�|�����벼");
            }
            goto return_i_read;
          }                       
#ifdef HAVE_NEWGEM
          else if (currstat == ANNOUNCE)
          {
            if (!gem_none())	/* shakalaca.000525: �� i_read �����I.. :X
            			   �o�Υt�@�� function �����X�ź�ذϪ��ˤl.. :p */
              goto return_i_read;
          }                                                                
#endif
          else if (currstat == LISTMAIN)
          {
            getdata(1, 0, "�|�����W�� (N)�s�W (Q)���}�H[Q] ",
              genbuf, 4, LCECHO, 0);
            if (genbuf[0] == 'n')
              list_add();
            goto return_i_read;
          }                       
          else if (currstat == LISTEDIT)
          {
            getdata(1, 0, "(N)�s�W (Q)���}�H[Q] ",
              genbuf, 4, LCECHO, 0);
            if (genbuf[0] == 'n')
              list_add();
            goto return_i_read;
          }                       
          else if (currmode & MODE_DIGEST)
          {
            board_digest(); /* Kaede */
            pressanykey("�|��������K");
          }
          else if (currmode & MODE_SELECT)
          {
            board_select(); /* Leeym */
            pressanykey("�S�����t�C���峹");
          }
          else if (curredit & EDIT_MAIL)
          {
            pressanykey("�S���ӫH");
            goto return_i_read;
          }
          else
          {
            getdata(b_lines - 1, 0, "�ݪO�s���� (P)�o��峹 (Q)���}�H[Q] ",
              genbuf, 4, LCECHO,0);
            if (genbuf[0] == 'p')
              do_post();
            goto return_i_read;
          }
        }
        num = last_line - p_lines + 1;
        locmem = getkeep(currdirect, num < 1 ? 1 : num, last_line);
      }
      recbase = -1;

    case RC_FULL:
      (void) (*dotitle) ();

    case RC_BODY:
      if (last_line < locmem->top_ln + p_lines)
      {
        num = rec_num(currdirect, FHSZ);

        if (last_line != num)
        {
          last_line = num;
          recbase = -1;
        }
      }

      if (last_line == 0)
         goto return_i_read;
      else if (recbase != locmem->top_ln)
      {
        recbase = locmem->top_ln;
        if (recbase > last_line)
        {
          recbase = last_line - p_lines >> 1;
          if (recbase < 1)
            recbase = 1;
          locmem->top_ln = recbase;
        }
        entries = get_records(currdirect, headers, FHSZ, recbase, p_lines);

      }
      if (locmem->crs_ln > last_line)
        locmem->crs_ln = last_line;
      move(3, 0);
      clrtobot();

    case RC_DRAW:
      move(3, 0);
      for (i = 0; i < entries; i++)
        (*doentry) (locmem->top_ln + i, &headers[i]);

    case RC_FOOT:
    if ((currstat == RMAIL) || (currstat == READING))
      outmsg(curredit & EDIT_MAIL ? msg_mailer : MSG_POSTER);
    else
    {
      move(b_lines, 0);
      clrtoeol();
    }
/*
woju
*/
      break;
    case RS_PREV:
    case RS_NEXT:
    case RELATE_PREV:
    case RELATE_NEXT:
    case RELATE_FIRST:
    case POS_NEXT:
    case 'A':
    case 'a':
    case '/':
    case '?':
      jump = 1;
      break;
    }

    /* -------------------------------------------------------- */
    /* Ū����L�A�[�H�B�z�A�]�w mode                             */
    /* -------------------------------------------------------- */

    if (!jump) {
       cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
       ch = igetkey();
       mode = RC_NONE;
    }
    else
       ch = ' ';

    if (mode == POS_NEXT) {
       mode = cursor_pos(locmem, locmem->crs_ln + 1, 1);
       if (mode == RC_NONE)
          mode = RC_DRAW;
       jump = 0;
    }
    else if (ch >= '0' && ch <= '9')
    {
      if ((i = search_num(ch, last_line)) != -1)
        mode = cursor_pos(locmem, i + 1, 10);
    }
    else
    {
      if (!jump)
         mode = i_read_key(rcmdlist, locmem, ch);

      while (mode == RS_NEXT || mode == RS_PREV ||
        mode == RELATE_FIRST || mode == RELATE_NEXT || mode == RELATE_PREV ||
        mode == THREAD_NEXT || mode == THREAD_PREV
          || mode == 'A' || mode == 'a' || mode == '/' || mode == '?')
      {
        int reload;

        if (mode == RS_NEXT || mode == RS_PREV)
        {
          reload = move_cursor_line(locmem, mode);
        }
        else
        {
          reload = thread(locmem, mode);
          if (!hit_thread)
          {
            mode = RC_FULL;
            break;
          }
        }

        if (reload == -1)
        {
          mode = RC_FULL;
          break;
        }
        else if (reload)
        {
          recbase = locmem->top_ln;
          entries = get_records(currdirect, headers, FHSZ, recbase, p_lines);
          if (entries <= 0)
          {
            last_line = -1;
            break;
          }
        }
        num = locmem->crs_ln - locmem->top_ln;
        if (headers[num].owner[0] != '-')
          mode = i_read_key(rcmdlist, locmem, ch);
      }
    }
  } while (mode != QUIT);

#undef  FHSZ
return_i_read:
   free(headers);
   last_line = last_line0;
   hit_thread = hit_thread0;
   headers = headers0;
   strcpy(currdirect ,currdirect0);
   return;
}
