
/*
 lotto ¹CÀ¸¶}¼úµ{¦¡

 2002/2 ±H¤¤¼ú³qª¾µ¹user , µ¹¿ú.

 2002/3 §¹¦¨²Î­p³¡¥÷

 2002/7/9 §¹¦¨·s¼Wª÷»È¹ô¿ï¾Ü,Ã±½ä¯¸¶}Ãö

 2002/8/17 ­×¥¿²Î­p¼Æ¦r¹L¤jµLªkshowªº°ÝÃD
	   ¥]µP¤¤¼ú¨S¦³¦b¤¤¼ú¨ç¤Î¤½§G¤¤show¥X
 2002/8/19 ·s¼W¦b¶}§¹¼ú¸¹¦Û°Ê±Nlotto_on_offÅÜ¦¨"on"
 2002/8/20 ·s¼W¦U¤H¸Ó´Á¦U¶µ²Î­p

 written by Worren.bbs@77bbs.com  ³{¥Ò¤j¾Ç¤C¤ë¤C¤é´¸BBS

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
                // ¨ä¥L user ¥¿¦b flushing ucache ==> CSMA/CD
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

                // µ¥ user ¸ê®Æ§ó·s«á¦A³]©w uptime

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
FILE *lotto_rule;       //ªþ¦b¤¤¼ú³qª¾«á­±ªº¤å¥ó
fileheader mymail;
time_t now;
int i;
char buf[200];
char genbuf[200];


                        if((fp = fopen(LOTTO_PATH"lotto/lotto_open_tmp", "w+")) == NULL)
                                return;

//                      now = time(NULL) - 6 * 60;
                        sprintf(genbuf, "§@ªÌ: ¤C¤C»È¦æ¼Ö³z³¡\n");
                        fputs(genbuf, fp);
                        sprintf(genbuf, "¼ÐÃD: ®¥³ß§A¤¤¼ú°Õ~~\n");
                        fputs(genbuf, fp);
                        time(&now);
                        sprintf(genbuf, "®É¶¡: %s\n", ctime(&now));
                        fputs(genbuf, fp);
			if(LOTTO_GOLD_SILVER==0){
                            sprintf(genbuf, "±z¦b [1;33;40m%s %s [mÃ±¤F [1;35;40m%s %s %s %s %s %s[m  , ©ã¤F¤C¤Cª÷¹ô[1;36;40m %s[m¤¸¾ã",
                               lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                               lotto_num[8],lotto_num[9],lotto_num[3]);
			}
			else if(LOTTO_GOLD_SILVER==1){
                            sprintf(genbuf, "±z¦b [1;33;40m%s %s [mÃ±¤F [1;35;40m%s %s %s %s %s %s[m  , ©ã¤F¤C¤C»È¹ô[1;36;40m %s[m¤¸¾ã",
                               lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                               lotto_num[8],lotto_num[9],lotto_num[3]);
			}
                        fputs(genbuf, fp);
                        if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                          fputs(" --¹q¸£¿ï¸¹\n\n",fp);
                        }
                        else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
                          fputs(" --¥]µP¿ï¸¹\n\n",fp);
			}			
                        else fputs("\n\n",fp);
                        sprintf(genbuf, "¥»´Á²Ä [1;37;40m%s[m ´Á©ó [1;36;40m%s[m ¶}¼ú,\n",string_time,data_now[1]);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "¶}¼ú¸¹½X¬° [1;32;41m%s %s %s %s %s %s[m ¯S§O¸¹ [1;33;44m%s[m  , ±z¤¤¤F [1;31;40m%s [m¼ú\n\n",
                           data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],
                           data_now[8],prize);
                        fputs(genbuf, fp);
			if(LOTTO_GOLD_SILVER==0){
                           sprintf(genbuf, "±z¥i¥H¿W±o±mª÷¤C¤Cª÷¹ô [1;36;41m%d[m¤¸¾ã\n\n\n\n",bonus_to_give);
			}
			else if(LOTTO_GOLD_SILVER==1){
                           sprintf(genbuf, "±z¥i¥H¿W±o±mª÷¤C¤C»È¹ô [1;36;41m%d[m¤¸¾ã\n\n\n\n",bonus_to_give);
			}
                        fputs(genbuf,fp);
                        /* ¥[¤Jlotto_rule.txt */
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
                        strcpy(mymail.owner, "[¤C¤C»È¦æ]");
                        sprintf(mymail.title, "®¥³ß±z¤¤¼ú¤F--±z¤¤¤F¤C¤C¼Ö³z[1;31;40m%s[m¼ú",prize);
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

FILE *get_now;          //§Y®É¶}¼ú¸¹½X
FILE *this_record;      //¥»´ÁÃ±½ä°O¿ý
FILE *record_history;   //Ã±½ä°O¿ý¥á¶ihistory¥Ø¿ý¸Ì
FILE *lotto_open_num;   //¦U´Á¶}¼ú°O¿ý
FILE *lotto_open;       //¤¤¼ú¦W³æ
FILE *lotto_stat;       //²Î­pµ²ªG
FILE *lotto_top;
FILE *fp;		//temp

fileheader mymail;

char data_now[10][25];  //Åªget_now(get_lotto_now.txt)ªº¸ê®Æ
char lotto_num[11][30];  //¦U­ÓÃ±½äªº¸¹½X

int luck_num[5];        //¹ï¤¤ªº¸¹½X
int luck_num_s;         //¯S§O¸¹ºX¼Ð, ¨S¹ï¤¤==0, ¹ï¤¤==1

long gamble_money;      //©ãª÷

int i=0;        //¼È¦s¥Îªº¾ã¼Æ0
int j=0;        //¼È¦s¥Îªº¾ã¼Æ1
int k=0;        //............2
unsigned long count_1=0;  //±oÀY¼úÁ`¼Æ
unsigned long count_2=0;  //2¼ú
unsigned long count_3=0;
unsigned long count_4=0;
unsigned long count_5=0;
unsigned long count_6=0;
unsigned long count_all=0;        //¦¹´ÁÁ`¦@Ã±¤F´X¤ä
unsigned long count_in=0;                //¦¬¤J
unsigned long count_out=0;       //¤ä¥X
float precent_lotto=0;  //±o¼ú²v

int lotto_no_statistics[80][2]; //²Î­p¦U¸¹½X¥X²{¦¸¼Æ
int lotto_no_s[80][2];          //²Î­p¯S§O¸¹
int statistics[80];                     //¼È¦s°}¦C
int s_stat[80];                         //¯S§O¸¹ªº¼È¦s°}¦C

int h=0;

char buf[200];
char buf2[200];
char buf3[200];
char data_read[100];
char *p;
char *p2;
char lotto_define[8][2]; //²Î­p ,1,2,3,4,5,6¼ú, 1=±H,0=¤£±H
char lotto_stat_echo[9][2][10]; //²Î­p¦U¶µ­Ó¤H¼Æ¾Ú¥Î!
char luck_user[50];

unsigned long bonus_to_give;    //±mª÷
char genbuf[200];
char string_time[20];           //´Á¼Æ==data_now[0], ­×¥¿©_©Çªºbug

savelog("¶}©l¶]lotto_open");

/* ²M°£©Ò¦³¼È¦sÀÉ */
sprintf(buf,"/bin/rm -f %slotto/tmp/*",LOTTO_PATH);
system(buf);

/*Åª¨úlotto__no_now.txtªº¸ê®Æ*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
        printf("Åª¤£¨ìÀÉ");
        exit(0);
   }
   fgets(data_read,79,get_now);
   fclose(get_now);
   if(strlen(data_read)<35) {
        printf("Åª¨S¦³¸ê®Æ");
        exit(0);
   }
   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
   }
   strcpy(string_time,data_now[0]);

//Åª¨údata_now§¹²¦

  /* ­n¶}©l¶}¼ú¤F, §âlotto_on_off.txt¤º®e§ï¦¨"off" */
	sprintf(buf,"/bin/echo off > %s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
        system(buf);



  /* ¶}±ÒÃ±½ä¾ú¥v¸ê®Æ */
  sprintf(buf,"%slotto/history/lotto_record_%s.txt",LOTTO_PATH,data_now[0]);
  record_history=fopen(buf,"a+");

/* ¶}©lÅª¨úÃ±½ä¸ê®Æ¨Ã®Ö¹ï¤¤¼ú¥Bµoµ¹¼úª÷ */
  sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
//  if(!(this_record=fopen(buf,"r")))  exit(0);
  if(!(this_record=fopen(buf,"r")))  this_record=fopen(buf,"a+");
//--->¥ý¼g¨ì³o¸Ì,²Ö¤F..ºÎ§a!91/02/21

  if(!(fgets(buf,180,this_record)))  ; //§â²Ä¤@¦æªºxx´Á¥ýÅª°_¨Ó!
  fputs(buf,record_history);

/* ¶}±Ò¤¤¼ú¦W³æ */
  sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_OPEN);
  lotto_open=fopen(buf,"w");            //¶}±Ò¤¤¼ú¦W³æÀÉ
  sprintf(buf,"²Ä %s ´Á ¤C¤C¼Ö³z¤¤¼ú¦W³æ\n",string_time);
  fputs(buf,lotto_open);
  sprintf(buf,"¥»´Á¶}¼ú¸¹½X : [1;32;41m %s %s %s %s %s %s [m ¯S§O¸¹ : [1;33;44m%s[m\n\n"
        ,data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],data_now[8]);
  fputs(buf,lotto_open);

  while(fgets(buf,180,this_record)){

    count_all++;
    strcpy(lotto_num[10],"");
    fputs(buf,record_history);
    sprintf(lotto_num[0],"%s",strtok(buf,";"));
    i=0;
        strcpy(string_time,data_now[0]);        //­×¥¿bug...«Ü©_©Ç, ¹J¤W¯S©wªº¸ê®Æ, data_now[0]´N¬O·|ÅÜ¦¨ªÅ¥Õª¬
    while(p2=strtok(NULL,";")){
      i++;
      sprintf(lotto_num[i],"%s",p2);
    }

    count_in=count_in+atoi(lotto_num[3]);


    k=0;                //°O¿ý¹ï¤¤¸¹½X¯Å¼Æ, ¤@¯ë¸¹ +1, ¯S§O¸¹ «hlotto_num_s=1
    luck_num_s=0;       //¯S§O¸¹ºX¼Ð=0(¨S¹ï¤¤)


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
    /* ¥[Á`userªº¤Uª`Á`¦X,¤Uª`¼Æ¤Î§âuser¥[¤J¦¹´Áªºplayer list 2002/8/19*/    
    //Åª¨ú¥Ø«euser¤Uª`Á`¦Xi

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
    
    //¤Uª`Á`ª÷ÃB,¤Uª`¼Æ& list µ{¦¡end

    /* ¨ú±ouser¦U³qª¾«H²[ªº³]©w­È */
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
    //³]©w­ÈÅª¨ú§¹²¦


    switch(k){

        case 2:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_6;
                  give_prize(current.userid,bonus_to_give);                  
                  if(strcmp(lotto_define[6],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"¤»",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤»¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

		  if(LOTTO_GOLD_SILVER==0){		
                      if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                        sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                      }
                      else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
                        sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
                      }
                      else{
                        sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                      }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
                      if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                        sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                      }
		      else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
			sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		      }
                      else{
                        sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
                  if(strcmp(lotto_define[5],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"¤­",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤­¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
                    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
		       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
		       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
                  if(strcmp(lotto_define[4],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"¥|",bonus_to_give,string_time);

                  sprintf(buf,"%s Ã±¤F  [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¥|¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

 		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
 		       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }

                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
 		       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
                          if(strcmp(lotto_define[3],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"¤T",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤T¼ú,"
                         ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                   fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
                    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
		       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
 		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
                          if(strcmp(lotto_define[2],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"¤G",bonus_to_give,string_time);
                          sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤G¼ú,"
                                ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                                ,lotto_num[8],lotto_num[9],lotto_num[3]);
                          fputs(buf,lotto_open);
 		  	if(LOTTO_GOLD_SILVER==0){
			    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
        	               sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                	    }
			    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
			       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
			    }
	                    else{
        	               sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                	    }
			  }
			  else if(LOTTO_GOLD_SILVER==1){
			    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
	                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
        	            }
			    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
 		               sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
			    }
                	    else{
	                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
                  if(strcmp(lotto_define[1],"1")==0)  mail_good_news(lotto_num[0],data_now,lotto_num,"ÀY",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤FÀY¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
		  if(LOTTO_GOLD_SILVER==0){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
		       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤Cª÷¹ô %ld¤¸\n",bonus_to_give);
                    }
		  }
		  else if(LOTTO_GOLD_SILVER==1){
		    if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                    }
		    else if(strcmp(lotto_num[10],"¥]µP¿ï¸¹")==0){
 		       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸ --¥]µP¿ï¸¹\n",bonus_to_give);
		    }
                    else{
                       sprintf(buf,"¥i±o±mª÷ ¤C¤C»È¹ô %ld¤¸\n",bonus_to_give);
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
//¨ú®ø¦¹¦æ°T®§,¥H§K©ìºC¶}¼úªº³t«×!­Y±z·QÀH®ÉºÊ¬Ý¶}¼ú¨ì²Ä´Xµ§,¥i¥H§â³o¦æ¶}±Ò!
    printf("No. %d data  completed..\n",h);
  }
        fclose(this_record);

  sprintf(buf,"¦@¦³%ldµ§¸ê®Æ,³B²z§¹²¦,¶}©l²Î­p§@·~...",count_all);
  savelog(buf);

        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  // ±N¶}§¹¼úªº°O¿ýÀÉremove
        remove(buf);

        /* ¤w¸g¶}§¹¼ú¤F, §âlotto_on_off.txt¤º®e§ï¦¨"on" */
	sprintf(buf,"/bin/echo on > %s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
        system(buf);


        /* ±N¶}¼ú¸¹½X°O¶i lotto_no_history.txt */
        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_HISTORY);
        lotto_open_num=fopen(buf,"a");
        sprintf(buf,"%s;%s;%d;%d;%d;%d;%d;%d;%d;\n",string_time,data_now[1],
                atoi(data_now[2]),atoi(data_now[3]),atoi(data_now[4]),
                atoi(data_now[5]),atoi(data_now[6]),atoi(data_now[7]),
                atoi(data_now[8]));

        fputs(buf,lotto_open_num);
        fclose(lotto_open_num);
        fclose(lotto_open);

        /* ±N²Î­pµ²ªG¼g¤J lotto_stat.txt */
        sprintf(buf,"%slotto/lotto_stat.txt",LOTTO_PATH);
        lotto_stat=fopen(buf,"w");
        sprintf(buf,"²Ä %s ´Á ¤C¤C¼Ö³z¶}¼ú¦U¶µ²Î­p\n\n",string_time);
        fputs(buf,lotto_stat);
        sprintf(buf," ÀY¼ú±o¼ú¼Æ : %ldª`\n",count_1);
        fputs(buf,lotto_stat);
        sprintf(buf," ¶L¼ú±o¼ú¼Æ : %ldª`\n",count_2);
        fputs(buf,lotto_stat);
        sprintf(buf," °Ñ¼ú±o¼ú¼Æ : %ldª`\n",count_3);
        fputs(buf,lotto_stat);
        sprintf(buf," ¸v¼ú±o¼ú¼Æ : %ldª`\n",count_4);
        fputs(buf,lotto_stat);
        sprintf(buf," ¥î¼ú±o¼ú¼Æ : %ldª`\n",count_5);
        fputs(buf,lotto_stat);
        sprintf(buf," ³°¼ú±o¼ú¼Æ : %ldª`\n\n",count_6);
        fputs(buf,lotto_stat);
        sprintf(buf," Á`¤Uª`¼Æ   : %ldª`\n\n",count_all);
        fputs(buf,lotto_stat);
        sprintf(buf," ½äª÷Á`¦¬¤J : %ld¤¸\n",count_in);
        fputs(buf,lotto_stat);
        sprintf(buf," ±mª÷Á`¤ä¥X : %ld¤¸\n",count_out);
        fputs(buf,lotto_stat);
        precent_lotto=((float)(count_1 + count_2 + count_3 + count_4 + count_5 + count_6) / (float)count_all) * 100;
        sprintf(buf," ¥»´Á¤¤¼ú²v : %f¢H\n\n",precent_lotto);
        fputs(buf,lotto_stat);

        /* ²Î­p¦U¸¹½X¥X²{¦¸¼Æ¤Î±Æ¦W */
        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_HISTORY);
        lotto_open_num=fopen(buf,"r");
           /*ªì©l¤Æ°}¦C*/
           for(i=0;i<(LOTTO_NUM_MAX+1);i++){
                statistics[i]=0;
                s_stat[i]=0;
           }


           sprintf(buf,"-=-=-=-=-¦U¸¹½X¥X²{¦¸¼Æ²Î­p-=-=-=-=-=-\n\n");
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

           /* ¼g¤J¦U¸¹½X¥X²{¦¸¼Æ */
           i=0;
           j=0;
           while(i<LOTTO_NUM_MAX){
             strcpy(buf,"");
             j=0;
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d¦¸",buf,i,statistics[i]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }
           fputs("\n",lotto_stat);

           /*±Æ§Ç¥X²{¦¸¼Æ*/
           i=0;
           while(i<LOTTO_NUM_MAX){
             i++;
             lotto_no_statistics[i][1]=statistics[i];
             lotto_no_statistics[i][0]=i;
           }
           //¶}©l±Æ, ®ðªw

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

           fputs("-=-=-=-=-¶}¼ú¸¹½X¥X²{¦¸¼Æ±Æ¦C-=-=-=-=-=-=\n\n",lotto_stat);
           strcpy(buf,"");
           sprintf(buf,"[1;33;40m1st,%d:%d¦¸    2ed,%d:%d¦¸    3rd,%d:%d¦¸[m\n",
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
                sprintf(buf,"%s%d:%d¦¸",buf,lotto_no_statistics[i][0],lotto_no_statistics[i][1]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }

           fputs("\n",lotto_stat);

           sprintf(buf,"-=-=-=-=-¯S§O¸¹¥X²{¦¸¼Æ²Î­p-=-=-=-=-=-\n\n");
           fputs(buf,lotto_stat);

           /* ¼g¤J¦U¸¹½X¥X²{¦¸¼Æ */
           i=0;
           j=0;
           while(i<LOTTO_NUM_MAX){
             strcpy(buf,"");
             j=0;
             while(j<5 & i<LOTTO_NUM_MAX){
                i++;
                j++;
                sprintf(buf,"%s%d:%d¦¸",buf,i,s_stat[i]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }
           fputs("\n",lotto_stat);

           /*±Æ§Ç¥X²{¦¸¼Æ*/
           i=0;
           while(i<LOTTO_NUM_MAX){
             i++;
             lotto_no_statistics[i][1]=s_stat[i];
             lotto_no_statistics[i][0]=i;
           }
           //¶}©l±Æ, ®ðªw

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

           fputs("-=-=-=-=-¯S§O¸¹¥X²{¦¸¼Æ±Æ¦C-=-=-=-=-=-=\n\n",lotto_stat);
           strcpy(buf,"");
           sprintf(buf,"[1;33;40m1st,%d:%d¦¸    2ed,%d:%d¦¸    3rd,%d:%d¦¸[m\n",
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
                sprintf(buf,"%s%d:%d¦¸",buf,lotto_no_statistics[i][0],lotto_no_statistics[i][1]);
                while(strlen(buf)<12*j){
                   sprintf(buf,"%s ",buf);
                }
             }
             sprintf(buf,"%s\n",buf);
             fputs(buf,lotto_stat);
           }

        fclose(lotto_stat);

        /*¶K¤W²Î­pµ²ªG*/
        sprintf(buf,"%sbin/post_open",LOTTO_PATH);
        system(buf);

	savelog("§¹¦¨Á`²Î­p,¶}©l­Ó¤H²Î­p...");

        /* ¶}©l±H²Î­p³qª¾²[ */        
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
            count_out=atol(buf2);      //Á`¤ä¥X
            fclose(get_now);
            sprintf(buf2,"%slotto/tmp/statistics_%s_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
              fgets(buf2,180,get_now);
              count_in=atol(buf2);       //Á`¦¬¤J
              fclose(get_now);
            }
            else count_in=0;
            sprintf(buf2,"%slotto/tmp/statistics_%s_amount",LOTTO_PATH,luck_user);
            get_now=fopen(buf2,"r");
            fgets(buf2,180,get_now);
            count_all=atol(buf2);      //Á`¤Uª`¼Æ
            fclose(get_now);
            //ÀY¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_1_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[1][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[1][0],"0");
            //¤G¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_2_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[2][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[2][0],"0");
            //3¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_3_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[3][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[3][0],"0");
            //4¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_4_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[4][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[4][0],"0");
            //5¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_5_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[5][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[5][0],"0");
            //6¼ú¼Æ
            sprintf(buf2,"%slotto/tmp/statistics_%s_6_amount",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[6][0],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[6][0],"0");
            //ÀY¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_1_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[1][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[1][1],"0");
            //2¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_2_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[2][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[2][1],"0");
            //3¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_3_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[3][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[3][1],"0");
            //4¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_4_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[4][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[4][1],"0");
            //5¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_5_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[5][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[5][1],"0");
            //6¼úª÷
            sprintf(buf2,"%slotto/tmp/statistics_%s_6_bonus",LOTTO_PATH,luck_user);
            if(get_now=fopen(buf2,"r")){
            	fgets(lotto_stat_echo[6][1],180,get_now);
            	fclose(get_now);
            }
            else strcpy(lotto_stat_echo[6][1],"0");
            precent_lotto=(((float)(atol(lotto_stat_echo[1][0])+atol(lotto_stat_echo[2][0])+
                           atol(lotto_stat_echo[3][0])+atol(lotto_stat_echo[4][0])+
                           atol(lotto_stat_echo[5][0])+atol(lotto_stat_echo[6][0])))/((float)count_all))*100;
            sprintf(buf2,"%slotto/tmp/stat_mail_%s",LOTTO_PATH,luck_user);	//¦¹³Bªºbuf¬Ousername
            get_now=fopen(buf2,"w");
            sprintf(buf2,"¿Ë·Rªº %s :\n\n",luck_user);
            fputs(buf2,get_now);
            sprintf(buf2,"      ±z¦b¥»´Á,²Ä%s´Á,ªº¤Uª`¤Î¤¤¼ú²Î­p¦p¤U...\n\n",string_time);
            fputs(buf2,get_now);
            sprintf(buf2,"      Á`¤Uª`¼Æ   : %ld\t Á`¤Uª`ª÷ÃB  : %ld\n",count_all,count_out);
            fputs(buf2,get_now);
            sprintf(buf2,"      ÀY¼ú¤¤¼ú¼Æ : %s\t\t ÀY¼ú±mª÷    : %s\n",lotto_stat_echo[1][0],lotto_stat_echo[1][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¤G¼ú¤¤¼ú¼Æ : %s\t\t ¤G¼ú±mª÷    : %s\n",lotto_stat_echo[2][0],lotto_stat_echo[2][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¤T¼ú¤¤¼ú¼Æ : %s\t\t ¤T¼ú±mª÷    : %s\n",lotto_stat_echo[3][0],lotto_stat_echo[3][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¥|¼ú¤¤¼ú¼Æ : %s\t\t ¥|¼ú±mª÷    : %s\n",lotto_stat_echo[4][0],lotto_stat_echo[4][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¤­¼ú¤¤¼ú¼Æ : %s\t\t ¤­¼ú±mª÷    : %s\n",lotto_stat_echo[5][0],lotto_stat_echo[5][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¤»¼ú¤¤¼ú¼Æ : %s\t\t ¤»¼ú±mª÷    : %s\n\n",lotto_stat_echo[6][0],lotto_stat_echo[6][1]);
            fputs(buf2,get_now);
            sprintf(buf2,"      ¥»´Á¤¤¼ú²v : %f¢H\t\t Á`±mª÷     : %ld\n",precent_lotto,count_in);
            fputs(buf2,get_now);
            if (count_out > count_in) sprintf(buf2,"      ¥»´Á±z½ß¤F : %ld¤¸\n",(count_out-count_in));
            else sprintf(buf2,"      ¥»´Á±zÁÈ¤F : %ld¤¸\n",(count_in-count_out));
            fputs(buf2,get_now);
            fclose(get_now);
	    //¸ê®Æ±H¥X
            sethomepath (buf2, luck_user);
            stampfile(buf2, &mymail);
            sprintf(buf3,"%slotto/tmp/stat_mail_%s",LOTTO_PATH,luck_user);	//¦¹³Bªºbuf¬Ousername
            rename(buf3, buf2);
            strcpy(mymail.owner, "[¤C¤C»È¦æ]");
            sprintf(mymail.title, "¤C¤C¼Ö³z²Ä%s´Á­Ó¤H¦U¶µ¼Æ¾Ú²Î­p",string_time);
            mymail.savemode = 0;
            sethomedir(buf2, luck_user);
            append_record(buf2, &mymail, sizeof(mymail));
          
        }
	fclose(fp);
        savelog("­Ó¤H²Î­p§¹¦¨,lotto_open°õ¦æ§¹²¦!!");

}

//logÀÉ°Æµ{¦¡
savelog(char what_log[200]){

FILE *updatelogtime ;
char nowt[200];
char *wday[]={"¬P´Á¤é","¬P´Á¤@","¬P´Á¤G","¬P´Á¤T","¬P´Á¥|","¬P´Á¤­","¬P´Á¤»"};

time_t timep;
struct tm *p;
time(&timep);

updatelogtime= fopen(LOTTO_PATH"lotto/lotto_open.log","a+");

p = localtime(&timep);

sprintf(nowt,"©ó%d/%d/%d %s %d:%d:%d %s \n",(1900+p->tm_year),
        (1+p->tm_mon),p->tm_mday,wday[p->tm_wday],p->tm_hour,p->tm_min,
        p->tm_sec,what_log);

fputs(nowt,updatelogtime);
fclose(updatelogtime);

}

