/*-------------------------------------------------------*/
/* util/bbsmail.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : �� Internet �H�H�� BBS �����ϥΪ�            */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "cache.c"
#include "record.c"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <sysexits.h>


#define SPAM_

#ifdef SPAM_
  
/*
rexchen �s�i�H���v�{��
*/

#define SPAM_KEY     1225
#define RULE  3

struct SPAM
{
        int spam_a;
        int spam_b;
        int spam_c;
        int spam_times;
};

struct SPAM_MAIL
{
        struct SPAM mail_flag[512];
        time_t update_time;
};

struct  SPAM_MAIL     *spam;

#endif

#define LOG_FILE       "log/bbsmail.log"
#define STRCPY(dst, src) sprintf(dst, "%.*s", sizeof(dst)  - 1, src)

char userid[IDLEN + 1];

int
belong_spam(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp))
    {
      if(buf[0] == '#') continue;
      if ((ptr = strtok(buf, " \t\n\r")) && strstr(key, ptr))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
  }
  return rc;
}


#ifdef SPAM_
int
seek_mailflag(spam_a,spam_b,spam_c)
int spam_a;
int spam_b;
int spam_c;
{
  int i=0,j=0;
  if ( spam_a == 0 && spam_b == 0 && spam_c == 0 )
     return 0 ;
  for ( i = 0 ; i < 512 ; i++ )
  {
    if ( spam->mail_flag[i].spam_a == spam_a && spam->mail_flag[i].spam_b
 == spam_b && spam->mail_flag[i].spam_c == spam_c ) {
        if ( spam->mail_flag[i].spam_times < 3 ) {
           spam->mail_flag[i].spam_times++;
           return 0;
         } else {
           spam->mail_flag[i].spam_times++;
           return 1;
        }

    }
  }
  for ( i = 0 ; i < 512 ; i++ ) {
      if ( spam->mail_flag[i].spam_times == 0 ) {
         spam->mail_flag[i].spam_a = spam_a;
         spam->mail_flag[i].spam_b = spam_b;
         spam->mail_flag[i].spam_c = spam_c;
         spam->mail_flag[i].spam_times = 1 ;
         j = 1;
         break;
      }
  }
  if ( j != 1 ) {
    for ( i = 0 ; i < 512 ; i++ ) {
        if ( spam->mail_flag[i].spam_times <= 3 ) {
            spam->mail_flag[i].spam_times = 0 ;
            spam->mail_flag[i].spam_a = 0 ;
            spam->mail_flag[i].spam_b = 0 ;
            spam->mail_flag[i].spam_c = 0 ;
        }
    }
  }
  return 0;
}
#endif


mailog(char* mode, char* msg)
{
   FILE *fp;

   if (fp = fopen(LOG_FILE, "a")) {
      time_t now;
      struct tm *p;

      time(&now);
      p = localtime(&now);
      fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d <%s> %s\n",
         p->tm_year % 100, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min,
         p->tm_sec, mode, msg);
      fclose(fp);
   }
}

dashd(char *fname)
{
   struct stat st;

   return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}


junk(char* reason)
{
   FILE* fp;
   int i;
   char msgbuf[256];

   sprintf(msgbuf, "<%s> %.100s", userid, reason);
   mailog("bbsmail", msgbuf);
   if (fp = popen("bin/bbspost junk", "w")) {
      while (fgets(msgbuf, sizeof(msgbuf), stdin))
        fputs(msgbuf, fp);
      pclose(fp);
   }
//   f_rm(stdin);
   return 0;
}


mail2bbs()
{
   fileheader mymail;
   char genbuf[2000], title[256], sender[256], *ip, *ptr;
//   char fname[200], fnamepgp[200], pgppath[200];
   char fname[200];
   char firstline[100];
   struct stat st;
   time_t tmp_time;
//   FILE *fout, *fpgp;
   FILE *fout;
//   int pgp = 0;

#ifdef SPAM_
/*
rexchen �s�i�H���v�{��
*/
   int spam_a=0,spam_b=0,spam_c=0;
#endif
   /* check if the userid is in our bbs now */
   if (!searchuser(userid))
      return junk("not exist");

   sprintf(genbuf, "home/%s", userid);

//   if (stat(genbuf, &st) == -1) {
   if (!dashd(genbuf)) {
      if (mkdir(genbuf, 0755) == -1)
         return junk("no mail box");
      else
        return junk("mail box error");
   }
//   else if (!(st.st_mode & S_IFDIR))
//      return junk("mail box error");

   /* allocate a file for the new mail */

   stampfile(genbuf, &mymail);
   strcpy(fname, genbuf);

   /* copy the stdin to the specified file */

   if ((fout = fopen(genbuf, "w")) == NULL)
      return junk("can't open stamp file");
//   sprintf(fnamepgp, "%s.pgp", strcpy(fname, genbuf));
//   sprintf(pgppath, "home/%s/pgp", userid);

   /* parse header */

   title[0] = sender[0] = 0;

   if (ip = getenv("SENDER"))
      STRCPY(sender, ip);
   while (fgets(genbuf, sizeof(genbuf), stdin)) 
   {
      if (!*sender && !strncasecmp(genbuf, "From", 4)) 
      {
         if ((ip = strchr(genbuf, '<')) && (ptr = strrchr(ip, '>'))) 
         {
/* shakalaca.000721: �|�X���D���a��
   ���ǩǩǪ� header �O
              <foo@foo.bar>
   �@��Ө� : "Nick" <foo@foo.bar>
              foo@foo.bar
                                                                                
            if (ip > genbuf && ip[-1] == ' ')
               ip[-1] = 0;
*/
            *ip = '\0';
                                                                                
            if (strchr(++ip, '@'))
              *ptr = '\0';
            else                                    /* �� local host �H�H */
              strcpy(ptr, "@" MYHOSTNAME);

            ptr = (char *) strchr(genbuf, ' ');
            while (*++ptr == ' ');
            sprintf(sender, "%s (%s)", ip, ptr);
         }
         else {
            strtok(genbuf, " \t\n\r");
            if (ip = strtok(0, " \t\n\r"))
               STRCPY(sender, ip);
            else
               STRCPY(sender, genbuf);
         }
         continue;
      }
      if (!memcmp(genbuf, "Subject: ", 9)) {
         strip_ansi(title, genbuf + 9,STRIP_ALL);
         str_decode(title);
         continue;
      }
      if (genbuf[0] == '\n')
         break;
   }

   time(&tmp_time);
   if (fgets(genbuf, sizeof(genbuf), stdin))
      STRCPY(firstline, genbuf);
   if (!title[0])
     sprintf(title, "�Ӧ� %.64s", sender);

   fprintf(fout, "�@��: %s\n���D: %s\n�ɶ�: %s\n",
      sender, title, ctime(&tmp_time));


   fputs(genbuf, fout);
   while (fgets(genbuf, sizeof(genbuf), stdin) != NULL)
   { 
     str_decode(genbuf);
     strcat(genbuf,"\n");
     fputs(genbuf, fout);
#ifdef SPAM_
// rexchen �s�i�H���v�{��
     if(spam_b < 250);
     {
       spam_b++;      
       spam_a = spam_a + genbuf[strlen(genbuf)/(spam_b%RULE + 1)];
       /* �쥻�S�[ 1�A�|��bugs */
       spam_c = spam_c + genbuf[strlen(genbuf)/RULE];
     }
#endif
   }
   fclose(fout);

#ifdef SPAM_
   if (seek_mailflag(spam_a,spam_b,spam_c) == 1) {
      unlink(fname);
      sprintf(genbuf,"SPAM: %x %x %x => %s => %s",
              spam_a,spam_b,spam_c,sender,userid);
      mailog("bbsmail", genbuf);
      return 0;
   }
#endif

/*==== wildcat : anti-spam from etc/spam-list & user define spam=====*/
   sprintf(genbuf, BBSHOME"/home/%s/spam-list",userid);
   if(belong_spam(BBSHOME"/etc/spam-list",sender) 
     || belong_spam(genbuf, sender))
   {
     unlink(fname);
     sprintf(genbuf,"SPAM: %s => %s",sender,userid);
     mailog("bbsmail", genbuf);
     return 0;
   }
       
   sprintf(genbuf, "%s => %s", sender, userid);
   mailog("bbsmail", genbuf);

   /* append the record to the MAIL control file */

   strncpy(mymail.title, title, 72);

   if (strtok(sender, " .@\t\n\r"))
     strcat(sender, ".");
   sender[IDLEN] = '\0';
   strcpy(mymail.owner, sender);
   sprintf(genbuf, "home/%s/.DIR", userid);
   return rec_add(genbuf, &mymail, sizeof(mymail));
}


main(int argc, char** argv)
{
   if (argc < 2) {
      printf("Usage:\t%s <bbs_uid>\n", argv[0]);
      exit(-1);
   }
#ifdef SPAM_
/*
rexchen �s�i�H���v�{��
*/
   spam = (void *)shm_new(SPAM_KEY,  sizeof(*spam));
#endif
   setgid(BBSGID);
   setuid(BBSUID);
   chdir(BBSHOME);
   STRCPY(userid, argv[1]);

   return mail2bbs();
}
