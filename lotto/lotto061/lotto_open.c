
/*
 lotto 遊戲開獎程式

 2002/2 寄中獎通知給user , 給錢.

 2002/3 完成統計部份

 2002/7/9 完成新增金銀幣選擇,簽賭站開關

 2002/8/17 修正統計數字過大無法show的問題
	   包牌中獎沒有在中獎函及公佈中show出
 2002/8/19 新增在開完獎號自動將lotto_on_off變成"on"
 2002/8/20 新增各人該期各項統計

 written by Worren.bbs@77bbs.com  逢甲大學七月七日晴BBS

 worren.feung@msa.hinet.net  http://worren.panfamily.org
 worren@77bbs.com 

*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "/home/bbs/src/include/bbs.h"
#include "lotto.h"

#define STR_DOTDIR ".DIR"
#define FN_PASSWD ".PASSWDS"
#define BUFSIZE 512
#define safewrite write
#define _BBS_UTIL_C

#undef HAVE_MMAP

struct UCACHE *uidshm;

struct manrec {
        char userid[IDLEN + 1];
        unsigned long int deposit;
        unsigned long int goldmoney;
        unsigned long int silvermoney;
};

typedef struct manrec manrec;
manrec current;

userec aman, xuser;

int usernumber;
char *str_home_file = "home/%s/%s";
char *str_dotdir = STR_DOTDIR;
char *fn_passwd = FN_PASSWD;
char *direct;

static
void
attach_err (shmkey, name)
        int shmkey;
        char *name;
{
        fprintf (stderr, "[%s error] key = %x\n", name, shmkey);
        exit (1);
}

static
void *
attach_shm (shmkey, shmsize)
        int shmkey, shmsize;
{
        void *shmptr;
        int shmid;
        shmid = shmget (shmkey, shmsize, 0);
        if (shmid < 0) {
                shmid = shmget (shmkey, shmsize, IPC_CREAT | 0600);
                if (shmid < 0)
                        attach_err (shmkey, "shmget");
                shmptr = (void *) shmat (shmid, NULL, 0);
                if (shmptr == (void *) -1)
                        attach_err (shmkey, "shmat");
                memset (shmptr, 0, shmsize);
        }
        else {
                shmptr = (void *) shmat (shmid, NULL, 0);
                if (shmptr == (void *) -1)
                        attach_err (shmkey, "shmat");
        }
        return shmptr;
}

substitute_record(fpath, rptr, size, id)
        char *fpath;
        char *rptr;
        int size, id;
{
        int fd;

        if ((fd = open(fpath, O_WRONLY | O_CREAT, 0644)) == -1)
                return -1;

        flock(fd, LOCK_EX);
        lseek(fd, (off_t) size * (id - 1), SEEK_SET);
        safewrite(fd, rptr, size);
        flock(fd, LOCK_UN);
        fsync(fd);
        close(fd);
        return 0;
}

int
apply_record(fpath, fptr, size)
        char *fpath;
        int (*fptr) ();
int size;
{
        char abuf[BUFSIZE];
        FILE* fp;

        if (!(fp = fopen(fpath, "r")))
                return -1;

        while (fread(abuf, 1, size, fp) == size)
                if ((*fptr) (abuf) == QUIT) {
                        fclose(fp);
                        return QUIT;
                }
        fclose(fp);
        return 0;
}


void
stampfile(fpath, fh)
        char *fpath;
        fileheader *fh;
{
        register char *ip = fpath;
        time_t dtime;
        struct tm *ptime;
        int fp;

        #if 1
        if (access(fpath, X_OK | R_OK | W_OK))
                mkdir(fpath, 0755);
        #endif
        time(&dtime);
        while (*(++ip));
                *ip++ = '/';
        do {
                sprintf(ip, "M.%d.A", ++dtime );
        } while ((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1);
        close(fp);
        memset(fh, 0, sizeof(fileheader));
        strcpy(fh->filename, ip);
        ptime = localtime(&dtime);
        sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}

int
get_record(fpath, rptr, size, id)
        char *fpath;
        char *rptr;
        int size, id;
{
        int fd;

        if ((fd = open(fpath, O_RDONLY, 0)) != -1) {
                if (lseek(fd, (off_t)(size * (id - 1)), SEEK_SET) != -1) {
                        if (read(fd, rptr, size) == size) {
                                close(fd);
                                return 0;
                        }
                }
                close(fd);
        }
        return -1;
}

int
do_append(fpath, record, size)
        char *fpath;
        fileheader *record;
        int size;
{
        int fd;

        if ((fd = open(fpath, O_WRONLY | O_CREAT, 0644)) == -1) {
                perror("error opening file...");
                return -1;
        }
        flock(fd, LOCK_EX);
        lseek(fd, (off_t)0, SEEK_END);
        safewrite(fd, record, size);
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
}

void
resolve_ucache ()
{
        if (uidshm == NULL) {
                uidshm = (struct UCACHE *) attach_shm (UIDSHM_KEY, sizeof (*uidshm));
                if (uidshm->touchtime == 0)
                        uidshm->touchtime = 1;
        }
        while (uidshm->uptime < uidshm->touchtime)
                reload_ucache ();
}

#ifndef HAVE_MMAP
static int
fillucache (uentp)
        userec *uentp;
{
        if (usernumber < MAXUSERS) {
                strncpy (uidshm->userid[usernumber], uentp->userid, IDLEN + 1);
                uidshm->userid[usernumber++][IDLEN] = '\0';
        }
        return 0;
}
#endif

reload_ucache ()
{
        if (uidshm->busystate) {
                // 其他 user 正在 flushing ucache ==> CSMA/CD
                if (uidshm->touchtime - uidshm->uptime > 30) {
                        uidshm->busystate = 0;  // leave busy state
                }
                else
                        sleep (1);
        }
        else {
                uidshm->busystate = 1;  // enter busy state
                usernumber = 0;
                apply_record (".PASSWDS", fillucache, sizeof (userec));
                uidshm->number = usernumber;

                // 等 user 資料更新後再設定 uptime

                uidshm->uptime = uidshm->touchtime;
                uidshm->busystate = 0;  // leave busy state
        }
}

void sethomepath(buf, userid)
        char *buf, *userid;
{
        sprintf(buf, "home/%s", userid);
}

int
ci_strcmp (s1, s2)
        register char *s1, *s2;
{
        register int c1, c2, diff;

        do {
                c1 = *s1++;
                c2 = *s2++;
                if (c1 >= 'A' && c1 <= 'Z')
                        c1 |= 32;
                if (c2 >= 'A' && c2 <= 'Z')
                        c2 |= 32;
                if (diff = c1 - c2)
                        return (diff);
        } while (c1);
        return 0;
}

void sethomedir(buf, userid)
        char *buf, *userid;
{
        sprintf(buf, str_home_file, userid, str_dotdir);
}

int
getuser (userid)
        char *userid;
{
        int uid;

        if(uid = searchuser(userid))
                get_record(fn_passwd, &xuser, sizeof (xuser), uid);

        return uid;
}

int
append_record(fpath, record, size)
        char *fpath;
        fileheader *record;
        int size;
{
        int fd , m,n;

        do_append(fpath,record,size);
        return 0;
}

int
searchuser (userid)
        char *userid;
{
        register char *ptr;
        register int i, j;

        resolve_ucache ();
        i = 0;
        j = uidshm->number;
        while (i < j) {
                ptr = uidshm->userid[i++];
                if (!ci_strcmp (ptr, userid)) {
                        strcpy (userid, ptr);
                        return i;
                }
        }
        return 0;
}

unsigned long int
give_prize(tuser, money)
        char *tuser;
        unsigned long int money;
{
        int unum;

        if (unum = getuser(tuser)) {
		if(LOTTO_GOLD_SILVER==0){
                   xuser.goldmoney += money;
                }
                else if(LOTTO_GOLD_SILVER==1){
                   xuser.silvermoney += money;
		}
                substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
		if(LOTTO_GOLD_SILVER==0){
                  return xuser.goldmoney;
		}
		else if(LOTTO_GOLD_SILVER==1){
                  return xuser.silvermoney;
		}
        }
        return -1;
}



mail_good_news( char *luck_user,char data_now[10][25],char lotto_num[11][30],char prize[5],unsigned long bonus_to_give,char string_time[20]){
FILE *fp;
FILE *lotto_rule;       //附在中獎通知後面的文件
fileheader mymail;
time_t now;
int i;
char buf[200];
char genbuf[200];


                        if((fp = fopen(LOTTO_PATH"lotto/lotto_open_tmp", "w+")) == NULL)
                                return;

//                      now = time(NULL) - 6 * 60;
                        sprintf(genbuf, "作者: 七七銀行樂透部\n");
                        fputs(genbuf, fp);
                        sprintf(genbuf, "標題: 恭喜你中獎啦~~\n");
                        fputs(genbuf, fp);
                        time(&now);
                        sprintf(genbuf, "時間: %s\n", ctime(&now));
                        fputs(genbuf, fp);
			if(LOTTO_GOLD_SILVER==0){
                            sprintf(genbuf, "您在 [1;33;40m%s %s [m簽了 [1;35;40m%s %s %s %s %s %s[m  , 押了七七金幣[1;36;40m %s[m元整",
                               lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                               lotto_num[8],lotto_num[9],lotto_num[3]);
			}
			else if(LOTTO_GOLD_SILVER==1){
                            sprintf(genbuf, "您在 [1;33;40m%s %s [m簽了 [1;35;40m%s %s %s %s %s %s[m  , 押了七七銀幣[1;36;40m %s[m元整",
                               lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                               lotto_num[8],lotto_num[9],lotto_num[3]);
			}
                        fputs(genbuf, fp);
                        if(strcmp(lotto_num[10],"電腦選號")==0){
                          fputs(" --電腦選號\n\n",fp);
                        }
                        else if(strcmp(lotto_num[10],"包牌選號")==0){
                          fputs(" --包牌選號\n\n",fp);
			}			
                        else fputs("\n\n",fp);
                        sprintf(genbuf, "本期第 [1;37;40m%s[m 期於 [1;36;40m%s[m 開獎,\n",string_time,data_now[1]);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "開獎號碼為 [1;32;41m%s %s %s %s %s %s[m 特別號 [1;33;44m%s[m  , 您中了 [1;31;40m%s [m獎\n\n",
                           data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],
                           data_now[8],prize);
                        fputs(genbuf, fp);
			if(LOTTO_GOLD_SILVER==0){
                           sprintf(genbuf, "您可以獨得彩金七七金幣 [1;36;41m%d[m元整\n\n\n\n",bonus_to_give);
			}
			else if(LOTTO_GOLD_SILVER==1){
                           sprintf(genbuf, "您可以獨得彩金七七銀幣 [1;36;41m%d[m元整\n\n\n\n",bonus_to_give);
			}
                        fputs(genbuf,fp);
                        /* 加入lotto_rule.txt */
                        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RULE);
                        if(lotto_rule=fopen(buf,"r")){
                            while(fgets(genbuf,200,lotto_rule)){
                                fputs(genbuf,fp);
                            }
                            fclose(lotto_rule);
                        }

                        fclose (fp);
                        sethomepath (genbuf, luck_user);
                        stampfile(genbuf, &mymail);
                        sprintf(buf,"%s%s",LOTTO_PATH,"lotto/lotto_open_tmp");
                        rename(buf, genbuf);
                        strcpy(mymail.owner, "[七七銀行]");
                        sprintf(mymail.title, "恭喜您中獎了--您中了七七樂透[1;31;40m%s[m獎",prize);
                        mymail.savemode = 0;
                        sethomedir(genbuf, luck_user);
                        append_record(genbuf, &mymail, sizeof(mymail));
}
	
add_stat_prize_amount_bonus(char userid[200],char prize_kind[5],unsigned long bonus_to_give){

FILE *fp;
char buf[200];
int i;
unsigned long li;
    
    li=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_%s_bonus",LOTTO_PATH,userid,prize_kind);
    if(fp=fopen(buf,"r")){
    	fgets(buf,180,fp);
    	li=atol(buf);
    	fclose(fp);
    }
    else li=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_%s_bonus",LOTTO_PATH,userid,prize_kind);
    fp=fopen(buf,"w");
    li=li+bonus_to_give;
    sprintf(buf,"%ld",li);
    fputs(buf,fp);
    fclose(fp);
    
    li=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_bonus",LOTTO_PATH,userid);
    if(fp=fopen(buf,"r")){
    	fgets(buf,180,fp);
    	li=atol(buf);
    	fclose(fp);
    }
    else li=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_bonus",LOTTO_PATH,userid);
    fp=fopen(buf,"w");
    li=li+bonus_to_give;
    sprintf(buf,"%ld",li);
    fputs(buf,fp);
    fclose(fp);

    i=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_%s_amount",LOTTO_PATH,userid,prize_kind);
    if(fp=fopen(buf,"r")){
    	fgets(buf,180,fp);
    	i=atoi(buf);
    	fclose(fp);
    }
    else i=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_%s_amount",LOTTO_PATH,userid,prize_kind);
    fp=fopen(buf,"w");
    i++;
    sprintf(buf,"%d",i);
    fputs(buf,fp);
    fclose(fp);  	    
}




main (){

FILE *get_now;          //即時開獎號碼
FILE *this_record;      //本期簽賭記錄
FILE *record_history;   //簽賭記錄丟進history目錄裡
FILE *lotto_open_num;   //各期開獎記錄
FILE *lotto_open;       //中獎名單
FILE *lotto_stat;       //統計結果
FILE *lotto_top;
FILE *fp;		//temp

fileheader mymail;

char data_now[10][25];  //讀get_now(get_lotto_now.txt)的資料
char lotto_num[11][30];  //各個簽賭的號碼

int luck_num[5];        //對中的號碼
int luck_num_s;         //特別號旗標, 沒對中==0, 對中==1

long gamble_money;      //押金

int i=0;        //暫存用的整數0
int j=0;        //暫存用的整數1
int k=0;        //............2
unsigned long count_1=0;  //得頭獎總數
unsigned long count_2=0;  //2獎
unsigned long count_3=0;
unsigned long count_4=0;
unsigned long count_5=0;
unsigned long count_6=0;
unsigned long count_all=0;        //此期總共簽了幾支
unsigned long count_in=0;                //收入
unsigned long count_out=0;       //支出
float precent_lotto=0;  //得獎率

int lotto_no_statistics[80][2]; //統計各號碼出現次數
int lotto_no_s[80][2];          //統計特別號
int statistics[80];                     //暫存陣列
int s_stat[80];                         //特別號的暫存陣列

int h=0;

char buf[200];
char buf2[200];
char buf3[200];
char data_read[100];
char *p;
char *p2;
char lotto_define[8][2]; //統計 ,1,2,3,4,5,6獎, 1=寄,0=不寄
char lotto_stat_echo[9][2][10]; //統計各項個人數據用!
char luck_user[50];

unsigned long bonus_to_give;    //彩金
char genbuf[200];
char string_time[20];           //期數==data_now[0], 修正奇怪的bug

savelog("開始跑lotto_open");

/* 清除所有暫存檔 */
sprintf(buf,"/bin/rm -f %slotto/tmp/*",LOTTO_PATH);
system(buf);

/*讀取lotto__no_now.txt的資料*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
        printf("讀不到檔");
        exit(0);
   }
   fgets(data_read,79,get_now);
   fclose(get_now);
   if(strlen(data_read)<35) {
        printf("讀沒有資料");
        exit(0);
   }
   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
   }
   strcpy(string_time,data_now[0]);

//讀取data_now完畢

  /* 要開始開獎了, 把lotto_on_off.txt內容改成"off" */
	sprintf(buf,"/bin/echo off > %s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
        system(buf);



  /* 開啟簽賭歷史資料 */
  sprintf(buf,"%slotto/history/lotto_record_%s.txt",LOTTO_PATH,data_now[0]);
  record_history=fopen(buf,"a+");

/* 開始讀取簽賭資料並核對中獎且發給獎金 */
  sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
//  if(!(this_record=fopen(buf,"r")))  exit(0);
  if(!(this_record=fopen(buf,"r")))  this_record=fopen(buf,"a+");
//--->先寫到這裡,累了..睡吧!91/02/21

  if(!(fgets(buf,180,this_record)))  ; //把第一行的xx期先讀起來!
  fputs(buf,record_history);

/* 開啟中獎名單 */
  sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_OPEN);
  lotto_open=fopen(buf,"w");            //開啟中獎名單檔
  sprintf(buf,"第 %s 期 七七樂透中獎名單\n",string_time);
  fputs(buf,lotto_open);
  sprintf(buf,"本期開獎號碼 : [1;32;41m %s %s %s %s %s %s [m 特別號 : [1;33;44m%s[m\n\n"
        ,data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],data_now[8]);
  fputs(buf,lotto_open);

  while(fgets(buf,180,this_record)){

    count_all++;
    strcpy(lotto_num[10],"");
    fputs(buf,record_history);
    sprintf(lotto_num[0],"%s",strtok(buf,";"));
    i=0;
        strcpy(string_time,data_now[0]);        //修正bug...很奇怪, 遇上特定的資料, data_now[0]就是會變成空白狀
    while(p2=strtok(NULL,";")){
      i++;
      sprintf(lotto_num[i],"%s",p2);
    }

    count_in=count_in+atoi(lotto_num[3]);


    k=0;                //記錄對中號碼級數, 一般號 +1, 特別號 則lotto_num_s=1
    luck_num_s=0;       //特別號旗標=0(沒對中)


    for (i=4;i<10;i++){
                for(j=2;j<8;j++){
              if(atoi(lotto_num[i])==atoi(data_now[j])){
                          k++;
                          break;
                  }
                }
                if(atoi(lotto_num[i])==atoi(data_now[8])){
              luck_num_s=1;
                }
    }

    strcpy(current.userid, lotto_num[0]);
    /* 加總user的下注總合,下注數及把user加入此期的player list 2002/8/19*/    
    //讀取目前user下注總合i

    sprintf(buf,"%slotto/tmp/statistics_%s_gamble",LOTTO_PATH,current.userid);
    if(fp=fopen(buf,"r")){
	fgets(buf,180,fp);
	i=atoi(buf);
	fclose(fp);
    }
    else{
    	fp=fopen(LOTTO_PATH"lotto/tmp/statistics_list","a");	//list
    	sprintf(buf,"%s\n",current.userid);
    	fputs(buf,fp);
    	fclose(fp);
    	i=0;
    }

    sprintf(buf,"%slotto/tmp/statistics_%s_gamble",LOTTO_PATH,current.userid);    
    fp=fopen(buf,"w");
    i=i+atoi(lotto_num[3]);
    sprintf(buf,"%d\n",i);
    fputs(buf,fp);
    fclose(fp);
    sprintf(buf,"%slotto/tmp/statistics_%s_amount",LOTTO_PATH,current.userid);
    if(fp=fopen(buf,"r")){
    	fgets(buf,180,fp);
    	i=atoi(buf);
    	fclose(fp);
    }
    else i=0;
    sprintf(buf,"%slotto/tmp/statistics_%s_amount",LOTTO_PATH,current.userid);
    fp=fopen(buf,"w");
    i=i++;
    sprintf(buf,"%d\n",i);
    fputs(buf,fp);
    fclose(fp);        	    
    
    //下注總金額,下注數& list 程式end

    /* 取得user各通知信涵的設定值 */
    sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_MAIL_DEFINE);   	
    if(!(fp=fopen(buf,"r"))){
	sprintf(lotto_define[0],"%d",MAIL_S);
	sprintf(lotto_define[1],"%d",MAIL_1);
	sprintf(lotto_define[2],"%d",MAIL_2);
	sprintf(lotto_define[3],"%d",MAIL_3);
	sprintf(lotto_define[4],"%d",MAIL_4);
	sprintf(lotto_define[5],"%d",MAIL_5);
	sprintf(lotto_define[6],"%d",MAIL_6);
    }
    else{
     i=0;
     while(fgets(buf,180,fp)){
       strcpy(buf2,"");
       sprintf(buf2,"%s",strtok(buf,";"));       
       if(strcmp(buf2,current.userid)==0){	 
	 while((p=strtok(NULL,";"))){
          if (!p) break;
          sprintf(lotto_define[i],"%s",p);
          i++;	  
         }
       }
       if(i>0) break;
       i=0;
     }
     fclose(fp);
     if (i==0){
	sprintf(lotto_define[0],"%d",MAIL_S);
	sprintf(lotto_define[1],"%d",MAIL_1);
	sprintf(lotto_define[2],"%d",MAIL_2);
	sprintf(lotto_define[3],"%d",MAIL_3);
	sprintf(lotto_define[4],"%d",MAIL_4);
	sprintf(lotto_define[5],"%d",MAIL_5);
	sprintf(lotto_define[6],"%d",MAIL_6);
     }   
    }
    //設定值讀取完畢


    switch(k){

        case 2:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_6;
                  give_prize(current.userid,bonus_to_give);                  
                  if(strcmp(lotto_define[6],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"六",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了六獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

		  if(LOTTO_GOLD_SILVER==0){		
                      if(strcmp(lotto_num[10],"電腦選號")==0){
                        sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                      }
                      else if(strcmp(lotto_num[10],"包牌選號")==0){
                        sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
                      }
                      else{
                        sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                      }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
                      if(strcmp(lotto_num[10],"電腦選號")==0){
                        sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
                      }
		      else if(strcmp(lotto_num[10],"包牌選號")==0){
			sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
		      }
                      else{
                        sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
                      }
		  }

                  fputs(buf,lotto_open);
                  count_6++;
                  count_out=count_out+bonus_to_give;
		  add_stat_prize_amount_bonus(current.userid,"6",bonus_to_give);		  
                  break;
        case 3:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_5;
                  give_prize(current.userid,bonus_to_give);
                  if(strcmp(lotto_define[5],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"五",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了五獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
                    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
		       sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
		       sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
                    }
		  }
                  fputs(buf,lotto_open);
                  count_5++;
                  count_out=count_out+bonus_to_give;
		  add_stat_prize_amount_bonus(current.userid,"5",bonus_to_give);
                  break;
        case 4:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_4;
                  give_prize(current.userid,bonus_to_give);
                  if(strcmp(lotto_define[4],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"四",bonus_to_give,string_time);

                  sprintf(buf,"%s 簽了  [1;32;41m%s %s %s %s %s %s[m  %s元,中了四獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

 		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
 		       sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
		    }

                    else{
                       sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
 		       sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
                    }
		  }

                  fputs(buf,lotto_open);
                  count_4++;
                  count_out=count_out+bonus_to_give;
        	  add_stat_prize_amount_bonus(current.userid,"4",bonus_to_give);
		  break;
        case 5:
                  if(luck_num_s==0){
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_3;
                          give_prize(current.userid,bonus_to_give);
                          if(strcmp(lotto_define[3],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"三",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了三獎,"
                         ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                   fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
                       sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
                    }
                    else{
                       sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
		       sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
 		    }
                    else{
                       sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
                    }
		  }

                   fputs(buf,lotto_open);
                   count_3++;
                   count_out=count_out+bonus_to_give;
                   add_stat_prize_amount_bonus(current.userid,"3",bonus_to_give);
                   break;
                  }
                  else{
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_2;
                          give_prize(current.userid,bonus_to_give);
                          if(strcmp(lotto_define[2],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"二",bonus_to_give,string_time);
                          sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了二獎,"
                                ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                                ,lotto_num[8],lotto_num[9],lotto_num[3]);
                          fputs(buf,lotto_open);
 		  	if(LOTTO_GOLD_SILVER==0){
			    if(strcmp(lotto_num[10],"電腦選號")==0){
        	               sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                	    }
			    else if(strcmp(lotto_num[10],"包牌選號")==0){
			       sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
			    }
	                    else{
        	               sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                	    }
			  }
			  else if(LOTTO_GOLD_SILVER==1){
			    if(strcmp(lotto_num[10],"電腦選號")==0){
	                       sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
        	            }
			    else if(strcmp(lotto_num[10],"包牌選號")==0){
 		               sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
			    }
                	    else{
	                       sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
        	            }
			  }

                          fputs(buf,lotto_open);
                          count_2++;
                          count_out=count_out+bonus_to_give;
                          add_stat_prize_amount_bonus(current.userid,"2",bonus_to_give);
                          break;

                  }
        case 6:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_1;
                  give_prize(current.userid,bonus_to_give);
                  if(strcmp(lotto_define[1],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"頭",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了頭獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
		       sprintf(buf,"可得彩金 七七金幣 %ld元 --包牌選號\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"電腦選號")==0){
                       sprintf(buf,"可得彩金 七七銀幣 %ld元 --電腦選號\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"包牌選號")==0){
 		       sprintf(buf,"可得彩金 七七銀幣 %ld元 --包牌選號\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"可得彩金 七七銀幣 %ld元\n",bonus_to_give);
                    }
		  }

                  fputs(buf,lotto_open);
                  count_1++;
                  count_out=count_out+bonus_to_give;
                  add_stat_prize_amount_bonus(current.userid,"1",bonus_to_give);
                  break;
        default:
          break;

    }

    h++;
//取消此行訊息,以免拖慢開獎的速度!若您想隨時監看開獎到第幾筆,可以把這行開啟!
    printf("No. %d data  completed..\n",h);
  }
        fclose(this_record);

  sprintf(buf,"共有%ld筆資料,處理完畢,開始統計作業...",count_all);
  savelog(buf);

        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  // 將開完獎的記錄檔remove
        remove(buf);

        /* 已經開完獎了, 把lotto_on_off.txt內容改成"on" */
	sprintf(buf,"/bin/echo on > %s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
        system(buf);


        /* 將開獎號碼記進 lotto_no_history.txt */
        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_HISTORY);
        lotto_open_num=fopen(buf,"a");
        sprintf(buf,"%s;%s;%d;%d;%d;%d;%d;%d;%d;\n",string_time,data_now[1],
                atoi(data_now[2]),atoi(data_now[3]),atoi(data_now[4]),
                atoi(data_now[5]),atoi(data_now[6]),atoi(data_now[7]),
                atoi(data_now[8]));

        fputs(buf,lotto_open_num);
        fclose(lotto_open_num);
        fclose(lotto_open);

        /* 將統計結果寫入 lotto_stat.txt */
        sprintf(buf,"%slotto/lotto_stat.txt",LOTTO_PATH);
        lotto_stat=fopen(buf,"w");
        sprintf(buf,"第 %s 期 七七樂透開獎各項統計\n\n",string_time);
        fputs(buf,lotto_stat);
        sprintf(buf," 頭獎得獎數 : %ld注\n",count_1);
        fputs(buf,lotto_stat);
        sprintf(buf," 貳獎得獎數 : %ld注\n",count_2);
        fputs(buf,lotto_stat);
        sprintf(buf," 參獎得獎數 : %ld注\n",count_3);
        fputs(buf,lotto_stat);
        sprintf(buf," 肆獎得獎數 : %ld注\n",count_4);
        fputs(buf,lotto_stat);
        sprintf(buf," 伍獎得獎數 : %ld注\n",count_5);
        fputs(buf,lotto_stat);
        sprintf(buf," 陸獎得獎數 : %ld注\n\n",count_6);
        fputs(buf,lotto_stat);
        sprintf(buf," 總下注數   : %ld注\n\n",count_all);
        fputs(buf,lotto_stat);
        sprintf(buf," 賭金總收入 : %ld元\n",count_in);
        fputs(buf,lotto_stat);
        sprintf(buf," 彩金總支出 : %ld元\n",count_out);
        fputs(buf,lotto_stat);
        precent_lotto=((float)(count_1 + count_2 + count_3 + count_4 + count_5 + count_6) / (float)count_all) * 100;
        sprintf(buf," 本期中獎率 : %f％\n\n",precent_lotto);
        fputs(buf,lotto_stat);

        /* 統計各號碼出現次數及排名 */
        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_HISTORY);
        lotto_open_num=fopen(buf,"r");
           /*初始化陣列*/
           for(i=0;i<(LOTTO_NUM_MAX+1);i++){
                statistics[i]=0;
                s_stat[i]=0;
           }


           sprintf(buf,"-=-=-=-=-各號碼出現次數統計-=-=-=-=-=-\n\n");
           fputs(buf,lotto_stat);
           while(fgets(buf,180,lotto_open_num)){
                i=0;
                sprintf(data_now[i],"%s",strtok(buf,";"));
                while((p=strtok(NULL,";"))){
                     i++;
                     sprintf(data_now[i],"%s",p);
                }
                for(j=2;j<8;j++){
                   statistics[atoi(data_now[j])]++;
                }
                s_stat[atoi(data_now[8])]++;
           }

           /* 寫入各號碼出現次數 */
           i=0;
           j=0;
           while(i<LOTTO_NUM_MAX){
             strcpy(buf,"");
             j=0;
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d次",buf,i,statistics[i]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }
           fputs("\n",lotto_stat);

           /*排序出現次數*/
           i=0;
           while(i<LOTTO_NUM_MAX){
             i++;
             lotto_no_statistics[i][1]=statistics[i];
             lotto_no_statistics[i][0]=i;
           }
           //開始排, 氣泡

           for(i=(LOTTO_NUM_MAX-1);i>=1;i=i-1){
             for(j=1;j<=i;j++){
                if(lotto_no_statistics[j][1]<lotto_no_statistics[j+1][1]){
                   lotto_no_s[79][0]=lotto_no_statistics[j][0];
                   lotto_no_s[79][1]=lotto_no_statistics[j][1];
                   lotto_no_s[80][0]=lotto_no_statistics[j+1][0];
                   lotto_no_s[80][1]=lotto_no_statistics[j+1][1];
                   lotto_no_statistics[j][0]=lotto_no_s[80][0];
                   lotto_no_statistics[j][1]=lotto_no_s[80][1];
                   lotto_no_statistics[j+1][0]=lotto_no_s[79][0];
                   lotto_no_statistics[j+1][1]=lotto_no_s[79][1];
                }
             }
           }

           fputs("-=-=-=-=-開獎號碼出現次數排列-=-=-=-=-=-=\n\n",lotto_stat);
           strcpy(buf,"");
           sprintf(buf,"[1;33;40m1st,%d:%d次    2ed,%d:%d次    3rd,%d:%d次[m\n",
           lotto_no_statistics[1][0],lotto_no_statistics[1][1],
           lotto_no_statistics[2][0],lotto_no_statistics[2][1],
           lotto_no_statistics[3][0],lotto_no_statistics[3][1]);

           fputs(buf,lotto_stat);
	   sprintf(buf,"%setc/lotto_top.txt",LOTTO_PATH);
	   lotto_top=fopen(buf,"w");
	   sprintf(buf,"%d\n",lotto_no_statistics[1][0]);
	   fputs(buf,lotto_top);
           sprintf(buf,"%d\n",lotto_no_statistics[1][1]);
           fputs(buf,lotto_top);
           sprintf(buf,"%d\n",lotto_no_statistics[2][0]);
           fputs(buf,lotto_top);
           sprintf(buf,"%d\n",lotto_no_statistics[2][1]);
           fputs(buf,lotto_top);
           sprintf(buf,"%d\n",lotto_no_statistics[3][0]);
           fputs(buf,lotto_top);
           sprintf(buf,"%d\n",lotto_no_statistics[3][1]);
           fputs(buf,lotto_top);
	   fclose(lotto_top);

           i=3;
           while(i<LOTTO_NUM_MAX){
             j=0;
             strcpy(buf,"");
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d次",buf,lotto_no_statistics[i][0],lotto_no_statistics[i][1]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }

           fputs("\n",lotto_stat);

           sprintf(buf,"-=-=-=-=-特別號出現次數統計-=-=-=-=-=-\n\n");
           fputs(buf,lotto_stat);

           /* 寫入各號碼出現次數 */
           i=0;
           j=0;
           while(i<LOTTO_NUM_MAX){
             strcpy(buf,"");
             j=0;
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d次",buf,i,s_stat[i]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }
           fputs("\n",lotto_stat);

           /*排序出現次數*/
           i=0;
           while(i<LOTTO_NUM_MAX){
             i++;
             lotto_no_statistics[i][1]=s_stat[i];
             lotto_no_statistics[i][0]=i;
           }
           //開始排, 氣泡

           for(i=(LOTTO_NUM_MAX-1);i>=1;i=i-1){
             for(j=1;j<=i;j++){
                if(lotto_no_statistics[j][1]<lotto_no_statistics[j+1][1]){
                   lotto_no_s[79][0]=lotto_no_statistics[j][0];
                   lotto_no_s[79][1]=lotto_no_statistics[j][1];
                   lotto_no_s[80][0]=lotto_no_statistics[j+1][0];
                   lotto_no_s[80][1]=lotto_no_statistics[j+1][1];
                   lotto_no_statistics[j][0]=lotto_no_s[80][0];
                   lotto_no_statistics[j][1]=lotto_no_s[80][1];
                   lotto_no_statistics[j+1][0]=lotto_no_s[79][0];
                   lotto_no_statistics[j+1][1]=lotto_no_s[79][1];
                }
             }
           }

           fputs("-=-=-=-=-特別號出現次數排列-=-=-=-=-=-=\n\n",lotto_stat);
           strcpy(buf,"");
           sprintf(buf,"[1;33;40m1st,%d:%d次    2ed,%d:%d次    3rd,%d:%d次[m\n",
           lotto_no_statistics[1][0],lotto_no_statistics[1][1],
           lotto_no_statistics[2][0],lotto_no_statistics[2][1],
           lotto_no_statistics[3][0],lotto_no_statistics[3][1]);

           fputs(buf,lotto_stat);
           i=3;
           while(i<LOTTO_NUM_MAX){
             j=0;
             strcpy(buf,"");
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d次",buf,lotto_no_statistics[i][0],lotto_no_statistics[i][1]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }

        fclose(lotto_stat);

        /*貼上統計結果*/
        sprintf(buf,"%sbin/post_open",LOTTO_PATH);
        system(buf);

	savelog("完成總統計,開始個人統計...");

        /* 開始寄統計通知涵 */        
        /*
        if(!fp=fopen(LOTTO_PATH"lotto/tmp/statistics_list","r")){
        	 exit;
        }
        */
        fp=fopen(LOTTO_PATH"lotto/tmp/statistics_list","r");
        while(fgets(buf,50,fp)){	    
	    buf[(strlen(buf)-1)]='\0';
	    strcpy(luck_user,buf);
            sprintf(buf2,"%slotto/tmp/statistics_%s_gamble",LOTTO_PATH,luck_user);
            get_now=fopen(buf2,"r");
	    fgets(buf2,180,get_now);
            count_out=atol(buf2);      //總支出
            fclose(get_now);
            sprintf(buf2,"%slotto/tmp/statistics_%s_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
              fgets(buf2,180,get_now);
              count_in=atol(buf2);       //總收入
              fclose(get_now);
            }
            else count_in=0;
            sprintf(buf2,"%slotto/tmp/statistics_%s_amount",LOTTO_PATH,luck_user);
            get_now=fopen(buf2,"r");
            fgets(buf2,180,get_now);
            count_all=atol(buf2);      //總下注數
            fclose(get_now);
            //頭獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_1_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[1][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[1][0],"0");
            //二獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_2_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[2][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[2][0],"0");
            //3獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_3_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[3][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[3][0],"0");
            //4獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_4_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[4][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[4][0],"0");
            //5獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_5_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[5][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[5][0],"0");
            //6獎數
            sprintf(buf2,"%slotto/tmp/statistics_%s_6_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[6][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[6][0],"0");
            //頭獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_1_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[1][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[1][1],"0");
            //2獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_2_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[2][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[2][1],"0");
            //3獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_3_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[3][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[3][1],"0");
            //4獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_4_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[4][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[4][1],"0");
            //5獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_5_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[5][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[5][1],"0");
            //6獎金
            sprintf(buf2,"%slotto/tmp/statistics_%s_6_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[6][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[6][1],"0");
            precent_lotto=(((float)(atol(lotto_stat_echo[1][0])+atol(lotto_stat_echo[2][0])+
                           atol(lotto_stat_echo[3][0])+atol(lotto_stat_echo[4][0])+
                           atol(lotto_stat_echo[5][0])+atol(lotto_stat_echo[6][0])))/((float)count_all))*100;
            sprintf(buf2,"%slotto/tmp/stat_mail_%s",LOTTO_PATH,luck_user);	//此處的buf是username
            get_now=fopen(buf2,"w");
            sprintf(buf2,"親愛的 %s :\n\n",luck_user);
            fputs(buf2,get_now);
            sprintf(buf2,"      您在本期,第%s期,的下注及中獎統計如下...\n\n",string_time);
            fputs(buf2,get_now);
            sprintf(buf2,"      總下注數   : %ld\t 總下注金額  : %ld\n",count_all,count_out);
            fputs(buf2,get_now);
            sprintf(buf2,"      頭獎中獎數 : %s\t\t 頭獎彩金    : %s\n",lotto_stat_echo[1][0],lotto_stat_echo[1][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      二獎中獎數 : %s\t\t 二獎彩金    : %s\n",lotto_stat_echo[2][0],lotto_stat_echo[2][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      三獎中獎數 : %s\t\t 三獎彩金    : %s\n",lotto_stat_echo[3][0],lotto_stat_echo[3][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      四獎中獎數 : %s\t\t 四獎彩金    : %s\n",lotto_stat_echo[4][0],lotto_stat_echo[4][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      五獎中獎數 : %s\t\t 五獎彩金    : %s\n",lotto_stat_echo[5][0],lotto_stat_echo[5][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      六獎中獎數 : %s\t\t 六獎彩金    : %s\n\n",lotto_stat_echo[6][0],lotto_stat_echo[6][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      本期中獎率 : %f％\t\t 總彩金     : %ld\n",precent_lotto,count_in);
            fputs(buf2,get_now);
            if (count_out > count_in) sprintf(buf2,"      本期您賠了 : %ld元\n",(count_out-count_in));
            else sprintf(buf2,"      本期您賺了 : %ld元\n",(count_in-count_out));
            fputs(buf2,get_now);
            fclose(get_now);
	    //資料寄出
            sethomepath (buf2, luck_user);
            stampfile(buf2, &mymail);
            sprintf(buf3,"%slotto/tmp/stat_mail_%s",LOTTO_PATH,luck_user);	//此處的buf是username
            rename(buf3, buf2);
            strcpy(mymail.owner, "[七七銀行]");
            sprintf(mymail.title, "七七樂透第%s期個人各項數據統計",string_time);
            mymail.savemode = 0;
            sethomedir(buf2, luck_user);
            append_record(buf2, &mymail, sizeof(mymail));
          
        }
	fclose(fp);
        savelog("個人統計完成,lotto_open執行完畢!!");

}

//log檔副程式
savelog(char what_log[200]){

FILE *updatelogtime ;
char nowt[200];
char *wday[]={"星期日","星期一","星期二","星期三","星期四","星期五","星期六"};

time_t timep;
struct tm *p;
time(&timep);

updatelogtime= fopen(LOTTO_PATH"lotto/lotto_open.log","a+");

p = localtime(&timep);

sprintf(nowt,"於%d/%d/%d %s %d:%d:%d %s \n",(1900+p->tm_year),
        (1+p->tm_mon),p->tm_mday,wday[p->tm_wday],p->tm_hour,p->tm_min,
        p->tm_sec,what_log);

fputs(nowt,updatelogtime);
fclose(updatelogtime);

}

