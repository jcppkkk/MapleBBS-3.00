/*-------------------------------------------------------*/
/* global.h     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : global definitions & variables               */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
#include "hyper.h"

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/* ----------------------------------------------------- */
/* GLOBAL DEFINITION                                     */
/* ----------------------------------------------------- */


/* 檔名設定 */

#define FN_PASSWD       ".PASSWDS"      /* User records */
#define FN_BOARD        ".BOARDS"       /* board list */
#define FN_RPG		".RPG"		/* RPG records */
#define FN_GAME		".GAME"	/* RPG records */
#define FN_USIES        "usies"         /* BBS log */
#define FN_USSONG       "ussong"        /* 點歌 */
#define FN_VOTE		".VCH"		/* 投票 */
#define FN_LIST		".LIST"		/* 名單 */
#define FN_PAL		"pal"
#define FN_ALOHA	"aloha"

#define FN_TOPSONG      "log/topsong"
#define FN_TOPRPG       "log/toprpg"
#define FN_GAMEMONEY    "game/money"
#define FN_MONEYLOG     "log/moneystat"
#define FN_OVERRIDES    "overrides"
#define FN_REJECT       "reject"
#define FN_CANVOTE      "can_vote"
#define FN_WATER        "water"
#define FN_APPLICATION  "application"
#define FN_VISABLE      "visable"
#define FN_MYFAVORITE   "favorite"

#define FN_TICKET_RECORD "game/ticket.data"
#define FN_TICKET_USER   "game/ticket.user"
#define FN_TICKET        "game/ticket.result"

#define DEFAULT_BOARD   str_sysop

/* 鍵盤設定 */

#ifndef EXTEND_KEY
#define EXTEND_KEY
#define KEY_TAB         9
#define KEY_ESC         27
#define KEY_UP          0x0101
#define KEY_DOWN        0x0102
#define KEY_RIGHT       0x0103
#define KEY_LEFT        0x0104
#define KEY_HOME        0x0201
#define KEY_INS         0x0202
#define KEY_DEL         0x0203
#define KEY_END         0x0204
#define KEY_PGUP        0x0205
#define KEY_PGDN        0x0206
#endif

#define Ctrl(c)         ( c & 037 )

#ifdef SYSV
#undef CTRL                     /* SVR4 CTRL macro is hokey */
#define CTRL(c) ('c'&037)       /* This gives ESIX a warning...ignore it! */
#endif

#define chartoupper(c)  ((c >= 'a' && c <= 'z') ? c+'A'-'a' : c)
#define char_lower(c)  ((c >= 'A' && c <= 'Z') ? c|32 : c)

/* ----------------------------------------------------- */
/* External function declarations                        */
/* ----------------------------------------------------- */

#define TRACE   log_usies

/* ----------------------------------------------------- */
/* 訊息字串：獨立出來，以利支援各種語言                  */
/* ----------------------------------------------------- */

#define STR_CURSOR      "喵"
#define STR_UNCUR       "  "

#define STR_AUTHOR1     "作者:"
#define STR_AUTHOR2     "發信人:"
#define STR_POST1       "看板:"
#define STR_POST2       "站內:"

#define LEN_AUTHOR1     5
#define LEN_AUTHOR2     7

#define STR_GUEST       "guest"

#define MSG_SEPERATOR   "\
───────────────────────────────────────"

#define MSG_CLOAKED     "嘿嘿,躲起來囉!"
#define MSG_UNCLOAK     "重現江湖了...."

#define MSG_WORKING     "處理中，請稍候..."

#define MSG_CANCEL      "取消。"
#define MSG_USR_LEFT    "User 已經離開了"
#define MSG_NOBODY      "目前無人上線"
#define MSG_MY_FAVORITE "我的最愛看板"

#define MSG_DEL_OK      "刪除完畢"
#define MSG_DEL_CANCEL  "取消刪除"
#define MSG_DEL_ERROR   "刪除錯誤"
#define MSG_DEL_NY      "請確定刪除(Y/N)?[N] "

#define MSG_FWD_OK      "文章轉寄完成!"
#define MSG_FWD_ERR1    "轉寄失誤: 系統發生錯誤"
#define MSG_FWD_ERR2    "轉寄失誤: 地址錯誤，查無此人"

#define MSG_SURE_NY     "請您確定(Y/N)？[N] "
#define MSG_SURE_YN     "請您確定(Y/N)？[Y] "

#define MSG_BID         "請輸入看板名稱："
#define MSG_UID         "請輸入使用者代號："
#define MSG_PASSWD      "請輸入您的密碼: "

#define MSG_BIG_BOY     "帥哥"
#define MSG_BIG_GIRL    "美女"
#define MSG_LITTLE_BOY  "底迪"
#define MSG_LITTLE_GIRL "美眉"
#define MSG_MAN         "叔叔"
#define MSG_WOMAN       "阿姨"
#define MSG_PLANT       "植物"
#define MSG_MIME        "礦物"

#define ERR_BOARD_OPEN  ".BOARD 開啟錯誤"
#define ERR_BOARD_UPDATE        ".BOARD 更新有誤"
#define ERR_PASSWD_OPEN ".PASSWDS 開啟錯誤"

#define ERR_BID         "你搞錯了啦！沒有這個板喔！"
#define ERR_UID         "這裡沒有這個人啦！"
#define ERR_PASSWD      "密碼不對喔！你有沒有冒用人家的名字啊？"
#define ERR_FILENAME    "檔名不合法！"

#define MSG_SELECT_BOARD        \
        COLOR1"[1;37m【 選擇看板 】[0m\n請輸入看板名稱(按空白鍵自動搜尋)："

#define MSG_MAILER      \
COLOR2"  信信相惜  "\
COLOR1" [33;200;442;720m(x)[37m轉達 [33;200;442;688m(X)[37m轉錄 [33;200;442;670m(F)[37m傳回信箱 [33;200;442;700m(d)[37m刪除 [33;200;442;716m(t)[37m標記信件 [33;200;442;604m(^D)[37m刪除 [33;200;442;709m[mark][201m   [m"

#define MSG_SHORTULIST  \
"[7m使用者代號    目前狀態   │使用者代號    目前狀態   │使用者代號    目前狀態  [0m"

#define P_BOARD "對不起,此板只准看板好友進入,請向板主申請入境許\可"


#ifdef  _MAIN_C_

/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */

char trans_buffer[256];         /* 一般傳遞變數 add by Ptt */

int usernum;
pid_t currpid;                  /* current process ID */
usint currstat;
int currmode = 0;
int curredit = 0;
int showansi = 1;
time_t login_start_time;
time_t update_time;
time_t schedule_time;
char schedule_string[100];
userec cuser;                   /* current user structure */
userec xuser;                   /* lookup user structure */
rpgrec rpguser;
rpgrec rpgtmp;
char quote_file[80] = "\0";
char quote_user[80] = "\0";
time_t paste_time;
char paste_title[STRLEN];
char paste_path[256];
int  paste_level;
char currtitle[TTLEN + 1] = "\0";
char vetitle[40] = "\0";
char currowner[STRLEN] = "\0";
char currauthor[IDLEN + 2] = "\0";
char currfile[FNLEN];           /* current file name @ bbs.c mail.c */
uschar currfmode;               /* current file mode */
char currmsg[100];
char currboard[IDLEN + 2];
usint currbrdattr;
char currBM[IDLEN * 3 + 10];
char reset_color[4] = "[m";
char margs[64] = "\0";           /*  main argv list*/
char watermode = 0;
int inmore = 0;
/* global string variables */


/* filename */

char *fn_passwd         = FN_PASSWD;
char *fn_board          = FN_BOARD;
char *fn_rpg		= FN_RPG;
char *fn_note_ans       = "note.ans";
char *fn_register       = "register.new";
char *fn_plans          = "plans";
char *fn_writelog       = "writelog";
char *fn_talklog        = "talklog";
char *fn_overrides      = FN_OVERRIDES;
char *fn_reject         = FN_REJECT;
char *fn_canvote        = FN_CANVOTE;
char *fn_notes          = "notes";
char *fn_water          = FN_WATER;
char *fn_visable        = FN_VISABLE;
char *fn_myfavorite     = FN_MYFAVORITE;
char *fn_mandex         = "/.Names";

/* message */

char *msg_seperator     = MSG_SEPERATOR;
char *msg_mailer        = MSG_MAILER;
char *msg_shortulist    = MSG_SHORTULIST;

char *msg_cancel        = MSG_CANCEL;
char *msg_usr_left      = MSG_USR_LEFT;
char *msg_nobody        = MSG_NOBODY;

char *msg_sure_ny       = MSG_SURE_NY;
char *msg_sure_yn       = MSG_SURE_YN;

char *msg_bid           = MSG_BID;
char *msg_uid           = MSG_UID;

char *msg_del_ok        = MSG_DEL_OK;
char *msg_del_ny        = MSG_DEL_NY;

char *msg_fwd_ok        = MSG_FWD_OK;
char *msg_fwd_err1      = MSG_FWD_ERR1;
char *msg_fwd_err2      = MSG_FWD_ERR2;

char *err_board_update  = ERR_BOARD_UPDATE;
char *err_bid           = ERR_BID;
char *err_uid           = ERR_UID;
char *err_filename      = ERR_FILENAME;

char *str_mail_address  = "." BBSUSER "@" MYHOSTNAME;
char *str_new           = "new";
char *str_reply         = "Re: ";
char *str_space         = " \t\n\r";
char *str_sysop         = "SYSOP";
char *str_author1       = STR_AUTHOR1;
char *str_author2       = STR_AUTHOR2;
char *str_post1         = STR_POST1;
char *str_post2         = STR_POST2;
char *BoardName         = BOARDNAME;
char *str_dotdir        = ".DIR";
char tmpbuf[512];
#else                           /* _MAIN_C_ */


/* ----------------------------------------------------- */
/* GLOBAL VARIABLE                                       */
/* ----------------------------------------------------- */

extern char trans_buffer[256];         /* 一般傳遞變數 add by Ptt */

extern int usernum;
extern pid_t currpid;
extern usint currstat;
extern int currmode;
extern int curredit;
extern int showansi;
extern int talkrequest;
extern time_t login_start_time;
extern time_t update_time;
extern time_t schedule_time;
extern char schedule_string[100];
extern userec cuser;            /* current user structure */
extern userec xuser;            /* lookup user structure */
extern rpgrec rpguser;
extern rpgrec rpgtmp;

extern char quote_file[80];
extern char quote_user[80];
extern time_t paste_time;
extern char paste_title[STRLEN];
extern char paste_path[256];
extern int  paste_level;
extern char currowner[STRLEN];
extern char currauthor[IDLEN + 2];
extern uschar currfmode;               /* current file mode */
extern char currtitle[TTLEN + 1];
extern char vetitle[44];
extern char currfile[FNLEN];
extern char currmsg[100];

extern char currboard[];        /* name of currently selected board */
extern usint currbrdattr;
extern char currBM[];           /* BM of currently selected board */
extern char reset_color[];
extern char margs[];
extern char watermode;
extern int inmore;
/* global string variable */

/* filename */

extern char *fn_passwd;
extern char *fn_board;
extern char *fn_rpg;
extern char *fn_note_ans;
extern char *fn_register;
extern char *fn_plans;
extern char *fn_writelog;
extern char *fn_talklog;
extern char *fn_overrides;
extern char *fn_reject;
extern char *fn_canvote;
extern char *fn_notes;
extern char *fn_water;
extern char *fn_visable;
extern char *fn_myfavorite;
extern char *fn_mandex;

/* message */

extern char *msg_seperator;
extern char *msg_mailer;
extern char *msg_shortulist;

extern char *msg_cancel;
extern char *msg_usr_left;
extern char *msg_nobody;

extern char *msg_sure_ny;
extern char *msg_sure_yn;

extern char *msg_bid;
extern char *msg_uid;

extern char *msg_del_ok;
extern char *msg_del_ny;

extern char *msg_fwd_ok;
extern char *msg_fwd_err1;
extern char *msg_fwd_err2;

extern char *err_board_update;
extern char *err_bid;
extern char *err_uid;
extern char *err_filename;

extern char *str_mail_address;
extern char *str_new;
extern char *str_reply;
extern char *str_space;
extern char *str_sysop;
extern char *str_author1;
extern char *str_author2;
extern char *str_post1;
extern char *str_post2;
extern char *BoardName;
extern char *str_dotdit;
extern char tmpbuf[512];

#ifdef XINU
extern int errno;

#endif

#endif                          /* _MAIN_C_ */

extern int errno;
extern jmp_buf byebye;          /* for exception condition like I/O error */

extern user_info *currutmp;

extern int dumb_term;
extern int t_lines, t_columns;  /* Screen size / width */
extern int b_lines;             /* Screen bottom line number: t_lines-1 */
extern int p_lines;             /* a Page of Screen line numbers: tlines-4 */

extern char fromhost[];
extern char save_title[];       /* used by editor when inserting */

extern int KEY_ESC_arg;


void doent();   /* read,mail,announce list */

#define RACE_NORACE     "無業遊民"
#define RACE_POST       "灌水強人"
#define RACE_READ       "讀文快手"
#define RACE_IDLE       "常駐程式"
#define RACE_CHAT       "聊天猛將"
#define RACE_MSG        "水球冠軍"
#define RACE_GAME       "遊戲剋星"

#endif                          /* _GLOBAL_H_ */
