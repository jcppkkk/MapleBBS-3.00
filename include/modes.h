/*-------------------------------------------------------*/
/* modes.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user operating mode & status                 */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#ifndef _MODES_H_
#define _MODES_H_

/* ----------------------------------------------------- */
/* strip ansi mode   Ptt                                 */
/* ----------------------------------------------------- */

enum
{STRIP_ALL, ONLY_COLOR, NO_RELOAD};

/* ----------------------------------------------------- */
/* money mode   wildcat                                  */
/* ----------------------------------------------------- */

enum
{SILVER,GOLD};

/* ----------------------------------------------------- */
/* �s�զW��Ҧ�   Ptt                                    */
/* ----------------------------------------------------- */

#define FRIEND_OVERRIDE 0
#define FRIEND_REJECT   1
#define FRIEND_ALOHA    2
#define FRIEND_POST     3         
#define FRIEND_SPECIAL  4
#define FRIEND_CANVOTE  5
#define BOARD_WATER     6
#define BOARD_VISABLE   7 
#define FRIEND_MAILLIST	8
#define FRIEND_MAILREPLYLIST	9



/* ----------------------------------------------------- */
/* user �ާ@���A�P�Ҧ�                                   */
/* ----------------------------------------------------- */

#define IDLE            0
#define MMENU           1       /* menu mode */
#define ADMIN           2
#define MAIL            3
#define TMENU           4
#define UMENU           5
#define XMENU           6
#define CLASS           7
#define PMENU           8
#define NMENU           9
#define FINANCE         10
#define POSTING         11      /* boards & class */
#define READBRD         12
#define READING         13
#define READNEW         14
#define SELECT          15
#define RMAIL           16      /* mail menu */
#define SMAIL           17
#define CHATING         18      /* talk menu */
#define XMODE           19
#define FRIEND          20
#define LAUSERS         21
#define LUSERS          22
#define MONITOR         23
#define PAGE            24
#define QUERY           25
#define TALK            26
#define EDITPLAN        27      /* user menu */
#define EDITSIG         28
#define VOTING          29
#define XINFO           30
#define MSYSOP          31
#define LOG		32
#define BIG2            33
#define REPLY           34
#define HIT             35
#define DBACK           36
#define NOTE            37
#define EDITING         38
#define MAILALL         39
/*Ptt*/
#define MJ              40
#define LOGIN           41       /* main menu */
#define DICT            42
#define BRIDGE          43
#define ARCHIE          44
#define GOPHER          45
#define SCHED		46
#define CATV            47
#define EDITEXP         48
#define CAL             49
#define PROVERB         50
#define ANNOUNCE        51       /* announce */
#define EDNOTE          52 
#define GAME            53
#define MARIE           54
#define CHICKEN         55
#define XBOARDS         56
#define TICKET		57
#define B_MENU          58
#define RACE            59
#define BINGO           60
#define XAXB		61
#define TETRIS		62
#define CDICT		63
#define XSYSTEM		64
#define XUSER		65
#define BANK		66
#define BLACKJACK	67
#define OSONG           68

/* RPG ��� */
#define RMENU		69
#define RGUILD 		70
#define RTRAIN		71
#define RINFO		72
#define RPK		73
#define RHELP		74
#define RCHOOSE		75
#define RINTRO		76
#define EDITPIC		77
#define RSHOP		78
#define RTOPLIST	79

#define DICE		80
#define RICH		81
#define STOCK		82
#define MINE            83
#define CHICKENPAGE	84
#define CHICKENTALK	85
#define GP		86
#define NINE		87
#define FIVE		88
#define CHESS		89
#define	POWERBOOK	90
#define	PRECORD		91
#define	MSGMENU		92
#define NumFight	93
#define	DRAGON		94
#define BBSNET		95
#define SEVENCARD	96
#define CHESSMJ		97
#define NETGAME		98
#define BET		99
#define NoteMoney	100
#define LISTMAIN	101
#define LISTEDIT	102
#define FORTUNE		103
#define TOBUY		104
#define RELOAD		105

/* ----------------------------------------------------- */
/* menu.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

#define XEASY   0x333           /* Return value to un-redraw screen */
#define QUIT    0x666           /* Return value to abort recursive functions */


/* ----------------------------------------------------- */
/* read.c �����Ҧ�                                       */
/* ----------------------------------------------------- */

/* Read menu command return states */

#define RC_NONE		0	/* nothing to do */
#define RC_FULL		0x0100	/* Entire screen was destroyed in this oper */
#define RC_BODY		0x0200	/* Only the top three lines not destroyed */
#define RC_DRAW		0x0400
#define RC_FOOT		0x0800
#define RC_ITEM		0x1000
#define RC_CHDIR	0x2000	/* Index file was changed */
#define RC_NEWDIR	0x4000	/* Directory has changed, re-read files */

#define RS_FORWARD      0x01    /* backward */
#define RS_TITLE        0x02    /* author/title */
#define RS_RELATED      0x04
#define RS_FIRST        0x08    /* find first article */
#define RS_CURRENT      0x10    /* match current read article */
#define RS_THREAD       0x20    /* search the first article */
#define RS_AUTHOR       0x40    /* search author's article */
#define RS_SCORE        0x80    /* article score */

#define RS_NEXT         0x8000  /* Direct read next file */
#define RS_PREV         0x10000 /* Direct read prev file */

#define CURSOR_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST)
#define CURSOR_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD)
#define CURSOR_PREV     (RS_RELATED | RS_TITLE)
#define RELATE_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST | RS_CURRENT)
#define RELATE_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD | RS_CURRENT)
#define RELATE_PREV     (RS_RELATED | RS_TITLE | RS_CURRENT)
#define THREAD_NEXT     (RS_THREAD | RS_FORWARD)
#define THREAD_PREV     (RS_THREAD)
#define AUTHOR_NEXT     (RS_AUTHOR | RS_FORWARD)
#define AUTHOR_PREV     (RS_AUTHOR)

/*
woju
*/
#define POS_NEXT        101     /* cursor_pos(locmem, locmem->crs_ln + 1, 1);*/


/* for currmode */

#define MODE_STARTED    0x01    /* �O�_�w�g�i�J�t�� */
#define MODE_MENU	0x02    /* �O�_�b MENU */
#define MODE_TAG	0x04	/* */
#define	MODE_POST	0x08	/* �O�_�i�H�b currboard �o��峹 */
#define	MODE_BOARD	0x10	/* �O�_�i�H�b currboard �R���Bmark�峹 */
#define	MODE_SECRET	0x20	/* �O�_�� secret board */
#define	MODE_DIGEST	0x40	/* �O�_�� digest mode */
#define	MODE_DIRTY	0x80	/* �O�_��ʹL userflag */
#define MODE_SELECT     0x100 
#define MODE_FAVORITE	0x200	/* �O�_���ߦn�ݪO */
#define MODE_TINLIKE	0x400	/* �O�_�� tin-like �\Ū */

/* for curredit */

#define EDIT_MAIL       1       /* �ثe�O mail/board ? */
#define EDIT_LIST       2       /* �O�_�� mail list ? */
#define EDIT_BOTH       4       /* both reply to author/board ? */
#define EDIT_ITEM	8
#ifdef HAVE_ANONYMOUS
#define EDIT_ANONYMOUS 10       /* �ΦW�Ҧ� */
#endif

/* for Tag List */
#define TAG_COMP        0       /* ��� TagList */
#define TAG_TOGGLE      1       /* ���� TagList */
#define TAG_INSERT      2       /* �[�J TagList */

/* for announce.c */
#define GEM_FMODE         1       /* �ɮ׼Ҧ� */
#define GEM_PERM          2       /* �i�s��� */
#define GEM_TAG           4       /* �O���аO */
#define GEM_LOCK_PATH     8       /* ��w��ذϸ��| */
#define GEM_NET           16      /* ��ذϳs�u�� */
#define GEM_RELOAD        32      /* ��ذ� �j�� Reload */
#define GEM_BM            64	  /* �p�O�D */

#endif                          /* _MODES_H_ */
