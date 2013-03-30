/*-------------------------------------------------------*/
/* menu.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : menu/help/movie routines                     */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"

/* ----------------------------------------------------- */
/* Menu Commands struct                                  */
/* ----------------------------------------------------- */

struct MENU
{
  void *cmdfunc;
//  int (*cmdfunc) ();
  usint level;
  char *desc;                   /* next/key/description */
  int mode;
};
typedef struct MENU MENU;


/* ------------------------------------- */
/* help & menu processring               */
/* ------------------------------------- */
int refscreen = NA;
extern char *boardprefix;


void
showtitle(title, mid)
  char *title, *mid;
{
  char buf[40];
  int spc, pad;

  spc = strlen(mid);

  if (title[0] == 0)
    title++;
   else if (chkmail(0))
  {
    mid = "\033[41;33;5m   �H�c�̭����s�H��I  \033[m\033[1m"COLOR1;

/*
 * CyberMax:
 *           spc �O�ǰt mid ���j�p.
 */
    spc = 22;
  }
  else if(check_personal_note(1,NULL))
  {
    mid = "\033[43;37;5m    �����������d����   \033[m\033[1m"COLOR1;
    spc = 22;
  }
  else if (dashf(BBSHOME"/register.new") && HAS_PERM(PERM_ACCOUNTS))
  {
    mid = "\033[45;33;5m  ���s���ϥΪ̵��U�o!  \033[m\033[1m"COLOR1;
    spc = 22;
  }

  spc = 66 - strlen(title) - spc - strlen(currboard);
/*
woju
*/
  if (spc < 0)
     spc = 0;
  pad = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

  move(0,0);
  clrtobot();
//  clear();
  prints(COLOR2"  \033[1;36m%s  "COLOR1"%s\033[33m%s%s%s\033[3%s\033[1m "COLOR2"  \033[36m%s  \033[m\n",
    title, buf, mid, buf, " " + pad,
    currmode & MODE_SELECT ? "1m�t�C" :
    currmode & MODE_DIGEST ? "5m��K" : "7m�ݪO", currboard);

}

// wildcat : ����u�Ϊ��ſ�� :p
int
null_menu()
{
  pressanykey("�o�O�@�Ӫſ�� :p ");
  return 0;
}


/* ------------------------------------ */
/* �ʵe�B�z                              */
/* ------------------------------------ */


#define FILMROW 11
unsigned char menu_row = 3;
unsigned char menu_column = 4;
char mystatus[512];


/* wildcat 1998.8.7 */

void
movie(i)
  int i;
{
  extern struct FILMCACHE *film;
  static short history[MAX_HISTORY];
  static char myweek[] = "��@�G�T�|����";
  static char buf[512],pbuf[512];
  char *msgs[] = {"��", "�}", "��", "��","��"};
  time_t now = time(NULL);
  struct tm *ptime = localtime(&now);

  resolve_garbage(); /* get film cache */

  if (currstat == GAME) return;
  if (HAVE_HABIT(HABIT_MOVIE))
  {
    if((!film->busystate) && film->max_film) /* race condition */
    {
      do{
        if (i != 1 || i != 0 || i != 999)
          i = 1 + (rand()%film->max_film);

        for (now = film->max_history; now >= 0; now--)
        if (i == history[now])
        {
          i = 0;
          break;
        }
      } while (i == 0);
    }
//    else i = 1;
    memcpy(history, &history[1], film->max_history * sizeof(short));
    history[film->max_history] = now = i;

    if (i == 999)       /* Goodbye my friend */
      i = 0;
    setapath(pbuf, "Note");
    sprintf(buf, "%s/%s", pbuf, film->notes[i]);
    if(film->notes[i][0])
      show_file(buf,13,FILMROW,NO_RELOAD);
  }
  i = ptime->tm_wday << 1;
  update_data();
  sprintf(mystatus, "\033[1;36m[%d:%02d %c%c %0d/%0d]"
"\033[1;37m ID: %-13s �C\033[37m%6d%c,\033[33m%5d%c"
"\033[32m[�]]%-2.2s \033[35m[%-20.20s]\033[m",
    ptime->tm_hour, ptime->tm_min, myweek[i], myweek[i + 1],
    ptime->tm_mon + 1, ptime->tm_mday, cuser.userid, 
    (cuser.silvermoney/1000) <= 0 ? cuser.silvermoney : cuser.silvermoney/1000,
    (cuser.silvermoney/1000) <= 0 ? ' ' : 'k',
    (cuser.goldmoney/1000) <= 0 ? cuser.goldmoney : cuser.goldmoney/1000,
    (cuser.goldmoney/1000) <= 0 ? ' ' : 'k',    
    msgs[currutmp->pager],
    currutmp->birth ? "�ͤ�O�o�n�Ыȭ�!!" : film->today_is);
  move(1,0);
  clrtoeol();
  outs(mystatus);
  refresh();
}


/* ===== end ===== */

static int
show_menu(p)
  MENU *p;
{
  register int n = 0, m = 0;
  int i = 0;
  register char *s;
          char buf[512],fn[512];
  char buf2[11][512] = {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};
//  char buf2[][];
  FILE *fp;
#ifdef HAVE_NOTE_2
  sprintf(fn,BBSHOME"/m2/%d",(rand()%10)+1);
  if(fp = fopen(fn,"r"))
  {
    while(fgets(buf2[i],512,fp) != NULL)
      i++;
    fclose(fp);
  }
#endif
  movie(0);
  move(2,0);
#ifdef HYPER_BBS
  prints(COLOR1"\033[1m[200;442;506;612m[��]�W�@��[201m �\\ ��       ��    ��                    [1;33m[200;442;626m[Ctrl-Z][201m   [200;442;621m[Ctrl-U][201m         [m");
#else
  prints(COLOR1"\033[1m         �\\  ��        ��    ��                 �� [\033[1;33mCtrl-Z\033[37m] \033[31m�D�U               \033[m");
#endif
  move(menu_row, 0);
  while ((s = p[n].desc)!=NULL || buf2[m][0]!='\0')
  {
    if ( s != NULL )
    {
      if (HAS_PERM(p[n].level))
      {
        sprintf(buf,s+2);
#ifdef HAVE_NOTE_2
//        if(currstat == FINANCE || currstat == GAME || currstat == RMENU
	if(currstat == FINANCE || currstat == GAME || currstat == NETGAME
          || buf2[m][0]=='\0' )
#endif
  #ifdef HYPER_BBS
          prints("%*s  \033[200m\033[446m[\033[1;36m\033[300m%c\033[302m\033[m]%s\033[201m\n", 
  #else
          prints("%*s  [\033[1;36m%c\033[m]%s\n", 
  #endif
            menu_column, "", s[1], buf);
#ifdef HAVE_NOTE_2
        else
  #ifdef HYPER_BBS
          prints("%*s  \033[200m\033[446m[\033[1;36m\033[300m%c\033[302m\033[m]%-28s\033[201m%s",
  #else
          prints("%*s  [\033[1;36m%c\033[m]%-28s%s",
  #endif
            menu_column, "", s[1], buf,buf2[m++]);
#endif
      }
      n++;
    }
#ifdef HAVE_NOTE_2
    else
    {
//      if (currstat == FINANCE || currstat == GAME || currstat == RMENU)
	if (currstat == FINANCE || currstat == GAME || currstat == NETGAME)
        break;
      prints("%37s%-s", "", buf2[m++] );
    }
#endif
  }
  return n - 1;
}


void
domenu(cmdmode, cmdtitle, cmd, cmdtable)
  char *cmdtitle;
  int cmdmode, cmd;
  MENU *cmdtable;
{

  int lastcmdptr;
  int n, pos, total, i;
  int err;
  int chkmailbox();
  static char cmd0[LOGIN];

  if (cmd0[cmdmode])
     cmd = cmd0[cmdmode];

  setutmpmode(cmdmode);
  sprintf(tmpbuf,"%s [�u�W %d �H]",BOARDNAME,count_ulist());

  showtitle(cmdtitle, tmpbuf);
  total = show_menu(cmdtable);
  move(1,0);
  outs(mystatus);

  lastcmdptr = pos = 0;

  do
  {
    i = -1;

    switch (cmd)
    {
    case KEY_ESC:
       if (KEY_ESC_arg == 'c')
          capture_screen();
       else if (KEY_ESC_arg == 'n') {
          edit_note();
          refscreen = YEA;
       }
       i = lastcmdptr;
       break;
    case Ctrl('N'):
       New();
       refscreen = YEA;
       i = lastcmdptr;
       break;
    case Ctrl('A'):
    {
      int stat0 = currstat;
      currstat = RMAIL;
      if (man() == RC_FULL)
        refscreen = YEA;
      i = lastcmdptr;
      currstat = stat0;
      break;
    }
    case KEY_DOWN:
      i = lastcmdptr;

    case KEY_HOME:
    case KEY_PGUP:
      do
      {
        if (++i > total)
          i = 0;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_END:
    case KEY_PGDN:
      i = total;
      break;

    case KEY_UP:
      i = lastcmdptr;
      do
      {
        if (--i < 0)
          i = total;
      } while (!HAS_PERM(cmdtable[i].level));
      break;

    case KEY_LEFT:
    case 'e':
    case 'E':
      if (cmdmode == MMENU)
        cmd = 'G';
      else if ((cmdmode == MAIL) && chkmailbox())
        cmd = 'R';
      else return;
    default:
       if ((cmd == Ctrl('G') || cmd == Ctrl('S')) && (currstat == MMENU || currstat == TMENU || currstat == XMENU))  {
          if (cmd == Ctrl('S'))
             ReadSelect();
//          else if (cmd == Ctrl('G'))
//            Read();
          refscreen = YEA;
          i = lastcmdptr;
          break;
        }
      if (cmd == '\n' || cmd == '\r' || cmd == KEY_RIGHT)
      {

        boardprefix = cmdtable[lastcmdptr].desc;

        if(cmdtable[lastcmdptr].mode && DL_get(cmdtable[lastcmdptr].cmdfunc))
        {
          void *p = (void *)DL_get(cmdtable[lastcmdptr].cmdfunc);
          if(p) cmdtable[lastcmdptr].cmdfunc = p;
          else break;
        }

        currstat = XMODE;

        {
          int (*func)() = 0;

          func = cmdtable[lastcmdptr].cmdfunc;
          if(!func) return;
          if ((err = (*func)()) == QUIT)
            return;
        }

        currutmp->mode = currstat = cmdmode;

        if (err == XEASY)
        {
          refresh();
          sleep(1);
        }
        else if (err != XEASY + 1 || err == RC_FULL)
          refscreen = YEA;

        if (err != -1)
          cmd = cmdtable[lastcmdptr].desc[0];
        else
          cmd = cmdtable[lastcmdptr].desc[1];
        cmd0[cmdmode] = cmdtable[lastcmdptr].desc[0];
      }

      if (cmd >= 'a' && cmd <= 'z')
        cmd &= ~0x20;
      while (++i <= total)
      {
        if (cmdtable[i].desc[1] == cmd)
          break;
      }
    }

    if (i > total || !HAS_PERM(cmdtable[i].level))
    {
      continue;
    }

    if (refscreen)
    {
      showtitle(cmdtitle, tmpbuf);
      show_menu(cmdtable);
      move(1,0);
      outs(mystatus);
      refscreen = NA;
    }

    cursor_clear(menu_row + pos, menu_column);
    n = pos = -1;
    while (++n <= (lastcmdptr = i))
    {
      if (HAS_PERM(cmdtable[n].level))
        pos++;
    }
    cursor_show(menu_row + pos, menu_column);
  } while (((cmd = igetkey()) != EOF) || refscreen);

  abort_bbs();
}
/* INDENT OFF */


/* ----------------------------------------------------- */
/* administrator's maintain menu                         */
/* ----------------------------------------------------- */

int m_user(), m_newbrd(), m_board(), m_register(),x_reg(),XFile(),
    search_key_user(),search_bad_id,reload_cache(),adm_givegold();
/*    ,search_bad_id();*/

#ifdef  HAVE_MAILCLEAN
int m_mclean();
#endif

static struct MENU adminlist[] = {
  m_user,       PERM_ACCOUNTS,  "UUser          [�ϥΪ̸��]",0,
  search_key_user,PERM_ACCOUNTS,"FFind User     [�j�M�ϥΪ�]",0,
  m_newbrd,     PERM_BOARD,     "NNew Board     [�}�ҷs�ݪO]",0,
  m_board,      PERM_BOARD,     "SSet Board     [ �]�w�ݪO ]",0,
  m_register,   PERM_ACCOUNTS,  "RRegister      [�f�ֵ��U��]",0,
  XFile,        PERM_SYSOP,     "XXfile         [�ק�t����]",0,
  reload_cache, PERM_SYSOP,     "CCache Reload  [ ��s���A ]",0,
  adm_givegold, PERM_SYSOP,     "GGive $$       [ �o����� ]",0,
//  adm_edit_m2,  PERM_SYSOP,     "SSet Board2    [�s��p�ݪO]",0,
//"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet        [ �s�u�u�� ]",1,
/*
  m_mclean, PERM_BBSADM, "MMail Clean    �M�z�ϥΪ̭ӤH�H�c",0,
#endif */
  x_reg,        PERM_ACCOUNTS,  "MMerge         [�f�֭ײz��]",0,
"SO/xyz.so:x_innbbsd",PERM_SYSOP,"IInnbbsd       [ ��s��H ]",1,
NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

int board(), local_board(), Boards(), ReadSelect() ,
    New(),Favor(),favor_edit(),good_board(),voteboard();

static struct MENU classlist[] = {
//   voteboard, 0,      "VVoteBoard    [�ݪO�s�p�t��]",0,
//   board, 0,          "CClass        [���������ݪO]",0,
  board, 0,		"00[1;37m���Ⱥ޲z�B  [0;37m[[1;36m���i,����,����[0;37m][m",0,
  board, 0,		"11[1;37m�p�X�Ǽ���  [0;37m[[1;33m�ն�,����,�Z��[0;37m][m",0,
  board, 0,		"22[1;37m��ƴϮ��a  [0;37m[[1;36m�ǳN,����,����[0;37m][m",0,
  board, 0,		"33[1;37m���w���x��  [0;37m[[1;33m����,�s�D,�ӤH[0;37m][m",0,
  board, 0,		"44[1;37m�𶢹B�ʳ�  [0;37m[[1;36m�q��,�T��,�B��[0;37m][m",0,
  board, 0,		"55[1;37m�q�����Ѻ�  [0;37m[[1;33m�n��,�w��,����[0;37m][m",0,
   New, 0,            "NNew          [�Ҧ��ݪO�C��]",0,
//   local_board,0,     "LLocal        [�����ݪO�C��]",0,
//   good_board,0,      "GGoodBoard    [  �u�}�ݪO  ]",0,
   Favor,PERM_BASIC,  "BBoardFavor   [�ڪ��̷R�ݪO]",0,
favor_edit,PERM_BASIC,"FFavorEdit    [�s��ڪ��̷R]",0,
   ReadSelect, 0,     "SSelect       [  ��ܬݪO  ]",0,
   NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* RPG menu                                             */
/* ----------------------------------------------------- */
int /* t_pk(),*/rpg_help();

struct MENU rpglist[] = {
  rpg_help,0,	"HHelp       ���C������/�W�h����",0,
  "SO/rpg.so:rpg_uinfo",
    0,		"UUserInfo   �ۤv�����A",1,
  "SO/rpg.so:rpg_race_c",
    PERM_BASIC,	"JJoin       �[�J¾�~�u�|(�ݤ��T����)",1,
  "SO/rpg.so:rpg_guild",
    0,		"GGuild      ¾�~�u�|",1,
  "SO/rpg.so:rpg_train",
    0,		"TTrain      �V�m��(�I�u��)",1,
  "SO/rpg.so:rpg_top",
    0,		"LListTop    �ϥΪ̱Ʀ�]",1,
  "SO/rpg.so:rpg_edit",
    PERM_SYSOP,	"QQuery      �d�߭ק�USER���",1,
//  "SO:rpg.so:rpg_shop",0,           "SShop       �˳ưө�(�I�u��)",1,
//  "SO:rpg.so:t_pk",0,                 "PPK         ���� PK",1,
NULL, 0, NULL,0};

int
rpg_menu()
{
  domenu(RMENU, "�����t�C��", 'U', rpglist);
  return 0;
}
/* ----------------------------------------------------- */
/* Ptt money menu                                        */
/* ----------------------------------------------------- */

static struct MENU finlist[] = {
  "SO/buy.so:bank",     0,      "11Bank           \033[1;36m�L�ƻȦ�\033[m",1,
  "SO/pip.so:pip_money",0,      "22ChickenMoney   �����I���B     �������q�l����",1,
  "SO/song.so:ordersong",0,     "33OrderSong      \033[1;33m�L���I�q��\033[m     $5g/��",1,
  "SO/buy.so:p_cloak",  0,      "44Cloak          �{������/�{��  $2g/��     (�{���K�O)",1,
  "SO/buy.so:p_from",   0,      "55From           �ק�G�m       $5g/��",1,
  "SO/buy.so:p_exmail", 0,      "66Mailbox        �ʶR�H�c�W��   $1000g/��",1,
  "SO/buy.so:p_fcloak", 0,      "77UltimaCloak    �׷������j�k   $10000g      �i�ä[����",1,
  "SO/buy.so:p_ffrom",  0,      "88PlaceBook      �G�m�ק��_��   $50000g     User�W���F�i��G�m",1,
//  "SO/buy.so:p_ulmail", 0,      "99NoLimit        �H�c�L�W��     $1000000g   �H�c�W���L����",1,
NULL, 0, NULL,0};

int
finance()
{
  domenu(FINANCE, "���Ĥ���", '1', finlist);
  return 0;
}

#ifdef HAVE_GAME

/* ----------------------------------------------------- */
/* NetGame menu                                          */
/* ----------------------------------------------------- */

struct MENU netgame_list[] = {
  "SO/xyz.so:x_mj",0,
    "QQkmj      �� �����±N��",1,
  "SO/xyz.so:x_big2",0,
    "BBig2      �� �����j�ѤG",1,
//  "SO/xyz.so:x_chess",PERM_LOGINOK,
//    "CChess     �� �����U�H��",1,
NULL, 0, NULL,0};

int
netgame_menu()
{
  domenu(NETGAME, "�����s�u�C��", 'Q', netgame_list);
  return 0;
}
/* ----------------------------------------------------- */
/* Game menu                                             */
/* ----------------------------------------------------- */

static struct MENU gamelist[] = {
//  rpg_menu,0,
//    "RRPG        �� �����t�C��           �I�u��",0,
  netgame_menu,0,
    "NNetGame    �� �����s�u�C��           $100s/��",0,
  "SO/gamble.so:ticket_main",PERM_BASIC,
    "GGamble     �� ���ֽ�L             $100s/�i",1,
  "SO/marie.so:mary_m",0,
    "MMarie      �� �p���R�ֶ�             �̧C���O $1s",1,
  "SO/gagb.so:gagb",0,
    "??A?B       �� �q�q�q�Ʀr             �̧C���O $1s",1,
  "SO/guessnum.so:fightNum",0,
    "FFightNum   �� ��Բq�Ʀr             �̧C���O $1s",1,
  "SO/bj.so:BlackJack",0,
    "JJack       �� �դ�³ǧJ             �̧C���O $1s",1,
  "SO/nine.so:p_nine",PERM_BASIC,
    "999         �� �Ѧa�[�E�E�            �̧C���O $1s",1,
  "SO/dice.so:x_dice",0,
    "DDice       �� ��K�Խ��             �̧C���O $1s",1,
  "SO/gp.so:p_gp",0,
    "PPoke       �� �����J����             �̧C���O $1s",1,
  "SO/pip.so:p_pipple",PERM_LOGINOK,
    "CChicken    �� �L�ƹq�l��             �K�O���A��!!",1,
  "SO/xyz.so:x_tetris",0,
    "TTetris     �� �Xù�����             �K�O���A��",1,
  "SO/mine.so:Mine",0,
    "LLandMine   �� �l�z��a�p             �C��100s",1,
  "SO/poker.so:p_dragon",0,
    "11���s      �� ���դ������s           �K�O���A��",1,
  "SO/chessmj.so:p_chessmj",0,
    "22ChessMJ   �� �H�ѳ±N               �̧C���O $1s",1,
  "SO/seven.so:p_seven",0,
    "33Seven     �� �䫰�C�i               �̧C���O $1s",1,
  "SO/bet.so:p_bet",0,
    "44Bet       �� �ƨg��L               �̧C���O $1s",1,
  "SO/lotto.so:menu_lotto",0,
    "55Lotto     �� �L�Ƽֳzñ�䯸         �̧C���O $1g",1,
  "SO/lo.so:q_lotto",PERM_NOTOP,
    "66Lotto     �� �]�P�ֳzñ�䯸 ",1,
  "SO/stock.so:p_stock",PERM_BASIC,
    "SStock      �� �L�ƪѥ�",1,

/*  x_bridgem,PERM_LOGINOK,"OOkBridge    �i ���P�v�� �j",0,*/
NULL, 0, NULL,0};
#endif

/* ----------------------------------------------------- */
/* Talk menu                                             */
/* ----------------------------------------------------- */

int t_users(), t_idle(), t_query(), t_pager();
// t_chat(), t_list(), t_talk();
/* Thor: for ask last call-in message */
int t_display();

static struct MENU talklist[] = {

  t_users,      0,              "LList          [�u�W�W��]",0,
  t_pager,      PERM_BASIC,     "PPager         [�������A]",0,
  t_idle,       0,              "IIdle          [��w�ù�]",0,
  t_query,      0,              "QQueryUser     [�d��User]",0,
/* t_talk,       PERM_PAGE,      "TTalk          [��H���]",0, */
 "SO/chat.so:t_chat",PERM_CHAT,"CChatRoom      [�s�u���]",1, 
  t_display,    0,              "DDisplay       [���y�^�U]",0,
  XFile,        PERM_XFILE,     "XXfile         [�ק�t����]",0,
NULL, 0, NULL,0};

/*-------------------------------------------------------*/
/* powerbook menu                                        */
/* ----------------------------------------------------- */

int null_menu(),my_gem(),my_allpost();

static struct MENU powerlist[] = {

  "SO/bbcall.so:bbcall_menu",
                       0,	"MMessager      [ �q�T�� ]",1,
  "SO/mn.so:show_mn",
	               0,	"NNoteMoney     [ �O�b�� ]",1,
  my_gem,              0,       "GGem           [�ڪ����]",0,
  my_allpost,          0,       "AAllPost       [�ڪ��峹]",0,
  null_menu,           0,	"------ ������ �\\�� ------",0,
  "SO/pnote.so:p_read",0,       "PPhone Answer  [ť���d��]",1,
  "SO/pnote.so:p_call",0,       "CCall phone    [�e�X�d��]",1,
  "SO/pnote.so:p_edithint",0,   "RRecord        [���w���]",1,

NULL, 0, NULL,0};

int
PowerBook()
{
  domenu(POWERBOOK, "�U�Τ�U", 'N', powerlist);
  return 0;
}


/* ----------------------------------------------------- */
/* User menu                                             */
/* ----------------------------------------------------- */

extern int u_editfile();
int u_info(), u_cloak(), u_list(), u_habit(), PowerBook(), ListMain();
#ifdef REG_FORM
int u_register();
#endif

#ifdef REG_MAGICKEY
int u_verify();
#endif

#ifdef POSTNOTIFY
int re_m_postnotify();
#endif

static struct MENU userlist[] = {
  PowerBook,	PERM_BASIC,	"PPowerBook     [�U�Τ�U]",0,
  u_info,       0,              "IInfo          [�ק���]",0,
  u_habit,      PERM_BASIC,     "HHabit         [�ߦn�]�w]",0,
  ListMain,     PERM_LOGINOK,   "LList          [�]�w�W��]",0, 

#ifdef POSTNOTIFY
  re_m_postnotify,PERM_BASIC,   "PPostNotify    [�f�֤峹�q��]",0,
#endif

  u_editfile,   PERM_LOGINOK,   "FFileEdit      [�ӤH�ɮ�]",0,
  u_cloak,      PERM_CLOAK,     "CCloak         [���αK�k]",0,

#ifdef REG_FORM
//  u_register,   PERM_POST,      "RRegister      [����U��]",0,
  u_register,   PERM_BASIC,      "RRegister      [����U��]",0,
#endif

#ifdef REG_MAGICKEY
  u_verify, 	PERM_BASIC,	"VVerify        [����U�X]",0,
#endif

  u_list,       PERM_BASIC,     "UUsers         [���U�W��]",0,
NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* XYZ menu                                              */
/* ----------------------------------------------------- */
#ifdef HAVE_GAME
int
game_list()
{
  domenu(GAME, "�����C�ֳ�", 'N', gamelist);
  return 0;
}
#endif

struct MENU netserv_list[] = {
"SO/bbcall.so:bbcall_main",
                PERM_LOGINOK,   "BB.B.Call      [�����ǩI]",1,
//"SO/tv.so:catv",PERM_BASIC,     "TTV-Program    [�q���`�جd��]",1,
"SO/fortune.so:main_fortune",
                PERM_BASIC,     "QQueryFortune  [�ӤH�B�չw��]",1,
"SO/railway.so:railway2",
                PERM_BASIC,     "RRailWay       [�����ɨ�d��]",1,
"SO/test.so:railway2",
                PERM_SYSOP,     "TTest          [���ե�]",1,  
#ifdef HAVE_GOPHER
  "SO/xyz.so:x_gopher",PERM_LOGINOK,"GGopher       �� �@�s�p�a�����A�� ��",1,
#endif
#ifdef BBSDOORS
  "SO/xyz.so:x_bbsnet", PERM_LOGINOK, "DDoor      �i ��L BBS�� �j",1,
#endif
NULL, 0, NULL,0};

int
net_serv()
{
  domenu(PMENU, "�����s�u�A��", 'B', netserv_list);
  return 0;
}

int note();

static struct MENU servicelist[] = {
#ifdef HAVE_GAME
  game_list,    0,              "PPlay          [�|�֤���]",0,
#endif
  finance,      PERM_LOGINOK,   "FFinance       [�ӫ~�j��]",0,
  net_serv,	0,		"SServNet       [�����A��]",0,
//  "SO/xyz.so:KoK",
//  		0,		"KKK-Service    [�U������]",1,
  "SO/vote.so:all_vote",
                PERM_LOGINOK,   "VVote          [�벼����]",1,
  note,         PERM_LOGINOK,   "NNote          [�g�d���O]",0,
  "SO/xyz.so:show_hint_message",
                0,              "HHint          [�оǺ��F]",1,

  "SO/indict.so:x_dict",
                0,              "DDictionary    [�ʬ����]",1,
/*
"SO/xyz.so:x_cdict",
                PERM_BASIC,     "CCD-67         [�q�l�r��]",1,
*/
"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet        [�s�u�u��]",1,
  NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* mail menu                                             */
/* ----------------------------------------------------- */
int m_new(), m_read(), m_send(),m_sysop(),mail_mbox(),mail_all(),
    setforward(),mail_list();

#ifdef INTERNET_PRIVATE_EMAIL
int m_internet();
#endif

static struct MENU maillist[] = {
  m_new,        PERM_READMAIL,  "RNew           [�\\Ū�s�H]",0,
  m_read,       PERM_READMAIL,  "RRead          [�H��C��]",0,
  m_send,       PERM_BASIC,     "SSend          [�����H�H]",0,
  mail_list,    PERM_BASIC,     "MMailist       [�s�ձH�H]",0,
  m_internet,   PERM_INTERNET,  "IInternet      [�����l��]",0,
  setforward,   PERM_LOGINOK,   "FForward       [���H��H]",0,
  m_sysop,      0,              "OOp Mail       [�ԴA����]",0,
  mail_mbox,    PERM_INTERNET,  "ZZip           [���]���]",0,
  mail_all,     PERM_SYSOP,     "AAll           [�t�γq�i]",0,
NULL, 0, NULL,0};



/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int
admin()
{
  domenu(ADMIN, "�����Ѥj", 'X', adminlist);
  return 0;
}

static int
BOARD()
{
  domenu(CLASS, "�ݪO�C��", 'G', classlist);
  return 0;
}

static int
Mail()
{
  domenu(MAIL, "�l����", 'R', maillist);
  return 0;
}

int
static Talk()
{
  domenu(TMENU, "��ѿ��", 'L', talklist);
  return 0;
}

static int
User()
{
  domenu(UMENU, "�ӤH�]�w", 'H', userlist);
  return 0;
}


static int
Service()
{
  domenu(PMENU, "�U�تA��", 'H', servicelist);
  return 0;
}


int Announce(), Boards(), Goodbye(),Log(),board();


struct MENU cmdlist[] = {
  admin,        PERM_ADMIN,     "00Admin        [�t�κ޲z]",0,
  Announce,     0,              "AAnnounce      [�Ѧa���]",0,
  BOARD,        0,              "BBoard         [�ݪO�\\��]",0,
  board,        0,              "CClass         [�����ݪO]",0,
  Mail,         PERM_BASIC,     "MMail          [�L�ƶl��]",0,
  Talk,         0,              "TTalk          [�ͤѻ��a]",0,
  User,         PERM_BASIC,     "UUser          [�ӤH�u��]",0,
  Log,          0,              "HHistory       [���v�y��]",0,
  Service,      PERM_BASIC,     "SService       [�U�تA��]",0,
  Goodbye,      0,              "GGoodbye       [���t�d��]",0,
NULL, 0, NULL,0};
/* INDENT ON */
