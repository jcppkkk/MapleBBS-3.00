
/* 	�o�O ��WD(9806�H��)==>>WD990617 �� .PASSWDS ���{�� 
	�����s�� PASSWDS �|�X�{�b ~bbs/PASSWDS.NEW
 	�`�N :  �ϥΫe�аȥ��ƥ��{�� .PASSWDS
		�л\�ɮ׫e�вM�� SHM �ýT�w���W�L���� USER 	
						���a�a Ziyun   */
/*	wildcat patch : �䴩�h�� bbs version */
/* �аt�X�ۤv���t�Ϊ��� define */
#define	Current_WD	/* WD_BBS 0617 */
#undef	OLD_WD		/* OLD WD_BBS */
#undef	SOB		/* SOB Version */
#undef	Ptt		/* Ptt Version */

#include "bbs.h"


userec tuser;

struct new
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
  char title[20];
  usint five_win;
  usint five_lost;
  usint five_draw;
  char pad[91];			  /* �ŵ۶񺡦�512��      */
};

typedef struct new new;

main()
{
  int fdr,fdw,i=0;
  new new;
  
  fdr=open(BBSHOME"/.PASSWDS",O_RDONLY);
  fdw=open(BBSHOME"/PASSWDS.NEW",O_WRONLY | O_CREAT | O_TRUNC, 0644);
  printf("%d",sizeof(new));

  while(read(fdr,&tuser,sizeof(userec))==sizeof(userec))
  {
    if (strlen(tuser.userid) < 2) continue;
    if (not_alpha(*tuser.userid)) continue;
/*
    while (ch = *(++userid))
    {
      if (not_alnum(ch))
       continue;
    }
*/
        memcpy(new.userid,tuser.userid,IDLEN+1);
        memcpy(new.realname,tuser.realname,20); 
        memcpy(new.username,tuser.username,24); 
        memcpy(new.passwd,tuser.passwd,PASSLEN);
        new.uflag=tuser.uflag;                  
        new.userlevel=tuser.userlevel;          
        new.numlogins=tuser.numlogins;          
        new.numposts=tuser.numposts;            
  	new.firstlogin=tuser.firstlogin;    
  	new.lastlogin=tuser.lastlogin;      
  	memcpy(new.lasthost,tuser.lasthost,24);
  	memcpy(new.vhost,tuser.lasthost,24);   
  	memcpy(new.email,tuser.email,50);      
  	memcpy(new.address,tuser.address,50);  
  	memcpy(new.justify,tuser.justify,REGLEN + 1);  
  	new.month=tuser.month;              
  	new.day=tuser.day;                  
  	new.year=tuser.year;                
  	new.sex=tuser.sex;                  
  	new.state=tuser.state;              
  	new.pager=tuser.pager;          
  	new.invisible=tuser.invisible;  
  	new.exmailbox=tuser.exmailbox;  
#ifdef Current_WD
  	memcpy(new.toqid,tuser.userid,IDLEN+1);     
  	memcpy(new.beqid,tuser.userid,IDLEN+1);     
  	new.habit=tuser.habit;          
  	new.exmailboxk=tuser.exmailboxk;
  	new.toquery=tuser.toquery;      
  	new.bequery=tuser.bequery;      
  	new.totaltime=tuser.totaltime;  
  	new.sendmsg=tuser.sendmsg;      
  	new.receivemsg=tuser.receivemsg;
  	new.dtime=tuser.dtime;
	new.silvermoney=tuser.silvermoney;
#endif
#ifdef OLD_WD
  	memcpy(new.toqid,tuser.userid,IDLEN+1);     
  	memcpy(new.beqid,tuser.userid,IDLEN+1);     
  	new.habit=tuser.habit;          
  	new.exmailboxk=tuser.exmailboxk;
  	new.toquery=tuser.toquery;      
  	new.bequery=tuser.bequery;      
  	new.totaltime=tuser.totaltime;  
  	new.sendmsg=tuser.sendmsg;      
  	new.receivemsg=tuser.receivemsg;
  	new.dtime=tuser.dtime;
	new.silvermoney=(tuser.money+tuser.deposit);
#endif
#ifdef SOB
  	memcpy(new.toqid,0,IDLEN+1);     
  	memcpy(new.beqid,0,IDLEN+1);     
  	new.habit=0;
  	new.exmailboxk=0;
  	new.exmailbox=0;
  	new.toquery=0;
  	new.bequery=0;
  	new.totaltime=0;
  	new.sendmsg=0;
  	new.receivemsg=0;
  	new.dtime=0;
	new.silvermoney=1000;
#endif
#ifdef Ptt
  	memcpy(new.toqid,0,IDLEN+1);     
  	memcpy(new.beqid,0,IDLEN+1);     
  	new.habit=0;
  	new.exmailboxk=0;
  	new.toquery=0;
  	new.bequery=0;
  	new.totaltime=0;
  	new.sendmsg=0;
  	new.receivemsg=0;
  	new.dtime=0;
	new.silvermoney=tuser.money;
  	new.exmailbox=tuser.emailbox;
#endif
#if defined (OLD_WD) || (SOB) || (Ptt)
	new.exp=0;
	new.goldmoney=0;
	new.limitmoney=0;
	new.rtimes=0;
	new.award=0;
        memcpy(new.feeling,"?",4);
	new.pagermode=0;
	memcpy(new.pagernum,"000",6);
#else
//	new.exp=tuser.exp;
	new.exp=tuser.numlogins/10 + tuser.numposts/10 ;
	new.goldmoney=tuser.goldmoney;
	new.scoretimes=tuser.scoretimes;
	new.rtimes=tuser.rtimes;
	new.award=tuser.award;
        memcpy(new.feeling,tuser.feeling,5);
	new.pagermode=tuser.pagermode;
	memcpy(new.pagernum,tuser.pagernum,7);
	memcpy(new.title,tuser.title,20);
#endif
	new.five_win = new.five_lost = new.five_draw = 0;
        write(fdw,&new,sizeof(new));
        ++i;
//        if(!new.userid[0] && new.address[0])
//        printf("%-5d - %-13s %s\n",i,new.userid,new.address);
   }
   close(fdr);
   close(fdw);
}     
