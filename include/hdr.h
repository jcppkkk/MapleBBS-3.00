#ifndef	_HDR_H_
#define	_HDR_H_


#include <sys/types.h>


/* ----------------------------------------------------- */
/* DIR of post / mail struct : 256 bytes		 */
/* ----------------------------------------------------- */


typedef struct
{
  time_t chrono;		/* timestamp */
  int xmode;

  int xid;			/* reserved */

  char xname[32];		/* �ɮצW�� */
  char owner[80];		/* �@�� (E-mail address) */
  char nick[50];		/* �ʺ� */

  char date[9];			/* [96/12/01] */
  char title[73];		/* �D�D (TTLEN + 1) */
}          HDR;


/* gopher url �r��Gxname + owner + nick + date */

#define	GEM_URLEN	(32 + 80 + 50 + 9 - 1)


/* ----------------------------------------------------- */
/* post.xmode ���w�q					 */
/* ----------------------------------------------------- */


#if 0
#define FILE_LOCAL	0x01	/* local saved */
#define FILE_MARKED	0x02	/* marked */
#define FILE_DIGEST	0x04	/* digest */

#define FILE_CANCEL	0x40	/* canceled */
#define FILE_DELETE	0x80	/* deleted */
#endif


#define POST_READ	0x0001	/* already read */
#define POST_MARKED	0x0002	/* marked */
#define POST_GEM	0x0004	/* gemed */
#define POST_CANCEL	0x0040	/* canceled */
#define POST_DELETE	0x0080	/* deleted */

#define	POST_INCOME	0x0100	/* ��H�i�Ӫ� */
#define	POST_EMAIL	0x0200	/* E-mail post �i�Ӫ� */
#define	POST_OUTGO	0x0400	/* ����H�X�h */
#define	POST_RESTRICT	0x0800	/* ����Ť峹�A�� manager �~��� */


/* ----------------------------------------------------- */
/* mail.xmode ���w�q					 */
/* ----------------------------------------------------- */


#define MAIL_READ	0x0001	/* already read */
#define MAIL_MARKED	0x0002	/* marked */
#define MAIL_REPLIED	0x0004	/* �w�g�^�L�H�F */
#define	MAIL_MULTI	0x0008	/* mail list */
#define	MAIL_HOLD	0x0010	/* �ۦs���Z */
#define	MAIL_NOREPLY	0x0020	/* ���i�^�H */
#define MAIL_DELETE	0x0080	/* �N�D�R�� */

#define	MAIL_INCOME	0x0100	/* bbsmail �i�Ӫ� */


/* ----------------------------------------------------- */
/* gem(gopher).xmode ���w�q				 */
/* ----------------------------------------------------- */


#define	GEM_FULLPATH	0x00008000	/* ������| */

#define	GEM_RESTRICT	0x0800		/* ����ź�ذϡA�� manager �~��� */
#define	GEM_RESERVED	0x1000		/* ����ź�ذϡA�� sysop �~���� */

#define	GEM_FOLDER	0x00010000	/* folder / article */
#define	GEM_BOARD	0x00020000	/* �ݪO��ذ� */
// #define	GEM_GOPHER	0x00040000	/* gopher */
#define	GEM_HTTP	0x00080000	/* http */
#define	GEM_EXTEND	0x80000000	/* ������ URL */


#define	HDR_URL		(GEM_GOPHER | GEM_HTTP)
#define	HDR_LINK	0x400		/* link() */


#endif//	_HDR_H_
