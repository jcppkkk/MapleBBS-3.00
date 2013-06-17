/*     ¹Ø¬Pµ{¦¡               96 10/11            */
/*     the program is designed by Ptt             */
/*     you can emailto://b3504102@csie.ntu.edu.tw */
/*     Ptt BBS telnet://ptt.m8.ntu.edu.tw         */

#include <stdio.h>
#include "bbs.h"
#define HAS_P(x) ((x)?cuser.userlevel&(x):1) 

#define DOTPASSWDS BBSHOME"/.PASSWDS"
#define OUTFILE    BBSHOME"/etc/birth.today"

struct userec cuser;

int
belong(filelist, key)
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
     if ((ptr = strtok(buf, " \t\n\r")) && !strcasecmp(ptr, key))
     {
     rc = 1;
     break;
     }
    }
    fclose(fp);
   }
   return rc;
}
    
                        

main(argc, argv)
int argc;
char **argv;
{
    FILE *fp1;
    char today[50][14],today_name[50][24],month[500][14],
         month_day[500],week[200][14],week_day[200];
    int i,day=0,mon=0,wee=0,a[50],b[50];
    time_t now;
    struct tm *ptime;

    now = time(NULL) ;     /* back to ancent */
    ptime = localtime(&now);

     fp1=fopen(DOTPASSWDS, "r");

    while( (fread( &cuser, sizeof(cuser), 1, fp1))>0 ) {
           if(cuser.month == ptime->tm_mon +1 )
             {
             if(cuser.day == ptime->tm_mday)
               {
               if (!HAS_P(PERM_NOTOP) && !belong("/home/bbs/etc/nontop",cuser.userid))
                {
                a[day]=cuser.numlogins;
                b[day]=cuser.numposts;
                strcpy(today[day  ],cuser.userid);
                strcpy(today_name[day++],cuser.username);
                }
               }
             else if(cuser.day <= ptime->tm_mday+7 && cuser.day >= 
                 ptime->tm_mday-7)
               {
		 if (!HAS_P(PERM_NOTOP) && !belong("/home/bbs/etc/nontop",cuser.userid))  
		 {
                 week_day[wee] = cuser.day;
                 strcpy(week[wee++],cuser.userid);
                 }
               }
             else
               {
		 if (!HAS_P(PERM_NOTOP) && !belong("/home/bbs/etc/nontop",cuser.userid)) 
		 {
                 month_day[mon] = cuser.day;
                 strcpy(month[mon++],cuser.userid);
                 }
               }
             }
      }
    fclose(fp1);

    fp1=fopen(OUTFILE,"w");

    fprintf(fp1,"\n             "
"[1m[36m¡´¢w¢w¢w¢w¢w¢w¢w¢w¡÷[33;42m  ¹Ø¬P¦Cªí  [40;36m¡ö¢w¢w¢w¢w¢w¢w¢w¢w¡´[m\n\n");
    if(day>0)
    {
     fprintf(fp1,"[1m[31m¡i[32m¥»¤é¹Ø¬P[40;31m¡j[m \n");
     for (i=0;i<day;i++)
        {
    fprintf(fp1,"   [1m[33m[%2d/%-2d] %-14s[0m %-24s login:%-5d post:%-5d\n",
           ptime->tm_mon+1,ptime->tm_mday,today[i],today_name[i],a[i],b[i]);
        }
    }
    if(week>0)
    {
     fprintf(fp1,"\n\n[1;33m¡i[46m«e«á¤@¶g¤º¹Ø¬P[40;33m¡j[m \n");
     for (i=0;i<wee;i++)
        {
          fprintf(fp1,"   [%2d/%-2d] [1;36m%-14s[m"
                  ,ptime->tm_mon+1,week_day[i],week[i]);
          if(!((i+1)%3)) fprintf(fp1,"\n");
        }
    }

    fprintf(fp1,"\n\n[1;33m¡i[46m¥»¤ë¹Ø¬P[40;33m¡j[m \n");
    for (i=0;i<mon;i++)
        {
          fprintf(fp1,"   [%2d/%-2d] %-14s"
                  ,ptime->tm_mon+1,month_day[i],month[i]);
          if(!((i+1)%3)) fprintf(fp1,"\n");
        }
        
    fclose(fp1);
    return 0;
}
