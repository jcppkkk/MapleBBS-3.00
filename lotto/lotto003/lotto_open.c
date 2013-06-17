
/*
 lotto ¹CÀ¸¶}¼úµ{¦¡

 2002/2 ±H¤¤¼ú³qª¾µ¹user , µ¹¿ú.

 2002/3 §¹¦¨²Î­p³¡¥÷

 written by Worren.bbs@77bbs.com  ³{¥Ò¤j¾Ç¤C¤ë¤C¤é´¸BBS

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
                xuser.goldmoney += money;
                substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
                return xuser.goldmoney;
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
                        sprintf(genbuf, "§@ªÌ: µL¼Æ»È¦æ¼Ö³z³¡\n");
                        fputs(genbuf, fp);
                        sprintf(genbuf, "¼ÐÃD: ®¥³ß§A¤¤¼ú°Õ~~\n");
                        fputs(genbuf, fp);
                        time(&now);
                        sprintf(genbuf, "®É¶¡: %s\n", ctime(&now));
                        fputs(genbuf, fp);
                        sprintf(genbuf, "±z¦b [1;33;40m%s %s [mÃ±¤F [1;35;40m%s %s %s %s %s %s[m  , ©ã¤F ª÷¹ô[1;36;40m %s[m¤¸¾ã",
                           lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7],
                           lotto_num[8],lotto_num[9],lotto_num[3]);
                        fputs(genbuf, fp);
                        if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                          fputs(" --¹q¸£¿ï¸¹\n\n",fp);
                        }
                        else fputs("\n\n",fp);
                        sprintf(genbuf, "¥»´Á²Ä [1;37;40m%s[m ´Á©ó [1;36;40m%s[m ¶}¼ú,\n",string_time,data_now[1]);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "¶}¼ú¸¹½X¬° [1;32;41m%s %s %s %s %s %s[m ¯S§O¸¹ [1;33;44m%s[m  , ±z¤¤¤F [1;31;40m%s [m¼ú\n\n",
                           data_now[2],data_now[3],data_now[4],data_now[5],data_now[6],data_now[7],
                           data_now[8],prize);
                        fputs(genbuf, fp);
                        sprintf(genbuf, "±z¥i¥H¿W±o±mª÷ ª÷¹ô [1;36;41m%d[m¤¸¾ã\n\n\n\n",bonus_to_give);
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
                        strcpy(mymail.owner, "[µL¼Æ»È¦æ]");
                        sprintf(mymail.title, "®¥³ß±z¤¤¼ú¤F--±z¤¤¤FµL¼Æ¼Ö³z[1;31;40m%s[m¼ú",prize);
                        mymail.savemode = 0;
                        sethomedir(genbuf, luck_user);
                        append_record(genbuf, &mymail, sizeof(mymail));
}


main (){

FILE *get_now;          //§Y®É¶}¼ú¸¹½X
FILE *this_record;      //¥»´ÁÃ±½ä°O¿ý
FILE *record_history;   //Ã±½ä°O¿ý¥á¶ihistory¥Ø¿ý¸Ì
FILE *lotto_open_num;   //¦U´Á¶}¼ú°O¿ý
FILE *lotto_open;       //¤¤¼ú¦W³æ
FILE *lotto_stat;       //²Î­pµ²ªG
FILE *lotto_top;

char data_now[10][25];  //Åªget_now(get_lotto_now.txt)ªº¸ê®Æ
char lotto_num[11][30];  //¦U­ÓÃ±½äªº¸¹½X

int luck_num[5];        //¹ï¤¤ªº¸¹½X
int luck_num_s;         //¯S§O¸¹ºX¼Ð, ¨S¹ï¤¤==0, ¹ï¤¤==1

long gamble_money;      //©ãª÷

int i=0;        //¼È¦s¥Îªº¾ã¼Æ0
int j=0;        //¼È¦s¥Îªº¾ã¼Æ1
int k=0;        //............2
int count_1=0;  //±oÀY¼úÁ`¼Æ
int count_2=0;  //2¼ú
int count_3=0;
int count_4=0;
int count_5=0;
int count_6=0;
int count_all=0;        //¦¹´ÁÁ`¦@Ã±¤F´X¤ä
long count_in=0;                //¦¬¤J
long count_out=0;       //¤ä¥X
float precent_lotto=0;  //±o¼ú²v

int lotto_no_statistics[80][2]; //²Î­p¦U¸¹½X¥X²{¦¸¼Æ
int lotto_no_s[80][2];          //²Î­p¯S§O¸¹
int statistics[80];                     //¼È¦s°}¦C
int s_stat[80];                         //¯S§O¸¹ªº¼È¦s°}¦C

int h=0;

char buf[200];
char data_read[100];
char *p;
char *p2;

unsigned long bonus_to_give;    //±mª÷
char genbuf[200];
char string_time[20];           //´Á¼Æ==data_now[0], ­×¥¿©_©Çªºbug

/*Åª¨úlotto__no_now.txtªº¸ê®Æ*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
//        return 0;
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
  sprintf(buf,"²Ä %s ´Á µL¼Æ¼Ö³z¤¤¼ú¦W³æ\n",string_time);
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
//h++;
//printf ("²Ä %d µ§,k=%d, userid:%s\n",h,k,lotto_num[0]);

    switch(k){

        case 2:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_6;
                  give_prize(current.userid,bonus_to_give);
//                  mail_good_news(lotto_num[0],data_now,lotto_num,"¤»",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤»¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);

                  if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                    sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                  }
                  else{
                    sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  }

//                sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);


                  fputs(buf,lotto_open);
                  count_6++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 3:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_5;
                  give_prize(current.userid,bonus_to_give);
//                  mail_good_news(lotto_num[0],data_now,lotto_num,"¤­",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤­¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
                  if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  }

                  //sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  fputs(buf,lotto_open);
                  count_5++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 4:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_4;
                  give_prize(current.userid,bonus_to_give);
//                  mail_good_news(lotto_num[0],data_now,lotto_num,"¥|",bonus_to_give,string_time);

                  sprintf(buf,"%s Ã±¤F  [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¥|¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
                  if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  }

//                sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  fputs(buf,lotto_open);

                  count_4++;
                  count_out=count_out+bonus_to_give;
                  break;
        case 5:
                  if(luck_num_s==0){
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_3;
                          give_prize(current.userid,bonus_to_give);
//                          mail_good_news(lotto_num[0],data_now,lotto_num,"¤T",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤T¼ú,"
                         ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                   fputs(buf,lotto_open);
                   if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                      sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                   }
                   else{
                      sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                   }

//                 sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                   fputs(buf,lotto_open);
                   count_3++;
                   count_out=count_out+bonus_to_give;
                   break;
                  }
                  else{
                          bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_2;
                          give_prize(current.userid,bonus_to_give);
//                          mail_good_news(lotto_num[0],data_now,lotto_num,"¤G",bonus_to_give,string_time);
                          sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤F¤G¼ú,"
                                ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                                ,lotto_num[8],lotto_num[9],lotto_num[3]);
                          fputs(buf,lotto_open);
//                        sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                          if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                             sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                          }
                          else{
                             sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                          }

                          fputs(buf,lotto_open);
                          count_2++;
                          count_out=count_out+bonus_to_give;
                          break;

                  }
        case 6:
                  bonus_to_give = atoi(lotto_num[3]) * LOTTO_RATE_1;
                  give_prize(current.userid,bonus_to_give);
//                  mail_good_news(lotto_num[0],data_now,lotto_num,"ÀY",bonus_to_give,string_time);
                  sprintf(buf,"%s Ã±¤F [1;32;41m%s %s %s %s %s %s[m  %s¤¸,¤¤¤FÀY¼ú,"
                        ,lotto_num[0],lotto_num[4],lotto_num[5],lotto_num[6],lotto_num[7]
                        ,lotto_num[8],lotto_num[9],lotto_num[3]);
                  fputs(buf,lotto_open);
//                sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸ --¹q¸£¿ï¸¹\n",bonus_to_give);
                  }
                  else{
                     sprintf(buf,"¥i±o±mª÷ ª÷¹ô %ld¤¸\n",bonus_to_give);
                  }

                  fputs(buf,lotto_open);
                  count_1++;
                  count_out=count_out+bonus_to_give;
                  break;
        default:
//        printf("¯u¥i±¤~..¨S¦³¤¤¼ú\n");
          break;

    }

    h++;
    printf("No. %d data  completed..\n",h);
  }

        fclose(this_record);

        sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  // ±N¶}§¹¼úªº°O¿ýÀÉremove
        remove(buf);

/* ±N¶}¼ú¸¹½X°O¶i lotto_no_history.txt */
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

/* ±N²Î­pµ²ªG¼g¤J lotto_stat.txt */
        sprintf(buf,"%slotto/lotto_stat.txt",LOTTO_PATH);
        lotto_stat=fopen(buf,"w");
        sprintf(buf,"²Ä %s ´Á µL¼Æ¼Ö¶}¼ú¦U¶µ²Î­p\n\n",string_time);
        fputs(buf,lotto_stat);
        sprintf(buf," ÀY¼ú±o¼ú¼Æ : %dª`\n",count_1);
        fputs(buf,lotto_stat);
        sprintf(buf," ¶L¼ú±o¼ú¼Æ : %dª`\n",count_2);
        fputs(buf,lotto_stat);
        sprintf(buf," °Ñ¼ú±o¼ú¼Æ : %dª`\n",count_3);
        fputs(buf,lotto_stat);
        sprintf(buf," ¸v¼ú±o¼ú¼Æ : %dª`\n",count_4);
        fputs(buf,lotto_stat);
        sprintf(buf," ¥î¼ú±o¼ú¼Æ : %dª`\n",count_5);
        fputs(buf,lotto_stat);
        sprintf(buf," ³°¼ú±o¼ú¼Æ : %dª`\n\n",count_6);
        fputs(buf,lotto_stat);
        sprintf(buf," Á`¤Uª`¼Æ   : %dª`\n\n",count_all);
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
//        if(lotto_open_num=fopen(buf,"r")) {
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




//        }


        fclose(lotto_stat);
}
