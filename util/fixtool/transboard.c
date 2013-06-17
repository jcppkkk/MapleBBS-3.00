/* 	�o�O ��WD(9806�H��)==>>WD990617 �� .BOARDS ���{�� 
	�����s�� BOARDS �|�X�{�b ~bbs/BOARDS.NEW
 	�`�N :  �ϥΫe�аȥ��ƥ��{�� .BOARDS
		�л\�ɮ׫e�вM�� SHM �ýT�w���W�L���� USER 	
						���a�a Ziyun   */
/* �аt�X�ۤv���t�Ϊ��� define */

#include "bbs.h"

boardheader bh;

struct new
{
  char brdname[IDLEN + 1];      /* �ݪO�^��W��    13 bytes */
  char title[BTLEN + 1];        /* �ݪO����W��    49 bytes */
  char BM[IDLEN * 3 + 3];       /* �O�DID�M"/"     39 bytes */
  usint brdattr;                /* �ݪO���ݩ�       4 bytes */
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
  char pad[79];
};

typedef struct new new;

main()
{
  int fdr,fdw, i = 0;
  new new;
  
  fdr=open(BBSHOME"/.BOARDS",O_RDONLY);
  fdw=open(BBSHOME"/BOARDS.NEW",O_WRONLY | O_CREAT | O_TRUNC, 0644);

  while(read(fdr,&bh,sizeof(boardheader))==sizeof(boardheader))
  {     
  	i++;
	if(strlen(bh.brdname) == 0) continue;
	printf("
=====================================================
brd num   : %d
boardname : %s
title     : %s
totalvisit: %d
=====================================================\n"
,i,bh.brdname,bh.title,bh.totalvisit);
        memcpy(new.brdname,bh.brdname,IDLEN+1);
        memcpy(new.title,bh.title,BTLEN + 1);
        memcpy(new.BM,bh.BM,24);
        new.brdattr = bh.brdattr;  
        new.bupdate=bh.bupdate;
        new.bvote = bh.bvote;
        new.vtime=bh.vtime;
	new.level=bh.level;  
        new.totalvisit=bh.totalvisit;
        new.totaltime=bh.totaltime;
        memcpy(new.lastvisit,bh.lastvisit,IDLEN+1);
        new.opentime=bh.opentime;
        new.lastime=bh.lastime;
        memcpy(new.passwd,bh.passwd,PASSLEN); 
  	new.postotal=bh.postotal;  
        write(fdw,&new,sizeof(new));
   }
   close(fdr);
   close(fdw);
}     
