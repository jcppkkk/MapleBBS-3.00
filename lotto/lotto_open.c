
/*
 lotto 遊戲開獎程式

 2002/2 寄中獎通知給user , 給錢.

 2002/3 完成統計部份

 written by Worren.bbs@77bbs.com  逢甲大學七月七日晴BBS

 worren@worren.2y.net  http://worren.2y.net
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
                xuser.goldmoney += money;
                substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
                return xuser.goldmoney;
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
                        sprintf(genbuf, "您在 [1;33;40m%s %s [m簽了 [1;35;40m%s %s %s %s %s %s[m  , 押了七七金幣[1;36;40m %s[m元整",
                           lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                           lotto_num[8],lotto_num[9],lotto_num[3]);
                        fputs(genbuf, fp);
                        if(strcmp(lotto_num[10],"電腦選號")==0){
                          fputs(" --電腦選號\n\n",fp);
                        }
                        else fputs("\n\n",fp);
                        sprintf(genbuf, "本期第 [1;37;40m%s[m 期於 [1;36;40m%s[m 開獎,\n",string_time,data_now[1]);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "開獎號碼為 [1;32;41m%s %s %s %s %s %s[m 特別號 [1;33;44m%s[m  , 您中了 [1;31;40m%s [m獎\n\n",
                           data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],
                           data_now[8],prize);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "您可以獨得彩金七七金幣 [1;36;41m%d[m元整\n\n\n\n",bonus_to_give);
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


main (){

FILE *get_now;          //即時開獎號碼
FILE *this_record;      //本期簽賭記錄
FILE *record_history;   //簽賭記錄丟進history目錄裡
FILE *lotto_open_num;   //各期開獎記錄
FILE *lotto_open;       //中獎名單
FILE *lotto_stat;       //統計結果
FILE *lotto_top;

char data_now[10][25];  //讀get_now(get_lotto_now.txt)的資料
char lotto_num[11][30];  //各個簽賭的號碼

int luck_num[5];        //對中的號碼
int luck_num_s;         //特別號旗標, 沒對中==0, 對中==1

long gamble_money;      //押金

int i=0;        //暫存用的整數0
int j=0;        //暫存用的整數1
int k=0;        //............2
int count_1=0;  //得頭獎總數
int count_2=0;  //2獎
int count_3=0;
int count_4=0;
int count_5=0;
int count_6=0;
int count_all=0;        //此期總共簽了幾支
long count_in=0;                //收入
long count_out=0;       //支出
float precent_lotto=0;  //得獎率

int lotto_no_statistics[80][2]; //統計各號碼出現次數
int lotto_no_s[80][2];          //統計特別號
int statistics[80];                     //暫存陣列
int s_stat[80];                         //特別號的暫存陣列

int h=0;

char buf[200];
char data_read[100];
char *p;
char *p2;

unsigned long bonus_to_give;    //彩金
char genbuf[200];
char string_time[20];           //期數==data_now[0], 修正奇怪的bug

/*讀取lotto__no_now.txt的資料*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
//        return 0;
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
//h++;
//printf ("第 %d 筆,k=%d, userid:%s\n",h,k,lotto_num[0]);

    switch(k){

        case 2:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_6;
                  give_prize(current.userid,bonus_to_give);
                  mail_good_news(lotto_num[0],data_now,lotto_num,"六",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了六獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

                  if(strcmp(lotto_num[10],"電腦選號")==0){
                    sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                  }
                  else{
                    sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  }

//                sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);


                  fputs(buf,lotto_open);
                  count_6++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 3:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_5;
                  give_prize(current.userid,bonus_to_give);
                  mail_good_news(lotto_num[0],data_now,lotto_num,"五",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了五獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
                  if(strcmp(lotto_num[10],"電腦選號")==0){
                     sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  }

                  //sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  fputs(buf,lotto_open);
                  count_5++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 4:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_4;
                  give_prize(current.userid,bonus_to_give);
                  mail_good_news(lotto_num[0],data_now,lotto_num,"四",bonus_to_give,string_time);

                  sprintf(buf,"%s 簽了  [1;32;41m%s %s %s %s %s %s[m  %s元,中了四獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
                  if(strcmp(lotto_num[10],"電腦選號")==0){
                     sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  }

//                sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  fputs(buf,lotto_open);

                  count_4++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 5:
                  if(luck_num_s==0){
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_3;
                          give_prize(current.userid,bonus_to_give);
                          mail_good_news(lotto_num[0],data_now,lotto_num,"三",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了三獎,"
                         ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                   fputs(buf,lotto_open);
                   if(strcmp(lotto_num[10],"電腦選號")==0){
                      sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                   }
                   else{
                      sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                   }

//                 sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                   fputs(buf,lotto_open);
                   count_3++;
                   count_out=count_out+bonus_to_give;
                   break;
                  }
                  else{
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_2;
                          give_prize(current.userid,bonus_to_give);
                          mail_good_news(lotto_num[0],data_now,lotto_num,"二",bonus_to_give,string_time);
                          sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了二獎,"
                                ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                                ,lotto_num[8],lotto_num[9],lotto_num[3]);
                          fputs(buf,lotto_open);
//                        sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                          if(strcmp(lotto_num[10],"電腦選號")==0){
                             sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                          }
                          else{
                             sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                          }

                          fputs(buf,lotto_open);
                          count_2++;
                          count_out=count_out+bonus_to_give;
                          break;

                  }
        case 6:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_1;
                  give_prize(current.userid,bonus_to_give);
                  mail_good_news(lotto_num[0],data_now,lotto_num,"頭",bonus_to_give,string_time);
                  sprintf(buf,"%s 簽了 [1;32;41m%s %s %s %s %s %s[m  %s元,中了頭獎,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
//                sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  if(strcmp(lotto_num[10],"電腦選號")==0){
                     sprintf(buf,"可得彩金 七七金幣 %ld元 --電腦選號\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"可得彩金 七七金幣 %ld元\n",bonus_to_give);
                  }

                  fputs(buf,lotto_open);
                  count_1++;
                  count_out=count_out+bonus_to_give;
                  break;
        default:
//        printf("真可惜~..沒有中獎\n");
          break;

    }

    h++;
    printf("No. %d data  completed..\n",h);
  }

        fclose(this_record);

        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  // 將開完獎的記錄檔remove
        remove(buf);

/* 將開獎號碼記進 lotto_no_history.txt */
        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_HISTORY);
        lotto_open_num=fopen(buf,"a");
        sprintf(buf,"%s;%s;%d;%d;%d;%d;%d;%d;%d;\n",string_time,data_now[1],
                atoi(data_now[2]),atoi(data_now[3]),atoi(data_now[4]),
                atoi(data_now[5]),atoi(data_now[6]),atoi(data_now[7]),
                atoi(data_now[8]));
//      printf("%s\n",buf);
        fputs(buf,lotto_open_num);
        fclose(lotto_open_num);
        fclose(lotto_open);

/* 將統計結果寫入 lotto_stat.txt */
        sprintf(buf,"%slotto/lotto_stat.txt",LOTTO_PATH);
        lotto_stat=fopen(buf,"w");
        sprintf(buf,"第 %s 期 七七樂開獎各項統計\n\n",string_time);
        fputs(buf,lotto_stat);
        sprintf(buf," 頭獎得獎數 : %d注\n",count_1);
        fputs(buf,lotto_stat);
        sprintf(buf," 貳獎得獎數 : %d注\n",count_2);
        fputs(buf,lotto_stat);
        sprintf(buf," 參獎得獎數 : %d注\n",count_3);
        fputs(buf,lotto_stat);
        sprintf(buf," 肆獎得獎數 : %d注\n",count_4);
        fputs(buf,lotto_stat);
        sprintf(buf," 伍獎得獎數 : %d注\n",count_5);
        fputs(buf,lotto_stat);
        sprintf(buf," 陸獎得獎數 : %d注\n\n",count_6);
        fputs(buf,lotto_stat);
        sprintf(buf," 總下注數   : %d注\n\n",count_all);
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
//        if(lotto_open_num=fopen(buf,"r")) {
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




//        }


        fclose(lotto_stat);
}
