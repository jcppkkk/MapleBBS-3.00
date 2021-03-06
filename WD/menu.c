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
    mid = "\033[41;33;5m   信箱裡面有新信唷！  \033[m\033[1m"COLOR1;

/*
 * CyberMax:
 *           spc 是匹配 mid 的大小.
 */
    spc = 22;
  }
  else if(check_personal_note(1,NULL))
  {
    mid = "\033[43;37;5m    答錄機中有留言喔   \033[m\033[1m"COLOR1;
    spc = 22;
  }
  else if (dashf(BBSHOME"/register.new") && HAS_PERM(PERM_ACCOUNTS))
  {
    mid = "\033[45;33;5m  有新的使用者註冊囉!  \033[m\033[1m"COLOR1;
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
    currmode & MODE_SELECT ? "1m系列" :
    currmode & MODE_DIGEST ? "5m文摘" : "7m看板", currboard);

}

// wildcat : 分格線用的空選單 :p
int
null_menu()
{
  pressanykey("這是一個空選單 :p ");
  return 0;
}


/* ------------------------------------ */
/* 動畫處理                              */
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
  static char myweek[] = "日一二三四五六";
  static char buf[512],pbuf[512];
  char *msgs[] = {"關", "開", "拔", "防","友"};
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
"\033[1;37m ID: %-13s ＄\033[37m%6d%c,\033[33m%5d%c"
"\033[32m[β]%-2.2s \033[35m[%-20.20s]\033[m",
    ptime->tm_hour, ptime->tm_min, myweek[i], myweek[i + 1],
    ptime->tm_mon + 1, ptime->tm_mday, cuser.userid, 
    (cuser.silvermoney/1000) <= 0 ? cuser.silvermoney : cuser.silvermoney/1000,
    (cuser.silvermoney/1000) <= 0 ? ' ' : 'k',
    (cuser.goldmoney/1000) <= 0 ? cuser.goldmoney : cuser.goldmoney/1000,
    (cuser.goldmoney/1000) <= 0 ? ' ' : 'k',    
    msgs[currutmp->pager],
    currutmp->birth ? "生日記得要請客唷!!" : film->today_is);
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
  prints(COLOR1"\033[1m[200;442;506;612m[←]上一頁[201m 功\ 能       說    明                    [1;33m[200;442;626m[Ctrl-Z][201m   [200;442;621m[Ctrl-U][201m         [m");
#else
  prints(COLOR1"\033[1m         功\  能        說    明                 按 [\033[1;33mCtrl-Z\033[37m] \033[31m求助               \033[m");
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
  sprintf(tmpbuf,"%s [線上 %d 人]",BOARDNAME,count_ulist());

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
  m_user,       PERM_ACCOUNTS,  "UUser          [使用者資料]",0,
  search_key_user,PERM_ACCOUNTS,"FFind User     [搜尋使用者]",0,
  m_newbrd,     PERM_BOARD,     "NNew Board     [開啟新看板]",0,
  m_board,      PERM_BOARD,     "SSet Board     [ 設定看板 ]",0,
  m_register,   PERM_ACCOUNTS,  "RRegister      [審核註冊單]",0,
  XFile,        PERM_SYSOP,     "XXfile         [修改系統檔]",0,
  reload_cache, PERM_SYSOP,     "CCache Reload  [ 更新狀態 ]",0,
  adm_givegold, PERM_SYSOP,     "GGive $$       [ 發放獎金 ]",0,
//  adm_edit_m2,  PERM_SYSOP,     "SSet Board2    [編輯小看板]",0,
//"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet        [ 連線工具 ]",1,
/*
  m_mclean, PERM_BBSADM, "MMail Clean    清理使用者個人信箱",0,
#endif */
  x_reg,        PERM_ACCOUNTS,  "MMerge         [審核修理機]",0,
"SO/xyz.so:x_innbbsd",PERM_SYSOP,"IInnbbsd       [ 更新轉信 ]",1,
NULL, 0, NULL,0};


/* ----------------------------------------------------- */
/* class menu                                            */
/* ----------------------------------------------------- */

int board(), local_board(), Boards(), ReadSelect() ,
    New(),Favor(),favor_edit(),good_board(),voteboard();

static struct MENU classlist[] = {
//   voteboard, 0,      "VVoteBoard    [看板連署系統]",0,
//   board, 0,          "CClass        [本站分類看板]",0,
  board, 0,		"00[1;37m站務管理處  [0;37m[[1;36m公告,站長,站務[0;37m][m",0,
  board, 0,		"11[1;37m聯合傳播網  [0;37m[[1;33m校園,社團,班版[0;37m][m",0,
  board, 0,		"22[1;37m文化棲息地  [0;37m[[1;36m學術,文藝,音樂[0;37m][m",0,
  board, 0,		"33[1;37m水庫洩洪區  [0;37m[[1;33m閒聊,新聞,個人[0;37m][m",0,
  board, 0,		"44[1;37m休閒運動場  [0;37m[[1;36m電玩,娛樂,運動[0;37m][m",0,
  board, 0,		"55[1;37m電腦知識網  [0;37m[[1;33m軟體,硬體,網路[0;37m][m",0,
   New, 0,            "NNew          [所有看板列表]",0,
//   local_board,0,     "LLocal        [站內看板列表]",0,
//   good_board,0,      "GGoodBoard    [  優良看板  ]",0,
   Favor,PERM_BASIC,  "BBoardFavor   [我的最愛看板]",0,
favor_edit,PERM_BASIC,"FFavorEdit    [編輯我的最愛]",0,
   ReadSelect, 0,     "SSelect       [  選擇看板  ]",0,
   NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* RPG menu                                             */
/* ----------------------------------------------------- */
int /* t_pk(),*/rpg_help();

struct MENU rpglist[] = {
  rpg_help,0,	"HHelp       本遊戲介紹/規則說明",0,
  "SO/rpg.so:rpg_uinfo",
    0,		"UUserInfo   自己的狀態",1,
  "SO/rpg.so:rpg_race_c",
    PERM_BASIC,	"JJoin       加入職業工會(需五枚金幣)",1,
  "SO/rpg.so:rpg_guild",
    0,		"GGuild      職業工會",1,
  "SO/rpg.so:rpg_train",
    0,		"TTrain      訓練場(施工中)",1,
  "SO/rpg.so:rpg_top",
    0,		"LListTop    使用者排行榜",1,
  "SO/rpg.so:rpg_edit",
    PERM_SYSOP,	"QQuery      查詢修改USER資料",1,
//  "SO:rpg.so:rpg_shop",0,           "SShop       裝備商店(施工中)",1,
//  "SO:rpg.so:t_pk",0,                 "PPK         不算 PK",1,
NULL, 0, NULL,0};

int
rpg_menu()
{
  domenu(RMENU, "角色扮演遊戲", 'U', rpglist);
  return 0;
}
/* ----------------------------------------------------- */
/* Ptt money menu                                        */
/* ----------------------------------------------------- */

static struct MENU finlist[] = {
  "SO/buy.so:bank",     0,      "11Bank           \033[1;36m無數銀行\033[m",1,
  "SO/pip.so:pip_money",0,      "22ChickenMoney   雞金兌換處     換錢給電子雞用",1,
  "SO/song.so:ordersong",0,     "33OrderSong      \033[1;33m無數點歌機\033[m     $5g/首",1,
  "SO/buy.so:p_cloak",  0,      "44Cloak          臨時隱身/現身  $2g/次     (現身免費)",1,
  "SO/buy.so:p_from",   0,      "55From           修改故鄉       $5g/次",1,
  "SO/buy.so:p_exmail", 0,      "66Mailbox        購買信箱上限   $1000g/封",1,
  "SO/buy.so:p_fcloak", 0,      "77UltimaCloak    終極隱身大法   $10000g      可永久隱形",1,
  "SO/buy.so:p_ffrom",  0,      "88PlaceBook      故鄉修改寶典   $50000g     User名單按F可改故鄉",1,
//  "SO/buy.so:p_ulmail", 0,      "99NoLimit        信箱無上限     $1000000g   信箱上限無限制",1,
NULL, 0, NULL,0};

int
finance()
{
  domenu(FINANCE, "金融中心", '1', finlist);
  return 0;
}

#ifdef HAVE_GAME

/* ----------------------------------------------------- */
/* NetGame menu                                          */
/* ----------------------------------------------------- */

struct MENU netgame_list[] = {
  "SO/xyz.so:x_mj",0,
    "QQkmj      ★ 網路麻將場",1,
  "SO/xyz.so:x_big2",0,
    "BBig2      ★ 網路大老二",1,
//  "SO/xyz.so:x_chess",PERM_LOGINOK,
//    "CChess     ★ 網路下象棋",1,
NULL, 0, NULL,0};

int
netgame_menu()
{
  domenu(NETGAME, "網路連線遊戲", 'Q', netgame_list);
  return 0;
}
/* ----------------------------------------------------- */
/* Game menu                                             */
/* ----------------------------------------------------- */

static struct MENU gamelist[] = {
//  rpg_menu,0,
//    "RRPG        ■ 角色扮演遊戲           施工中",0,
  netgame_menu,0,
    "NNetGame    ■ 網路連線遊戲           $100s/次",0,
  "SO/gamble.so:ticket_main",PERM_BASIC,
    "GGamble     ★ 對對樂賭盤             $100s/張",1,
  "SO/marie.so:mary_m",0,
    "MMarie      ☆ 小瑪麗樂園             最低消費 $1s",1,
  "SO/gagb.so:gagb",0,
    "??A?B       ☆ 猜猜猜數字             最低消費 $1s",1,
  "SO/guessnum.so:fightNum",0,
    "FFightNum   ☆ 對戰猜數字             最低消費 $1s",1,
  "SO/bj.so:BlackJack",0,
    "JJack       ☆ 盈月黑傑克             最低消費 $1s",1,
  "SO/nine.so:p_nine",PERM_BASIC,
    "999         ☆ 天地久九九�            最低消費 $1s",1,
  "SO/dice.so:x_dice",0,
    "DDice       ☆ 西八拉賭場             最低消費 $1s",1,
  "SO/gp.so:p_gp",0,
    "PPoke       ☆ 金撲克梭哈             最低消費 $1s",1,
  "SO/pip.so:p_pipple",PERM_LOGINOK,
    "CChicken    ◆ 無數電子雞             免費給你玩!!",1,
  "SO/xyz.so:x_tetris",0,
    "TTetris     ◆ 俄羅斯方塊             免費給你玩",1,
  "SO/mine.so:Mine",0,
    "LLandMine   ◆ 勁爆踩地雷             每次100s",1,
  "SO/poker.so:p_dragon",0,
    "11接龍      ◆ 測試中的接龍           免費給你玩",1,
  "SO/chessmj.so:p_chessmj",0,
    "22ChessMJ   ☆ 象棋麻將               最低消費 $1s",1,
  "SO/seven.so:p_seven",0,
    "33Seven     ☆ 賭城七張               最低消費 $1s",1,
  "SO/bet.so:p_bet",0,
    "44Bet       ☆ 瘋狂賭盤               最低消費 $1s",1,
  "SO/lotto.so:menu_lotto",0,
    "55Lotto     ◇ 無數樂透簽賭站         最低消費 $1g",1,
  "SO/lo.so:q_lotto",PERM_NOTOP,
    "66Lotto     ◇ 包牌樂透簽賭站 ",1,
  "SO/stock.so:p_stock",PERM_BASIC,
    "SStock      ◇ 無數股市",1,

/*  x_bridgem,PERM_LOGINOK,"OOkBridge    【 橋牌競技 】",0,*/
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

  t_users,      0,              "LList          [線上名單]",0,
  t_pager,      PERM_BASIC,     "PPager         [切換狀態]",0,
  t_idle,       0,              "IIdle          [鎖定螢幕]",0,
  t_query,      0,              "QQueryUser     [查詢User]",0,
/* t_talk,       PERM_PAGE,      "TTalk          [找人聊天]",0, */
 "SO/chat.so:t_chat",PERM_CHAT,"CChatRoom      [連線聊天]",1, 
  t_display,    0,              "DDisplay       [水球回顧]",0,
  XFile,        PERM_XFILE,     "XXfile         [修改系統檔]",0,
NULL, 0, NULL,0};

/*-------------------------------------------------------*/
/* powerbook menu                                        */
/* ----------------------------------------------------- */

int null_menu(),my_gem(),my_allpost();

static struct MENU powerlist[] = {

  "SO/bbcall.so:bbcall_menu",
                       0,	"MMessager      [ 通訊錄 ]",1,
  "SO/mn.so:show_mn",
	               0,	"NNoteMoney     [ 記帳本 ]",1,
  my_gem,              0,       "GGem           [我的精華]",0,
  my_allpost,          0,       "AAllPost       [我的文章]",0,
  null_menu,           0,	"------ 答錄機 功\能 ------",0,
  "SO/pnote.so:p_read",0,       "PPhone Answer  [聽取留言]",1,
  "SO/pnote.so:p_call",0,       "CCall phone    [送出留言]",1,
  "SO/pnote.so:p_edithint",0,   "RRecord        [錄歡迎詞]",1,

NULL, 0, NULL,0};

int
PowerBook()
{
  domenu(POWERBOOK, "萬用手冊", 'N', powerlist);
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
  PowerBook,	PERM_BASIC,	"PPowerBook     [萬用手冊]",0,
  u_info,       0,              "IInfo          [修改資料]",0,
  u_habit,      PERM_BASIC,     "HHabit         [喜好設定]",0,
  ListMain,     PERM_LOGINOK,   "LList          [設定名單]",0, 

#ifdef POSTNOTIFY
  re_m_postnotify,PERM_BASIC,   "PPostNotify    [審核文章通知]",0,
#endif

  u_editfile,   PERM_LOGINOK,   "FFileEdit      [個人檔案]",0,
  u_cloak,      PERM_CLOAK,     "CCloak         [隱形密法]",0,

#ifdef REG_FORM
//  u_register,   PERM_POST,      "RRegister      [填註冊單]",0,
  u_register,   PERM_BASIC,      "RRegister      [填註冊單]",0,
#endif

#ifdef REG_MAGICKEY
  u_verify, 	PERM_BASIC,	"VVerify        [填註冊碼]",0,
#endif

  u_list,       PERM_BASIC,     "UUsers         [註冊名單]",0,
NULL, 0, NULL,0};

/* ----------------------------------------------------- */
/* XYZ menu                                              */
/* ----------------------------------------------------- */
#ifdef HAVE_GAME
int
game_list()
{
  domenu(GAME, "網路遊樂場", 'N', gamelist);
  return 0;
}
#endif

struct MENU netserv_list[] = {
"SO/bbcall.so:bbcall_main",
                PERM_LOGINOK,   "BB.B.Call      [網路傳呼]",1,
//"SO/tv.so:catv",PERM_BASIC,     "TTV-Program    [電視節目查詢]",1,
"SO/fortune.so:main_fortune",
                PERM_BASIC,     "QQueryFortune  [個人運勢預測]",1,
"SO/railway.so:railway2",
                PERM_BASIC,     "RRailWay       [火車時刻查詢]",1,
"SO/test.so:railway2",
                PERM_SYSOP,     "TTest          [測試用]",1,  
#ifdef HAVE_GOPHER
  "SO/xyz.so:x_gopher",PERM_LOGINOK,"GGopher       ■ 世新小地鼠伺服器 ←",1,
#endif
#ifdef BBSDOORS
  "SO/xyz.so:x_bbsnet", PERM_LOGINOK, "DDoor      【 其他 BBS站 】",1,
#endif
NULL, 0, NULL,0};

int
net_serv()
{
  domenu(PMENU, "網路連線服務", 'B', netserv_list);
  return 0;
}

int note();

static struct MENU servicelist[] = {
#ifdef HAVE_GAME
  game_list,    0,              "PPlay          [育樂中心]",0,
#endif
  finance,      PERM_LOGINOK,   "FFinance       [商品大街]",0,
  net_serv,	0,		"SServNet       [網路服務]",0,
//  "SO/xyz.so:KoK",
//  		0,		"KKK-Service    [萬王之王]",1,
  "SO/vote.so:all_vote",
                PERM_LOGINOK,   "VVote          [投票中心]",1,
  note,         PERM_LOGINOK,   "NNote          [寫留言板]",0,
  "SO/xyz.so:show_hint_message",
                0,              "HHint          [教學精靈]",1,

  "SO/indict.so:x_dict",
                0,              "DDictionary    [百科全書]",1,
/*
"SO/xyz.so:x_cdict",
                PERM_BASIC,     "CCD-67         [電子字典]",1,
*/
"SO/xyz.so:x_bbsnet",PERM_SYSOP,"BBBSNet        [連線工具]",1,
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
  m_new,        PERM_READMAIL,  "RNew           [閱\讀新信]",0,
  m_read,       PERM_READMAIL,  "RRead          [信件列表]",0,
  m_send,       PERM_BASIC,     "SSend          [站內寄信]",0,
  mail_list,    PERM_BASIC,     "MMailist       [群組寄信]",0,
  m_internet,   PERM_INTERNET,  "IInternet      [網路郵件]",0,
  setforward,   PERM_LOGINOK,   "FForward       [收信轉寄]",0,
  m_sysop,      0,              "OOp Mail       [諂媚站長]",0,
  mail_mbox,    PERM_INTERNET,  "ZZip           [打包資料]",0,
  mail_all,     PERM_SYSOP,     "AAll           [系統通告]",0,
NULL, 0, NULL,0};



/* ----------------------------------------------------- */
/* main menu                                             */
/* ----------------------------------------------------- */

static int
admin()
{
  domenu(ADMIN, "站長老大", 'X', adminlist);
  return 0;
}

static int
BOARD()
{
  domenu(CLASS, "看板列表", 'G', classlist);
  return 0;
}

static int
Mail()
{
  domenu(MAIL, "郵件選單", 'R', maillist);
  return 0;
}

int
static Talk()
{
  domenu(TMENU, "聊天選單", 'L', talklist);
  return 0;
}

static int
User()
{
  domenu(UMENU, "個人設定", 'H', userlist);
  return 0;
}


static int
Service()
{
  domenu(PMENU, "各種服務", 'H', servicelist);
  return 0;
}


int Announce(), Boards(), Goodbye(),Log(),board();


struct MENU cmdlist[] = {
  admin,        PERM_ADMIN,     "00Admin        [系統管理]",0,
  Announce,     0,              "AAnnounce      [天地精華]",0,
  BOARD,        0,              "BBoard         [看板功\能]",0,
  board,        0,              "CClass         [分類看板]",0,
  Mail,         PERM_BASIC,     "MMail          [無數郵局]",0,
  Talk,         0,              "TTalk          [談天說地]",0,
  User,         PERM_BASIC,     "UUser          [個人工具]",0,
  Log,          0,              "HHistory       [歷史軌跡]",0,
  Service,      PERM_BASIC,     "SService       [各種服務]",0,
  Goodbye,      0,              "GGoodbye       [有緣千里]",0,
NULL, 0, NULL,0};
/* INDENT ON */
