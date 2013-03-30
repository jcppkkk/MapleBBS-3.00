/*-------------------------------------------------------*/
/* struct.h     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : all definitions about data structure         */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#ifndef _STRUCT_H_
#define _STRUCT_H_


#define STRLEN   80             /* Length of most string data */
#define BRC_STRLEN 15           /* Length of boardname */
#define BTLEN    48             /* Length of board title */
#define TTLEN    72             /* Length of title */
#define NAMELEN  40             /* Length of username/realname */
#define FNLEN    33             /* Length of filename  */
				/* Ptt ���o�̦�bug*/
#define IDLEN    12             /* Length of bid/uid */
#define PASSLEN  14             /* Length of encrypted passwd field */
#define REGLEN   38             /* Length of registration data */



typedef unsigned char uschar;   /* length = 1 */
typedef unsigned int usint;     /* length = 4 */

/* ----------------------------------------------------- */
/* .PASSWDS struct : 512 bytes                           */
/* ----------------------------------------------------- */
struct userec
{
  char userid[IDLEN + 1];         /* �ϥΪ̦W��  13 bytes */
  char realname[20];              /* �u��m�W    20 bytes */
  char username[24];              /* �ʺ�        24 bytes */
  char passwd[PASSLEN];           /* �K�X        14 bytes */
  uschar uflag;                   /* �ϥΪ̿ﶵ   1 byte  */
  usint userlevel;                /* �ϥΪ��v��   4 bytes */
  ushort numlogins;               /* �W������     2 bytes */
  ushort numposts;                /* POST����     2 bytes */
  time_t firstlogin;              /* ���U�ɶ�     4 bytes */
  time_t lastlogin;               /* �e���W��     4 bytes */
  char lasthost[24];              /* �W���a�I    24 bytes */
  char vhost[24];                 /* �������}    24 bytes */
  char email[50];                 /* E-MAIL      50 bytes */
  char address[50];               /* �a�}        50 bytes */
  char justify[REGLEN + 1];       /* ���U���    39 bytes */
  uschar month;                   /* �X�ͤ��     1 byte  */
  uschar day;                     /* �X�ͤ��     1 byte  */
  uschar year;                    /* �X�ͦ~��     1 byte  */
  uschar sex;                     /* �ʧO         1 byte  */
  uschar state;                   /* ���A??       1 byte  */
  usint habit;                    /* �ߦn�]�w     4 bytes */
  uschar pager;                   /* �߱��C��     1 bytes */
  uschar invisible;               /* �����Ҧ�     1 bytes */
  usint exmailbox;                /* �H�c�ʼ�     4 bytes */
  usint exmailboxk;               /* �H�cK��      4 bytes */
  usint toquery;                  /* �n�_��       4 bytes */
  usint bequery;                  /* �H���       4 bytes */
  char toqid[IDLEN + 1];	  /* �e���d��    13 bytes */
  char beqid[IDLEN + 1];	  /* �e���Q�֬d  13 bytes */
  unsigned long int totaltime;    /* �W�u�`�ɼ�   8 bytes */
  usint sendmsg;                  /* �o�T������   4 bytes */
  usint receivemsg;               /* ���T������   4 bytes */
  unsigned long int goldmoney;	  /* ���Ъ���     8 bytes */ 
  unsigned long int silvermoney;  /* �ȹ�         8 bytes */
  unsigned long int exp;          /* �g���       8 bytes */
  time_t dtime;                   /* �s�ڮɶ�     4 bytes */
  int scoretimes;		  /* �����U��	  4 bytes */
  uschar rtimes;		  /* ����U�榸�� 1 bytes */
  int award;			  /* ���g�P�_	  4 bytes */ 
  int pagermode; 		  /* �I�s������   4 bytes */
  char pagernum[7];		  /* �I�s�����X   7 bytes */
  char feeling[5];		  /* �߱�����     5 bytes */
  char title[20];		  /* �ٿ�(�ʸ�)  20 bytes */
  usint five_win;
  usint five_lost;
  usint five_draw;
  char pad[91];                  /* �ŵ۶񺡦�512��      */
};

typedef struct userec userec;

/* these are flags in userec.uflag */
#define SIG_FLAG        0x3     /* signature number, 2 bits */
#define PAGER_FLAG      0x4     /* true if pager was OFF last session */
#define CLOAK_FLAG      0x8     /* true if cloak was ON last session */
#define FRIEND_FLAG     0x10    /* true if show friends only */
#define BRDSORT_FLAG    0x20    /* true if the boards sorted alphabetical */
#define MOVIE_FLAG      0x40    /* true if show movie */
#define COLOR_FLAG      0x80    /* true if the color mode open */

/* ----------------------------------------------------- */
/* RPG struct :256 bytes    		                 */
/* ----------------------------------------------------- */
struct rpgrec
{
  char userid[IDLEN+1];		  /* User ID     13 bytes */
  usint age;			  /* �~��	  4 bytes */
  uschar race;			  /* �ر�	  1 bytes */
  uschar subrace;		  /* �Ʒ~	  1 bytes */
  ushort level;			  /* ����	  2 bytes */  
  char family[20];		  /* �a��	 20 bytes */
  char nick[20];		  /* �ʸ�	 20 bytes */
  int hp;			  /* ��O	  4 bytes */
  int mp;			  /* �k�O	  4 bytes */
  usint skill;			  /* �ޯ�	  4 bytes */
  ushort str;			  /* �O�q	  2 bytes */
  ushort dex;			  /* �ӱ�	  2 bytes */
  ushort wis;			  /* ���z	  2 bytes */
  ushort con;			  /* ���	  2 bytes */
  ushort kar;			  /* �B��	  2 bytes */
  uschar weapon;		  /* �Z��	  1 bytes */
  uschar armor;			  /* ����	  1 bytes */
  usint object;			  /* ����	  4 bytes */
  char pad[164];
};
typedef struct rpgrec rpgrec;

/* ----------------------------------------------------- */
/* LOG of games struct : 128 bytes                       */
/* ----------------------------------------------------- */

struct gamedata
{
  int five_win;
  int five_lost;
  int five_draw;
  char pad[116];
};
typedef struct gamedata gamedata;

/* ----------------------------------------------------- */
/* DIR of board struct : 128 bytes                       */
/* ----------------------------------------------------- */

struct fileheader
{
  char filename[FNLEN-1];       /* M.9876543210.A 	33 bytes*/
  char score;                   /* ����                  1 bytes*/
  char savemode;                /* file save mode 	 1 bytes*/
  char owner[IDLEN + 2];        /* uid[.] 		14 bytes*/
  char date[6];                 /* [02/02] or space(5)   6 bytes*/
  char title[TTLEN + 1];	/* title		73 bytes*/
//  time_t chrono;                /* timestamp */
//  char dummy;
  uschar filemode;              /* must be last field @ boards.c 1 bytes*/
};
typedef struct fileheader fileheader;


struct PAL
{
  char userid[FNLEN];           /* list name/userid */
  char savemode;                
  char owner[IDLEN + 2];        /* /bbcall */
  char date[6];                 /* /birthday */
  char desc[TTLEN + 1];         /* list/user desc */
  uschar ftype;                 /* mode:  PAL, BAD */
};
typedef struct PAL PAL;

#define M_PAL		0x01
#define M_BAD		0x02
#define M_ALOHA		0x04

#define M_VISABLE	0x01
#define M_WATER		0x02
#define M_CANVOTE	0x04 


#define FILE_LOCAL      0x1     /* local saved */
#define FILE_READ       0x1     /* already read : mail only */
#define FILE_MARKED     0x2     /* opus: 0x8 */
#define FILE_DIGEST     0x4     /* digest */
#define FILE_TAGED      0x8     /* taged */
#define FILE_REPLYOK	0x10	/* reply ok */
#define FILE_REFUSE	0x20	/* refuse */
#define FILE_DIR	0x40	/* dir */


/* ----------------------------------------------------- */
/* Structure used in UTMP file : ??? bytes               */
/* ----------------------------------------------------- */

/* �q�l�� */
typedef struct pipdata
{
  char name[20];
  int hp;         /*��O*/
  int maxhp;      /*��O�W��*/
  int mp;         /*�k�O*/
  int maxmp;      /*�k�O�W��*/
  int attack;     /*����*/
  int resist;     /*���m*/
  int speed;      /*�t��*/
  int mresist;    /*�]�k���m*/
  int resistmore; /*���m���A*/
  int nodone;     /*����*/
  int leaving;    /*���}*/
  int pipmode;    /*���A*/
  int money;      /*����*/
  int msgcount;   /*�T���Ӽ�*/
  int chatcount;
  char msg[150];  /*�T�����e*/
  char chat[10][150]; /*��Ѥ��e*/
}pipdata;


/*
woju
Message queue
*/
typedef struct {
   pid_t last_pid;
   char last_userid[IDLEN + 1];
   char last_call_in[80];
} msgque;

struct user_info
{
  int uid;                      /* Used to find user name in passwd file */
  pid_t pid;                    /* kill() to notify user of talk request */
  int sockaddr;                 /* ... */
  int destuid;                  /* talk uses this to identify who called */
  struct user_info* destuip;
  uschar active;                /* When allocated this field is true */
  uschar invisible;             /* Used by cloaking function in Xyz menu */
  uschar sockactive;            /* Used to coordinate talk requests */
  usint userlevel;
  uschar mode;                  /* UL/DL, Talk Mode, Chat Mode, ... */
  uschar pager;                 /* pager toggle, YEA, or NA */
  uschar in_chat;               /* for in_chat commands   */
  uschar sig;                   /* signal type */
  char userid[IDLEN + 1];
  char chatid[11];              /* chat id, if in chat mode */
  char realname[20];
  char username[24];
  char from[27];                /* machine name the user called in from */
  int from_alias;
  char birth;                   /* �O�_�O�ͤ� Ptt*/
  char tty[11];                 /* tty port */
  ushort friend[MAX_FRIEND];
  ushort reject[MAX_REJECT];
  uschar msgcount;
  msgque msgs[3];
  time_t uptime;
  time_t lastact;             /* �W���ϥΪ̰ʪ��ɶ� */
  usint brc_id;
  uschar lockmode;
//  pipdata pip;
  int turn;
  char feeling[5];		/* �߱� */
};
typedef struct user_info user_info;


/* ----------------------------------------------------- */
/* BOARDS struct : 512 bytes                             */
/* ----------------------------------------------------- */
#define BRD_NOZAP       00001         /* ���izap  */
#define BRD_NOCOUNT     00002         /* ���C�J�έp */
#define BRD_NOTRAN      00004         /* ����H */
#define BRD_GROUPBOARD  00010         /* �s�ժO */
#define BRD_HIDE        00020         /* ���êO (�ݪO�n�ͤ~�i��) */
#define BRD_POSTMASK    00040         /* ����o��ξ\Ū */
#define BRD_ANONYMOUS   00100         /* �ΦW�O? */
#define BRD_CLASS	00200         /* �����ݪO */
#define BRD_GOOD	00400         /* �u�}�ݪO */
#define BRD_PERSONAL	01000         /* �ӤH�ݪO */
#define BRD_NOFOWARD	02000	      /* �T����� */
#define BRD_NOSCORE     04000         /* �������� */

struct boardheader
{
  char brdname[IDLEN + 1];      /* �ݪO�^��W��    13 bytes */
  char title[BTLEN + 1];        /* �ݪO����W��    49 bytes */
  char BM[IDLEN * 3 + 3];       /* �O�DID�M"/"     39 bytes */
  usint brdattr;    		/* �ݪO���ݩ�       4 bytes */
  time_t bupdate;               /* note update time 4 bytes */
  uschar bvote;                 /* Vote flags       1 bytes */
  time_t vtime;                 /* Vote close time  4 bytes */
  usint level;                  /* �i�H�ݦ��O���v�� 4 bytes */
  unsigned long int totalvisit; /* �`���X�H��       8 bytes */
  unsigned long int totaltime;  /* �`���d�ɶ�       8 bytes */
  char lastvisit[IDLEN + 1];    /* �̫�ݸӪO���H  13 bytes */
  time_t opentime;              /* �}�O�ɶ�         4 bytes */
  time_t lastime;               /* �̫���X�ɶ�     4 bytes */
  char passwd[PASSLEN];         /* �K�X            14 bytes */
  unsigned long int postotal;   /* �`���q :p        8 bytes */
// wildcat note : check �o�� , expire.conf �����L�C�C�����a ...
  usint maxpost;		/* �峹�W��         4 bytes */
  usint maxtime;		/* �峹�O�d�ɶ�	    4 bytes */
  char desc[3][80];		/* ����y�z	  240 bytes */
  char pad[87];
};
typedef struct boardheader boardheader;


struct one_key
{                               /* Used to pass commands to the readmenu */
  int key;
  int (*fptr) ();
};


/* ----------------------------------------------------- */
/* cache.c ���B�Ϊ���Ƶ��c                              */
/* ----------------------------------------------------- */


#define USHM_SIZE       (MAXACTIVE + 4)
struct UTMPFILE
{
  user_info uinfo[USHM_SIZE];
  time_t uptime;
  int number;
  int busystate;
};

struct BCACHE
{
  boardheader bcache[MAXBOARD];
  usint total[MAXBOARD];
  time_t lastposttime[MAXBOARD];
  time_t uptime;
  time_t touchtime;
  int number;
  int busystate;
};

struct UCACHE
{
  char userid[MAXUSERS][IDLEN + 1];
  time_t uptime;
  time_t touchtime;
  int number;
  int busystate;
};
/* Ptt */
struct FILMCACHE
{
  char notes[MAX_MOVIE][FNLEN];
  char today_is[20];
  int max_film;
  int max_history;
  time_t uptime;
  time_t touchtime;
  int busystate;
};

struct FROMCACHE
{
  char domain[MAX_FROM][50];
  char replace[MAX_FROM][50];
  int top;
  int max_user;
  time_t max_time;
  time_t uptime;
  time_t touchtime;
  int busystate;
};

struct BACACHE          
{                       
  char author[300][100];
  int top;              
  time_t uptime;        
  time_t touchtime;     
  int busystate;        
};                      

struct hosts
{
 char shortname[24];
 char address[40];
 char desc[24];
};

typedef struct hosts hosts;


typedef struct
{
  time_t chrono;
  int recno;
}      TagItem;


/* ----------------------------------------------------- */
/* screen.c ���B�Ϊ���Ƶ��c                             */
/* ----------------------------------------------------- */

#define ANSILINELEN (511)       /* Maximum Screen width in chars */

/* Line buffer modes */
#define MODIFIED (1)            /* if line has been modifed, screen output */
#define STANDOUT (2)            /* if this line has a standout region */

#define SL_MODIFIED	(1)	/* if line has been modifed, screen output */
#define SL_STANDOUT	(2)	/* if this line contains standout code */
#define SL_ANSICODE	(4)	/* if this line contains ANSI code */

struct screenline
{
  uschar oldlen;                /* previous line length */
  uschar len;                   /* current length of line */
  uschar width;			/* padding length of ANSI codes */
  uschar mode;                  /* status of line, as far as update */
  uschar smod;                  /* start of modified data */
  uschar emod;                  /* end of modified data */
  uschar sso;                   /* start stand out */
  uschar eso;                   /* end stand out */
  uschar data[ANSILINELEN];
};
typedef struct screenline screenline;

typedef struct LinkList
{
  struct LinkList *next;
  char data[0];
}        LinkList;

/* ----------------------------------------------------- */
/* name.c ���B�Ϊ���Ƶ��c                               */
/* ----------------------------------------------------- */

struct word
{
  char *word;
  struct word *next;
};


/* ----------------------------------------------------- */
/* edit.c ���B�Ϊ���Ƶ��c                               */
/* ----------------------------------------------------- */

#define WRAPMARGIN (500)

struct textline
{
  struct textline *prev;
  struct textline *next;
  int len;
  char data[WRAPMARGIN + 1];
};
typedef struct textline textline;

#endif                          /* _STRUCT_H_ */

/* ----------------------------------------------------- */
/* announce.c                                            */
/* ----------------------------------------------------- */

#define MAXITEMS        1000     /* �@�ӥؿ��̦h���X�� */

typedef struct
{
  fileheader *header;
  char mtitle[STRLEN];
  char *path;
  int num, page, now, level, mode;
} AMENU;

union x_item
{
  struct                        /* bbs_item */
  {
    char fdate[9];              /* [mm/dd/yy] */
    char editor[13];            /* user ID */
    char fname[31];
  }      B;

  struct                        /* gopher_item */
  {
    char path[81];
    char server[48];
    int port;
  }      G;
};

typedef struct
{
  char title[63];
  union x_item X;
}      ITEM;

typedef struct
{
  ITEM *item[MAXITEMS];
  char mtitle[STRLEN];
  char *path;
  int num, page, now, level;
}      GMENU;


/* �������� struct */

struct notedata {
  time_t date;
  char userid[IDLEN + 1];
  char username[19];
  char buf[3][80];
};
typedef struct notedata notedata;

// bwboard �Ψ쪺

typedef struct
{
  int key;
  int (*func) ();
}      KeyFunc;

/* ----------------------------------------------------- */
/* mn.c ���B�Ϊ���Ƶ��c                                 */
/* ----------------------------------------------------- */
struct  money_note
{
  usint  year;      // �~         4 b
  uschar month;     // ��         1 b
  uschar day;       // ��         1 b
  uschar flag;      // ���J/��X  1 b
  usint  money;     // ���B       4 b
  uschar use_way;   // ���O(������|��?)    1b
  char   desc[50];  // ����       50 b
  char   pad[62];   // null pad   62 b
};
typedef struct money_note MN;

#define MODE_OUT	0x1	// ��X
#define MODE_IN		0x2	// ���J

#define	WAY_EAT		0	// ��
#define WAY_WEAR	1	// ��
#define	WAY_LIVE	2	// ��
#define WAY_MOVE	3	// ��
#define	WAY_EDU		4	// �|
#define	WAY_PLAY	5	// ��
#define WAY_OTHER	6	// ��L