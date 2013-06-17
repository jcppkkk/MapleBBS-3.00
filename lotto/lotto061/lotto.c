/*

ª©Åv«Å§i

77 Lotto 0.6ª©

¥»µ{¦¡­ì«h¤W¶}©ñµ¹¤j®a¦b«DÀç§Qªº±¡ªp¤U¥i¥H¥ô·N­×§ï½Æ»s,
­Y­n¥H¥ô¦ó§Î¦¡´²¼½¥»µ{¦¡½Ð§iª¾§@ªÌ.

¤p§Ì¤~²¨¾Ç²L, µ{¦¡¤¤¦³¥ô¦ó¿ù»~ªº¦a¤è, Àµ½Ð¦U¦ì¥ý¶i¤£§[½ç±Ð!

¹ï©ó¥»µ{¦¡¦³¥ô¦ó°ÝÃDÅwªï¨ì telnet://77bbs.com/Worren. °Q½×;

					³{¥Ò¤j¾Ç¤C¤ë¤C¤é´¸BBS 
					Worren Feung ¶¾°ê´É
					worren.feung@msa.hinet.net
					worren@77bbs.com
					­®«Ç: http://worren.panfamily.org
					bbs : telnet://77bbs.com					

					2002/8/16


77¼Ö³zµ{¦¡:

  ->ÀÉ®×<-

	path== ~/lotto/
        =>lotto_record.txt  ¦¹´ÁÃ±½ä¬ö¿ý
           ²Ä¤@¦æ°O¿ýµÛ     ´Á¼Æ
           ²Ä¤G¦æ¥H«á°O¿ýµÛ ID,Ã±½ä®É¶¡,½äª÷,¸¹½X1~6

        =>lotto_record.txt     Ã±½ä°O¿ý
	=>history/lotto_record_[n].txt
			       ²Än´ÁÃ±½ä°O¿ý

        =>lotto_no_history.txt ¶}¼ú¸¹½X¾ú¥v°O¿ý(¥Ñ¶}¼úµ{¦¡¨Ó°O¿ý)
                               ´Á¼Æ,¶}¼ú¤é´Á,¶}¼ú¸¹½X,ÀY¼úÁ`¼úª÷,¤¤¼ú¤H¼Æ

        =>lotto_no_now.txt     §Y®É¬d¸ß¨ìªº¶}¼ú¸¹½X(¤@¦æ)(¥Ñ¶}¼úµ{¦¡¨Ó²£¥Í)
		               ´Á¼Æ,¤é´Á,¤¤¼ú¸¹(6),¯S§O¸¹,ÀY¼úÁ`¼úª÷,¤¤¼ú¤H

	=>lotto_stat.txt       ²Î­pª¬ªp
	=>lotto_rule.txt       ªþ¦b¤¤¼ú³q¤§«á­±,§Ú¥Î¨Ó¼g³W«h
	=>lotto_open.txt       ¶}¼úµ²ªG

*/
#include "bbs.h"
#include "lotto.h"
#include <time.h>
int
p_lotto()
{
FILE *get_now ;			//§Y®É¬d¸ßªº¶}¼ú¸¹½X
FILE *this_record ;		//¥»´ÁÃ±½ä°O¿ý
char data_now[10][25];          //get_lotto_now.txtªº¸ê®Æ
char data_read[79];
char *p;
char buf[200];
long gamble_money;
int i=0;	//¼È¦s¥Îªº¾ã¼Æ0
int i_tmp1=0;	//¼È¦s¥Îªº¾ã¼Æ1
int i_tmp2=0;	//............2
char lotto_num[9][10];  //Ã±½äªº¸¹½X

/*
int j;
int k;
struct timeval tv;
struct timezone tz;
*/

time_t timep;
struct tm *pt;
time(&timep);

if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("·P¼ö¯È¤£°÷¥Î¤F!...¤U´Á½Ð¦­...");
  return;
}


/*Åª¨úget_lotto_now.txtªº¸ê®Æ*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
        pressanykey("te");
        return 0;
   }
   fgets(data_read,79,get_now);
   fclose(get_now);
   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
	if (i>=8) break;
   }

   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>¤C¤C¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);
   prints("¥Ø«e¤w¸g¶}¼ú¨ì²Ä [1;33;40m%s[m ´Á, ©ó %s ¶}¼ú¤F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("¶}¼ú¸¹½X¬° : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   ¯S§O¸¹ : [1;35;40m%s[m",data_now[8]);

   pt = localtime(&timep);

   while(1){
     move(4,10);
     pt = localtime(&timep);
     prints("²{¦b®É¶¡¬° : %d/%d/%d  ¬P´Á %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("½Ð¦b¨C­Ó¬P´Á¤G,¤­ªº7:30pm~9:00pm ¶}¼ú¥H¥~ªº®É¶¡Ã±½ä");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
          prints("±z¥Ø«e¦@¦³ %d ¤C¤Cª÷¹ô, ¤w¸gÃ±¤F¤C¤C¼Ö³z%d¤ä",cuser.goldmoney,lotto_amount(cuser.userid));	  
     }
     else if(LOTTO_GOLD_SILVER==1){
          prints("±z¥Ø«e¦@¦³ %d ¤C¤C»È¹ô, ¤w¸gÃ±¤F¤C¤C¼Ö³z%d¤ä",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check¬O§_¦bÃ±½ä®É¶¡ */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"¦¹¦¸±z­n¤U¦h¤Ö©O½äª`©O? «ö Enter Â÷¶}>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(10,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("±z©ã¤F¤F¤C¤Cª÷¹ô[1;33;40m%ld[m¤¸....",gamble_money);
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("±z©ã¤F¤F¤C¤C»È¹ô[1;33;40m%ld[m¤¸....",gamble_money);
     }

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
	  sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°ª÷¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
	  sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°»È¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªºª÷¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªº»È¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}
        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77ª÷¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77»È¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	return 0;	
     }

     if(LOTTO_GOLD_SILVER==0){
       if(gamble_money > cuser.goldmoney){
          pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
          return ;
       }
       if(gamble_money < cuser.goldmoney){
	  break;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if(gamble_money > cuser.silvermoney){
          pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h»È¹ô§a?...¥Îª÷¹ô´«¨Ç¹L¨Ó°Õ!!");
          return ;
       }
       if(gamble_money < cuser.silvermoney){
          break;
       }
     }

   }
    
/* ¿é¤JÃ±¸@¸¹½X */
   do{
     strcpy(buf,"");
     getdata(9, 0,"½Ð¿é¤J­nÃ±½äªº¸¹½X(1~42),¨C­Ó¸¹½X¥HªÅ¥ÕÁä¹j¶}:", buf, 30, 1, 0);
     
     for(i=0;i<6;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<6;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     if(atoi(lotto_num[0]) == 0) return;

     move(10,0);
     prints("                                                                             ");
     move(10,0);
     prints("¤w¿ï©w¤U¦C¸¹½X : %d %d %d %d %d %d",atoi(lotto_num[0]),atoi(lotto_num[1]),
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     /* 
	§PÂ_©Ò¿é¤Jªº¸¹½X¬O¤£¬O©ÒÄÝ½d³ò 1~42 and¬O§_¦³­«ÂÐ
        i_tmp ,§P§O¬O§_­ncontinue do°j¸ô
        default i_tmp = 0 ; ¥¿±`
		i_tmp = 1 ; ¦³¿é¤J¤£¥¿±`ªº!
     */

     i_tmp1=0;
     for(i=0;i<6;i++){

	/* §PÂ_¬O§_¦³¶W¥X½d³ò */
	if(atoi(lotto_num[i]) > (LOTTO_NUM_MAX)) {
	  pressanykey("½Ð¿é¤J6­Ó1~42ªº¾ã¼Æ");
	  i_tmp1=1;
	  break;
	}
	if(atoi(lotto_num[i]) < (LOTTO_NUM_MIN)) {
	  pressanykey("½Ð¿é¤J6­Ó1~42ªº¾ã¼Æ");
	  i_tmp1=1;
	  break;
	}

	/* §PÂ_¬O§_¦³­«ÂÐÃ±¸¹ªº±¡ªp */
	if(i>0){
	  for(i_tmp2=0;i_tmp2 < i;i_tmp2++){
	     if(atoi(lotto_num[i])==atoi(lotto_num[i_tmp2])){
		pressanykey("...³Þ~~...§O¿é¤J­«ÂÐªº¼Æ¦r°Õ!!");
		i_tmp1=1;
		break;
	     }
   	  }
	  if(i_tmp1==1) break;
	}

     }
	if(i_tmp1==0) break;
   }while(i_tmp1==1);

   strcpy(buf,"y");
   getdata(11,0,"½Ð°Ý­nÃ±¤U¥h¤F¶Ü?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* bug fix ..©_©Ç...lotto_num[5]¦brun¹Llotto_open_time_check«á,
      ·|ÅÜ¦¨ "2" ..©Ç©Ç~~~  */
   strcpy(buf,lotto_num[5]);

   /* §PÂ_¦³¨S¦³¦b¤£¥i¥HÃ±½äªº®É¶¡¤º */

   if(lotto_open_time_check()==1) return 0 ;

   strcpy(lotto_num[5],buf);	//bug fix

   /* ¼g¤JÀÉ®× */
	/* >>§PÂ_¬O¤£¬O¥»´Á²Ä¤@¦¸Ã±½ä<< */
	/* ¦¹³B¤wª¾ªºbug:
		¦b¸ó¦~®Éªº"´Á¼Æ"¨Ã¤£·|¦Û¦æ¸ó¦~´Á
	*/
/* original 
	pt = localtime(&timep);
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
	this_record = fopen(buf,"r");
	strcpy(buf,"");
	fgets(buf,79,this_record);
	if(strlen(buf)<5){
		
		fclose(this_record);
                sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
                this_record = fopen(buf,"a");
                sprintf(buf,"%d\n",(atoi(data_now[0])+1));
                fputs(buf,this_record);
                fclose(this_record);
	}
	else fclose(this_record);
*/
   pt = localtime(&timep);
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   if(!(this_record = fopen(buf,"r"))){
      sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
      this_record = fopen(buf,"a");
      sprintf(buf,"%d\n",(atoi(data_now[0])+1));
      fputs(buf,this_record);
   }
   fclose(this_record);


   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   this_record=fopen(buf,"a");
   strcpy(buf,"");
   pt = localtime(&timep);
   sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;\n",cuser.userid,
           1900+pt->tm_year,pt->tm_mon+1,pt->tm_mday,pt->tm_hour,
           pt->tm_min,pt->tm_sec,gamble_money,
           atoi(lotto_num[0]),atoi(lotto_num[1]),atoi(lotto_num[2]),
           atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));

   fputs(buf,this_record);
   fclose(this_record);

   if(LOTTO_GOLD_SILVER==0){
       degold(gamble_money);
   }
   else if(LOTTO_GOLD_SILVER==1){
       demoney(gamble_money);
   }

   lotto_amount_add(cuser.userid,1);

   pressanykey("ÁÂÁÂ±z,Åwªï¤U¦¸¦A¨Ó...");


return ;
}

/* ¹q¸£¿ï¸¹µ{¦¡ */
c_lotto()
{
FILE *get_now ;			//§Y®É¬d¸ßªº¶}¼ú¸¹½X
FILE *this_record ;		//¥»´ÁÃ±½ä°O¿ý
FILE *rand_tmp;			//¹q¸£¶Ã¼Æ¼È¦sÀÉ
char data_now[10][25];          //get_lotto_now.txtªº¸ê®Æ
char data_read[79];
char *p;
char buf[200];
char buf2[200];
long gamble_money;
int i=0;	//¼È¦s¥Îªº¾ã¼Æ0
int i_tmp2=0;	//............2
char lotto_num[9][10];  //Ã±½äªº¸¹½X
int lotto_amount_tmp=0; //¹q¸£¿ï¸¹²£¥Í²Õ¼Æ

int j;
int k;
struct timeval tv;
struct timezone tz;


time_t timep;
struct tm *pt;
time(&timep);

if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("·P¼ö¯È¤£°÷¥Î¤F!...¤U´Á½Ð¦­...");
  return;
}


/*Åª¨úget_lotto_now.txtªº¸ê®Æ*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
        pressanykey("te");
        return 0;
   }
   fgets(data_read,79,get_now);
   fclose(get_now);
   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
	if (i>=8) break;
   }

   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>¤C¤C¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);
   prints("¥Ø«e¤w¸g¶}¼ú¨ì²Ä [1;33;40m%s[m ´Á, ©ó %s ¶}¼ú¤F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("¶}¼ú¸¹½X¬° : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   ¯S§O¸¹ : [1;35;40m%s[m",data_now[8]);

   /* check¬O§_¦bÃ±½ä®É¶¡ */
   if(lotto_open_time_check()==1) return 0;

   pt = localtime(&timep);

   while(i){
     move(4,10);
     pt = localtime(&timep);
     prints("²{¦b®É¶¡¬° : %d/%d/%d  ¬P´Á %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("½Ð¦b¨C­Ó¬P´Á¤G,¤­ªº7:30pm~9:00pm ¶}¼ú¥H¥~ªº®É¶¡Ã±½ä");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("±z¥Ø«e¦@¦³ %d ¤C¤Cª÷¹ô, ¥ç¤wÃ±¤F¤C¤C¼Ö³z%d°¦",cuser.goldmoney,lotto_amount(cuser.userid));
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("±z¥Ø«e¦@¦³ %d ¤C¤C»È¹ô, ¥ç¤wÃ±¤F¤C¤C¼Ö³z%d°¦",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check¬O§_¦bÃ±½ä®É¶¡ */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"±z­n¥H¦h¤Ö½äª`¨Ó¤U¨C¤@¤ä¹q¸£¿ï¸¹©O? «ö Enter Â÷¶}>", buf, 20, 1, 0);
     gamble_money=atol(buf);

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
          sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°ª÷¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
          sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°»È¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªºª÷¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªº»È¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}

        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77ª÷¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77»È¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	return 0;	
     }
     if(LOTTO_GOLD_SILVER==0){
        if(gamble_money > cuser.goldmoney){
           pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
	   return;
        }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(gamble_money > cuser.silvermoney){
           pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
	   return;
        }
     }

     strcpy(buf,"");
     move(10,0);
     if(LOTTO_GOLD_SILVER==0){
        prints("½Ðª`·N:[1;35;40m ±zªº77ª÷¹ô¬O§_¨¬°÷Åý±z¤U¨º»ò¦hª`[m");
     }
     else if(LOTTO_GOLD_SILVER==1){
        prints("½Ðª`·N:[1;35;40m ±zªº77»È¹ô¬O§_¨¬°÷Åý±z¤U¨º»ò¦hª`[m");
     }

     getdata(9, 0,"½Ð¿é¤J±z·Q­nÅý¹q¸£¥H¤W­±ªºª÷ÃBÀ°±z¶R´X­Ó¸¹½X©O?:", buf, 6, 1, 0);

     if((atoi(buf)+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("£°~...¤£¯àÃ±¨º»ò¦h°Õ!...¯dÂI·P¼ö¯Èµ¹§O¤H¥Î!..¤U¦¸½Ð¦­~~");
	 return;
     }
     if(atoi(buf) ==0) return;
     if(atoi(buf) < 0) return;
     
     lotto_amount_tmp=atoi(buf);

     if(LOTTO_GOLD_SILVER==0){
       if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	  pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a ?...");
	  return;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if((gamble_money * lotto_amount_tmp) > cuser.silvermoney){
	  pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a ?...");
	  return;
       }
     }

     move(11,0);
     prints("±zÁ`¦@¤U¤F %ld¤¸/ª` * %dª` = %ld¤¸",gamble_money,atoi(buf),gamble_money*atoi(buf));
     strcpy(buf2,"");
     getdata(12,0,"½Ð½T©w­n¤Uª`¤F(y/n)?[y]",buf2,2,1,0);
     if(buf2[0]=='n') return;
      else if(buf2[0]=='N') return;
      else break;

   }


   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"w");
   fputs("¹q¸£¿ï¸¹µ²ªG:\n\n",rand_tmp);
  
   /*¤U¶Ã¼ÆºØ¤l*/
   gettimeofday(&tv,&tz);
   srand(tv.tv_usec);
  
   i_tmp2=0;
   while(lotto_amount_tmp > i_tmp2){
     i_tmp2++;

     /* ¹q¸£¿ï¸¹ */

	    k=0;
	    while(k<6){
	        sprintf(lotto_num[k],"%d",1+(int)(42.0*rand()/(RAND_MAX+1.0)));
	        i=0;
	        for(j=0;j<k;j++){
	           if(atoi(lotto_num[k])==atoi(lotto_num[j])){
        	        i=1;
	                break;
        	   }
	        }
        	if (i==0) k++;
	    }

     /* ±Æ§Ç */
     lotto_stor(lotto_num);


     sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,\n",i_tmp2,atoi(lotto_num[0]),atoi(lotto_num[1]), 
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     fputs(buf,rand_tmp);

   }

   fclose(rand_tmp);
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   more(buf);

   strcpy(buf,"y");
   getdata(22,0,"½Ð°Ý­nÃ±¤U¥h¤F¶Ü?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;
/*¥ý¼g¨ì³o£·2002/3/8 */

   /* §PÂ_¦³¨S¦³¦b¤£¥i¥HÃ±½äªº®É¶¡¤º */
//   if(lotto_open_time_check()==1){
//	pressanykey("tttttt");
//	 return ;
//   }
   /* ¼g¤JÀÉ®× */
	/* >>§PÂ_¬O¤£¬O¥»´Á²Ä¤@¦¸Ã±½ä<< */
	/* ¦¹³B¤wª¾ªºbug:
		¦b¸ó¦~®Éªº"´Á¼Æ"¨Ã¤£·|¦Û¦æ¸ó¦~´Á
	*/
/* original
	pt = localtime(&timep);
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
        this_record = fopen(buf,"r");
        strcpy(buf,"");
        fgets(buf,79,this_record);
        if(strlen(buf)<5){

                fclose(this_record);
                sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
                this_record = fopen(buf,"a");
                sprintf(buf,"%d\n",(atoi(data_now[0])+1));
                fputs(buf,this_record);
                fclose(this_record);
        }
*/
   pt = localtime(&timep);
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   if(!(this_record = fopen(buf,"r"))){
      sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
      this_record = fopen(buf,"a");
      sprintf(buf,"%d\n",(atoi(data_now[0])+1));
      fputs(buf,this_record);
   }
   fclose(this_record);

// here ¤§«e...ok

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"r");
   fgets(buf,79,rand_tmp);
   fgets(buf,79,rand_tmp);   //²Ä¤@,¤G¦æ¤£¬O¸ê®Æ!

   time(&timep);

   strcpy(buf,"");
   while(fgets(buf2,79,rand_tmp)){

     sprintf(lotto_num[8],"%s",strtok(buf2,","));
     i=0;
     while ((p=strtok(NULL,","))){
	sprintf(lotto_num[i],"%s",p);
	i++;
     }

     pt = localtime(&timep);
     sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;¹q¸£¿ï¸¹;\n",cuser.userid,
             1900+pt->tm_year,pt->tm_mon+1,pt->tm_mday,pt->tm_hour,
             pt->tm_min,pt->tm_sec,gamble_money,
             atoi(lotto_num[0]),atoi(lotto_num[1]),atoi(lotto_num[2]),
             atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     fputs(buf,this_record);
   }

   fclose(this_record);
   fclose(rand_tmp);

   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   remove(buf);

   if(LOTTO_GOLD_SILVER==0){
     degold(gamble_money*lotto_amount_tmp);
   }
   if(LOTTO_GOLD_SILVER==1){
     demoney(gamble_money*lotto_amount_tmp);
   }

   lotto_amount_add(cuser.userid,lotto_amount_tmp );

   pressanykey("ÁÂÁÂ±z,Åwªï¤U¦¸¦A¨Ó...");


//return 0;
}

/* ¥]µP¤Uª`µ{¦¡ */
t_lotto()
{
FILE *get_now ;			//§Y®É¬d¸ßªº¶}¼ú¸¹½X
FILE *this_record ;		//¥»´ÁÃ±½ä°O¿ý
FILE *rand_tmp;			//¥]µP¼È¦sÀÉ
char data_now[10][25];          //get_lotto_now.txtªº¸ê®Æ
char data_read[79];
char *p;
char buf[200];
char buf2[200];
long gamble_money;
int i=0;	//¼È¦s¥Îªº¾ã¼Æ0
int i_tmp2=0;	//............2
char lotto_num[9][10];  //Ã±½äªº¸¹½X
char lotto_num_t[80][10];  //­n¥]ªº¸¹½X
int lotto_amount_n=0; //¿é¤J¤Fn­Ó¸¹½X¨Ó²Õ¦X
unsigned long int lotto_amount_tmp=0; //¥]µP²£¥Í²Õ¼Æ
long double j=0;
long double k=0;
int x=0;
int y=0;
int z=0;
int l=0;
int m=0;
int ix=0;
int iy=0;
int iz=0;


time_t timep;
struct tm *pt;
time(&timep);

if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("·P¼ö¯È¤£°÷¥Î¤F!...¤U´Á½Ð¦­...");
  return;
}


/*Åª¨úget_lotto_now.txtªº¸ê®Æ*/
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now = fopen(buf,"r");
   if(get_now == NULL) {
        pressanykey("te");
        return 0;
   }
   fgets(data_read,79,get_now);
   fclose(get_now);
   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
	if (i>=8) break;
   }

   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>¤C¤C¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);
   prints("¥Ø«e¤w¸g¶}¼ú¨ì²Ä [1;33;40m%s[m ´Á, ©ó %s ¶}¼ú¤F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("¶}¼ú¸¹½X¬° : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   ¯S§O¸¹ : [1;35;40m%s[m",data_now[8]);

   /* check¬O§_¦bÃ±½ä®É¶¡ */
   if(lotto_open_time_check()==1) return 0;

   pt = localtime(&timep);

   while(i){
     move(4,10);
     pt = localtime(&timep);
     prints("²{¦b®É¶¡¬° : %d/%d/%d  ¬P´Á %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("½Ð¦b¨C­Ó¬P´Á¤G,¤­ªº7:30pm~9:00pm ¶}¼ú¥H¥~ªº®É¶¡Ã±½ä");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("±z¥Ø«e¦@¦³ %d ¤C¤Cª÷¹ô, ¥ç¤wÃ±¤F¤C¤C¼Ö³z%d°¦",cuser.goldmoney,lotto_amount(cuser.userid));
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("±z¥Ø«e¦@¦³ %d ¤C¤C»È¹ô, ¥ç¤wÃ±¤F¤C¤C¼Ö³z%d°¦",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check¬O§_¦bÃ±½ä®É¶¡ */
     if(lotto_open_time_check()==1) return 0;

     getdata(9, 0,"½Ð¿é¤J±z·Q­n¥]µPªº¸¹½X,¨C­Ó¸¹½X¤§¶¡½Ð¥HªÅ¥ÕÁä¹j¶}:\n", buf, 79, 1, 0);
     if(strcmp("",buf)==0){
	return 0;
     }
     for(i=0;i<81;i++) sprintf(lotto_num_t[i],"","");
     sprintf(lotto_num_t[0],"%s",strtok(buf," "));
     i=1;
     do{
     	    sprintf(buf2,"%s",strtok(NULL," "));
     	    if(atoi(buf2) < 1) break;
	    sprintf(lotto_num_t[i],"%s",buf2);
	    i++;
     }while(i);     

     if(atoi(lotto_num_t[0]) <=0 ) return;

     lotto_amount_n=i; //¥Ø«elotto_amount_n¬°"¿é¤J¤F´X­Ó¸¹½X"
     if( lotto_amount_n <=6 ) return;

     /* §PÂ_¬O§_¦³¥ô¦ó¤@­Ó¼Æ¤£¬O¦b½d³ò¤º */

     for (i=0 ; i<lotto_amount_n ; i++){
	if (atoi(lotto_num_t[i]) < LOTTO_NUM_MIN ) {
	  pressanykey("±z©Ò¤Uªº¸¹½X¤§¤¤¦³¤£¦b½d³ò¤§¤ºªº£¬,­«¤U§a!");
	  return;
	}
	if (atoi(lotto_num_t[i]) > LOTTO_NUM_MAX ) {
	  pressanykey("±z©Ò¤Uªº¸¹½X¤§¤¤¦³¤£¦b½d³ò¤§¤ºªº£¬,­«¤U§a!");
	  return;
        }
     }

     /* §PÂ_¬O§_¦³­«ÂÐ¤F */

     for (l=0; l<lotto_amount_n;l++){
       for (m=l+1; m<lotto_amount_n;m++){
	 if (atoi(lotto_num_t[l])==atoi(lotto_num_t[m])){
           pressanykey("±z©Ò¤Uªº¸¹½X¤§¤¤¦³­«ÂÐªº£¬...­«¤U§a!");
	   return;
         }
       }
     }
   
     /* ¶}©lºâ±Æ¦C²Õ¦X§¹«á·|¦³´X²Õ(C i_tmp2¨ú6) */
     j=1;
     for(iz=lotto_amount_n;iz>6;iz=iz-1){
     	j=j*iz;
     }
     k=1;
     for(ix=(lotto_amount_n-6);ix>0;ix=ix-1){
     	k=k*ix;
     }
     lotto_amount_tmp=(j/k);   //lotto_amount_tmp =±Æ¦C«á·|¦³´X²Õ

     if((lotto_amount_tmp+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("£°~...¤£¯àÃ±¨º»ò¦h°Õ!...¯dÂI·P¼ö¯Èµ¹§O¤H¥Î!..¤U¦¸½Ð¦­~~");
	 return;
     }


move (12,1);
printf("±z¿ï¤F%d­Ó¸¹½X,¥i±Æ¥X%ld²Õ\n",lotto_amount_n,lotto_amount_tmp);

     move(9,0);
     if(LOTTO_GOLD_SILVER==0){
        prints("½Ðª`·N:[1;35;40m ±zªº77ª÷¹ô¬O§_¨¬°÷Åý±z¤U¨º»ò¦hª`[m          ");
     }
     else if(LOTTO_GOLD_SILVER==1){
        prints("½Ðª`·N:[1;35;40m ±zªº77»È¹ô¬O§_¨¬°÷Åý±z¤U¨º»ò¦hª`[m          ");
     }
     getdata(8, 0,"±z­n¥H¦h¤Ö½äª`¨Ó¤U¨C¤@¤ä¥]µP¿ï¸¹©O? «ö Enter Â÷¶}>", buf, 20, 1, 0);
     gamble_money=atol(buf);

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
          sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°ª÷¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
          sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°»È¹ô %d¤¸...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªºª÷¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("½ÞÀY...§A«ç¤U¤F­tªº»È¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
	}

        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77ª÷¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("¦º~~~~½ÞÀY!!...½Ð¤U77»È¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	}
	return 0;	
     }
     if(LOTTO_GOLD_SILVER==0){
        if(gamble_money > cuser.goldmoney){
           pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
	   return;
        }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(gamble_money > cuser.silvermoney){
           pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
	   return;
        }
     }


     if(LOTTO_GOLD_SILVER==0){
       if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	  pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a ?...");
	  return;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if((gamble_money * lotto_amount_tmp) > cuser.silvermoney){
	  pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a ?...");
	  return;
       }
     }

     move(11,0);
     prints("½Ð¬d·Ó: %ld¤¸/ª` * %dª` = %ld¤¸",gamble_money,lotto_amount_tmp,gamble_money * lotto_amount_tmp);
     strcpy(buf2,"");
     getdata(12,0,"½Ð½T©w­n¤Uª`¤F(y/n)?[y]",buf2,2,1,0);
     if(buf2[0]=='n') return;
      else if(buf2[0]=='N') return;
      else break;

   }


   sprintf(buf,"%slotto/tmp/%s_t",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"w");
   fputs("¥]µP±Æ¦C²Õ¦X«áµ²ªG:\n\n",rand_tmp);
   
   /* ±Æ¦C²Õ¦X */
   i_tmp2=0;
   for(x=0;x<lotto_amount_n-6+1;x++){
     for(y=x+1;y< lotto_amount_n-6+2;y++){
	for(z=y+1;z < lotto_amount_n-6+3;z++){
	  for(ix=z+1;ix < lotto_amount_n-6+4;ix++){
	     for(iy=ix+1;iy< lotto_amount_n-6+5;iy++){
		for(iz=iy+1;iz<lotto_amount_n-6+6;iz++){
		   i_tmp2++;
		   sprintf(lotto_num[0],"%s",lotto_num_t[x]);
		   sprintf(lotto_num[1],"%s",lotto_num_t[y]);
		   sprintf(lotto_num[2],"%s",lotto_num_t[z]);
		   sprintf(lotto_num[3],"%s",lotto_num_t[ix]);
		   sprintf(lotto_num[4],"%s",lotto_num_t[iy]);
		   sprintf(lotto_num[5],"%s",lotto_num_t[iz]);
		   lotto_stor(lotto_num); //±Æ§Ç
     		   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,\n",i_tmp2,
			   atoi(lotto_num[0]),atoi(lotto_num[1]),
			   atoi(lotto_num[2]),atoi(lotto_num[3]),
			   atoi(lotto_num[4]),atoi(lotto_num[5]));
		   fputs(buf,rand_tmp);
		}
	     }
	  }
	}
     }
   }
   
   fclose(rand_tmp);
   sprintf(buf,"%slotto/tmp/%s_t",LOTTO_PATH,cuser.userid);
   more(buf);

   strcpy(buf,"y");
   getdata(22,0,"½Ð°Ý­nÃ±¤U¥h¤F¶Ü?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* ¼g¤JÀÉ®× */
	/* >>§PÂ_¬O¤£¬O¥»´Á²Ä¤@¦¸Ã±½ä<< */
	/* ¦¹³B¤wª¾ªºbug:
		¦b¸ó¦~®Éªº"´Á¼Æ"¨Ã¤£·|¦Û¦æ¸ó¦~´Á
	*/
/* original
	pt = localtime(&timep);
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
        this_record = fopen(buf,"r");
        strcpy(buf,"");
        fgets(buf,79,this_record);
        if(strlen(buf)<5){

                fclose(this_record);
                sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
                this_record = fopen(buf,"a");
                sprintf(buf,"%d\n",(atoi(data_now[0])+1));
                fputs(buf,this_record);
                fclose(this_record);
        }
*/
   pt = localtime(&timep);
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   if(!(this_record = fopen(buf,"r"))){
      sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
      this_record = fopen(buf,"a");
      sprintf(buf,"%d\n",(atoi(data_now[0])+1));
      fputs(buf,this_record);
   }
   fclose(this_record);


   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s_t",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"r");
   fgets(buf,79,rand_tmp);
   fgets(buf,79,rand_tmp);   //²Ä¤@,¤G¦æ¤£¬O¸ê®Æ!

time(&timep);

   strcpy(buf,"");
   while(fgets(buf2,79,rand_tmp)){

     sprintf(lotto_num[8],"%s",strtok(buf2,","));
     i=0;
     while ((p=strtok(NULL,","))){
	sprintf(lotto_num[i],"%s",p);
	i++;
     }

     pt = localtime(&timep);
     sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;¥]µP¿ï¸¹;\n",cuser.userid,
             1900+pt->tm_year,pt->tm_mon+1,pt->tm_mday,pt->tm_hour,
             pt->tm_min,pt->tm_sec,gamble_money,
             atoi(lotto_num[0]),atoi(lotto_num[1]),atoi(lotto_num[2]),
             atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     fputs(buf,this_record);
   }

   fclose(this_record);
   fclose(rand_tmp);

   sprintf(buf,"%slotto/tmp/%s_t",LOTTO_PATH,cuser.userid);
   remove(buf);

   if(LOTTO_GOLD_SILVER==0){
     degold(gamble_money*lotto_amount_tmp);
   }
   if(LOTTO_GOLD_SILVER==1){
     demoney(gamble_money*lotto_amount_tmp);
   }

   lotto_amount_add(cuser.userid, lotto_amount_tmp);

   pressanykey("ÁÂÁÂ±z,Åwªï¤U¦¸¦A¨Ó...");


//return 0;
}





int lotto_open_time_check(){

char buf[80];
char open_wday[5][3];
char *lotto_wday;
int i;

time_t timep;
struct tm *pt;
time(&timep);

    strcpy(lotto_wday,LOTTO_OPEN_WDAY);
    sprintf(open_wday[0],"%s",strtok(lotto_wday,","));
    for(i=1; i < LOTTO_OPEN_WTIME ; i++)
	sprintf(open_wday[i],"%s",strtok(NULL,","));

    pt = localtime(&timep);

    for(i=0; i < LOTTO_OPEN_WTIME ; i++){
	if(atoi(open_wday[i])==pt->tm_wday){
//	   sprintf(buf,"%d%d",pt->tm_hour,pt->tm_min);
	   strftime(buf,sizeof(buf),"%H%M",pt);
	   if( atoi(buf) > atoi(LOTTO_OPEN_BEGIN)){
		if( atoi(buf) < atoi(LOTTO_OPEN_END) ){
		   move(15,5);
		   prints("¥Ø«eªº®É¶¡¬O ¬P´Á %d %d:%d, ½Ð¦b¶}¼ú¤éªº %s¡ã%s ¥H¥~ªº®É¶¡¨ÓÃ±½ä",
			   pt->tm_wday,pt->tm_hour,pt->tm_min,
			   LOTTO_OPEN_BEGIN,LOTTO_OPEN_END);
		   pressanykey("½Ð¦b¶}¼ú®É¶¡¤§¥~Ã±½ä");
		   return 1;
		}
	   }	   
	}
    }
    return 0;
}

/* ¬d¸ß¥Ø«eÃ±½äªºª¬ªp */
check_lotto(){

   char buf[256];
   FILE *fp;
   FILE *lotto_record;
   char perio[8];
   int i;
   char lotto_num[11][30];
   char *p2;
   char mail_tmp[400];

//   time_t now;

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   if(!(lotto_record=fopen(buf,"r"))) { 
	pressanykey("¥Ø«e©|µL¸ê®Æ");
	return 0 ;
   }	

   /* ¶}±Ò¼È¦sÀÉ */
   sprintf(mail_tmp,"%slotto/tmp/ckeck_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //¥ýÅª¨ú²Ä¤@¦æªº ²Äxx´Á
   
//   time(&now);
   sprintf(buf,"         ¤C¤ë¤C¤é´¸BBS¼Ö³z¯¸[1;33;44m²Ä%s´Á[m§ëª`°O¿ý¬d¸ßµ²ªG\n\n",perio);
   fputs(buf,fp);

   /* ¶}©lÅª¨úÃ±½ä°O¿ýÀÉ */
   while(fgets(buf,180,lotto_record)){
     sprintf(lotto_num[0],"%s",strtok(buf,";"));
     strcpy(lotto_num[10],"");
     if ((strcmp(cuser.userid,lotto_num[0]))) continue;
     i=0;
     while(p2=strtok(NULL,";")){
       i++;
       sprintf(lotto_num[i],"%s",p2);
     }

     if(LOTTO_GOLD_SILVER==0){
       if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){     
         sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤Cª÷¹ô %s¤¸¾ã --¹q¸£¿ï¸¹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
       else{
         sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤Cª÷¹ô %s¤¸¾ã\n"
	    ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
	    ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
          sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤C»È¹ô %s¤¸¾ã --¹q¸£¿ï¸¹\n"
             ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
             ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
        }
        else{
          sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤C»È¹ô %s¤¸¾ã\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
        }
     }



     fputs(buf,fp);
   }	

   fclose(fp);
   fclose(lotto_record);

   more(mail_tmp);
   remove(mail_tmp);

   return 0;

}



/* ±N¥»´ÁlottoªºÃ±½ä°O¿ý±Hµ¹user */
m_lotto(){

   char buf[256];
   FILE *fp;
   FILE *lotto_record;
   fileheader mymail;
   char perio[8];
   int i;
   char lotto_num[11][30];
   char *p2;
   char mail_tmp[400];

   time_t now;

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   if(!(lotto_record=fopen(buf,"r"))) { 
	pressanykey("¥Ø«e©|µL¸ê®Æ");
	return 0 ;
   }	

   /* ¶}±Ò«H¥ó¼È¦sÀÉ */
   sprintf(mail_tmp,"%stmp/mail_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //¥ýÅª¨ú²Ä¤@¦æªº ²Äxx´Á
   
   time(&now);
   sprintf(buf,"§@ªÌ: ¤C¤C»È¦æ¼Ö³z³¡ \n"
               "¼ÐÃD:[³qª¾]²Ä%s´ÁÃ±½ä°O¿ý\n"
               "®É¶¡: %s\n",perio,ctime(&now));
   fputs(buf,fp);
   sprintf(buf,"         ¤C¤ë¤C¤é´¸BBS¼Ö³z¯¸[1;33;44m²Ä%s´Á[m§ëª`°O¿ý¬d¸ßµ²ªG\n\n",perio);
   fputs(buf,fp);

   /* ¶}©lÅª¨úÃ±½ä°O¿ýÀÉ */
   while(fgets(buf,180,lotto_record)){
     strcpy(lotto_num[10],"");
     sprintf(lotto_num[0],"%s",strtok(buf,";"));
     if ((strcmp(cuser.userid,lotto_num[0]))) continue;
     i=0;
     while(p2=strtok(NULL,";")){
       i++;
       sprintf(lotto_num[i],"%s",p2);
     }

     if(LOTTO_GOLD_SILVER==0){
         if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){  
          sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤Cª÷¹ô %s¤¸¾ã --¹q¸£¿ï¸¹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
         }
         else{
         sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤Cª÷¹ô %s¤¸¾ã\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }
     if(LOTTO_GOLD_SILVER==1){
         if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){
          sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤C»È¹ô %s¤¸¾ã --¹q¸£¿ï¸¹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
         }
         else{
         sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ¤C¤C»È¹ô %s¤¸¾ã\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }


     fputs(buf,fp);
   }	

   fclose(fp);
   fclose(lotto_record);


   /* ¶}©l±H¦^user«H½c */
   sprintf(buf,"home/%s",cuser.userid);
   stampfile(buf,&mymail);
   strcpy(mymail.owner,"¤C¤C¼Ö³z¯¸");
   rename(mail_tmp,buf);
   sprintf(mymail.title,"[³qª¾] ²Ä%s´Á Ã±½ä°O¿ý",perio);
   sprintf(buf,"home/%s/.DIR",cuser.userid);
   rec_add(buf,&mymail,sizeof(mymail));

   update_data();
  
   pressanykey("¤w¸g¬d¸ßµ²ªG±H¦Ü±zªº«H½c...ÁÂÁÂ±zªº©l¥Î...");
   
   remove(mail_tmp);
   return 0;

}

/* Åýuser¦Û¦æ³]©w¬O§_­n±H¦UºØ¤¤¼ú³qª¾ */
define_lotto_mail(){
   
   char buf[256];
   char buf2[256];
   char buf3[256];
   FILE *fp;
   FILE *lotto_mail_define;
   int i;
   int j;
   int k;
   char lotto_define[8][2]; //²Î­p ,1,2,3,4,5,6¼ú, 1=±H,0=¤£±H
   char *p;
   char show_define[7][2][10]; //show¥X­È
   for(j=0;j<7;j++){
      strcpy(show_define[j][0],"¤£±H");
      strcpy(show_define[j][1],"±H");
   }

   /* ½T»{¬O§_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/define_lock","r")){
      fclose(fp);
      sleep(1);
   }
   
   /*Åª¨úuser¥Ø«eªº³]©w­È*/
 while(1){
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_MAIL_DEFINE);   	
   if(!(lotto_mail_define=fopen(buf,"r"))){
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
     while(fgets(buf,180,lotto_mail_define)){
       strcpy(buf2,"");
       sprintf(buf2,"%s",strtok(buf,";"));       
       if(strcmp(buf2,cuser.userid)==0){	 
	 while((p=strtok(NULL,";"))){
          if (!p) break;
          sprintf(lotto_define[i],"%s",p);
          i++;	  
         }
       }
       if(i>0) break;
       i=0;
     }
     fclose(lotto_mail_define);
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
   
   /* show¥X¹º­± */
   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>¤C¤C¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);
   prints("¿Ë·Rªº%s,±z¥Ø«e±Hµo¤¤¼ú³qª¾ªº³]©w­È¦p¤U:",cuser.userid);
   i=20;
   k=4;
   move(k,i);
   prints("[%d] ­Ó¤H¸Ó´Á¦U²Î­p¼Æ¾Ú : %s",k-3,show_define[0][atoi(lotto_define[0])]);
   k++;
   move(k,i);
   prints("[%d] ÀY¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[1][atoi(lotto_define[1])]);
   k++;
   move(k,i);
   prints("[%d] ¤G¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[2][atoi(lotto_define[2])]);
   k++;
   move(k,i);
   prints("[%d] ¤T¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[3][atoi(lotto_define[3])]);
   k++;
   move(k,i);
   prints("[%d] ¥|¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[4][atoi(lotto_define[4])]);
   k++;
   move(k,i);
   prints("[%d] ¤­¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[5][atoi(lotto_define[5])]);
   k++;
   move(k,i);
   prints("[%d] ¤»¼ú¤¤¼ú³qª¾       : %s",k-3,show_define[6][atoi(lotto_define[6])]);
   k++;
   move(k,i);
   prints("[0] ¦^Lotto¥D¥Ø¿ý(default)");
   k++;
   k++;
   getdata(k,i,"½Ð¿é¤J±z­n­×§ïªº¶µ¥Ø:",buf,2,1,0);	

   if(strcmp(buf,"")==0 | strcmp(buf,"0")==0) break;
   if(atoi(buf)>7 | atoi(buf)<1 ) break;
   
   /* update date */
   if(strcmp(lotto_define[atoi(buf)-1],"0")==0) strcpy(lotto_define[atoi(buf)-1],"1");
   else strcpy(lotto_define[atoi(buf)-1],"0");
   /* ¥Ø«e¼È¤£Åýuser¿ï¾Ü¬O§_­n¤£­n­p­Ó¤H²Î­p, ¤@«ß³£±H */
   strcpy(lotto_define[0],"1");
   
   /* ½T»{¬O§_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/define_lock","r")){
      fclose(fp);
      sleep(1);
   }   

   /* ¥[¤JlockÀÉ */   
   sprintf(buf,"/bin/echo lock > %slotto/tmp/define_lock",LOTTO_PATH);
   system (buf);

   /* copy¸ê®Æ¨ìtemp */
   fp=fopen(LOTTO_PATH"lotto/tmp/define_tmp","w");
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_MAIL_DEFINE);
   if(lotto_mail_define=fopen(buf,"r")){
     while(fgets(buf,180,lotto_mail_define)){
       strcpy(buf3,buf);
       strcpy(buf2,"");
       sprintf(buf2,"%s",strtok(buf3,";"));       
       if(strcmp(buf2,cuser.userid)!=0){
       	  sprintf(buf,"%s",buf);
       	  fputs(buf,fp);	
       }
     }
     fclose(lotto_mail_define);
   }
   
   /* ±N­×¥¿«áªºdata ¥[¤Jtmp ¨Ã§â tmp rename¦¨LOTTO_MAIL_DEFINE */
   sprintf(buf,"%s;%s;%s;%s;%s;%s;%s;%s;\n",cuser.userid,lotto_define[0],
           lotto_define[1],lotto_define[2],lotto_define[3],
           lotto_define[4],lotto_define[5],lotto_define[6]);
   fputs(buf,fp);
   fclose(fp);
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_MAIL_DEFINE);
   remove(buf);
   rename(LOTTO_PATH"lotto/tmp/define_tmp",buf);

   /*²¾°£lock*/
   remove(LOTTO_PATH"lotto/tmp/define_lock");
 }
 return;	
}	

menu_lotto(){

FILE *get_now_menu;
FILE *fp;
char data_read_menu[79];
//char data_now_menu[10][25];
char buf[200];
char *p_menu;
char *data_menu[20];	//menu­n¥Î¨ìªºdata...
char *data_menu2;
int i=0;	
int j=0;
int k=0;

/*
Åª¨ú lotto_on_off.txt
if¦pªGÅª±o¨ì lotto_on_off.txt ¥B¤º®e¬° "on" ¨º´N¬O¼Ö³z¯¸Àç·~¤¤
­YÅª¤£¨ì lotto_on_off.txt, ¨º´N¬OÃö³¬¤¤
*/

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
   fp = fopen(buf,"r");
   if(fp == NULL) {
   pressanykey("sorry, ¨t²Î¾ã­×, ¥Ø«e¼È¤£¶}©ñ");
        fclose(fp);
        return 0;
   }

   strcpy(buf,"");
   fgets(buf,20,fp);
   fclose(fp);
   if(strncmp(buf,"on",2)!=0) {
      pressanykey("sorry, ¨t²Î¾ã­×, ¥Ø«e¼È¤£¶}©ñ....");
      return ;
   }
  
//§PÂ_¬O§_¶}©ñ¤¤µ²§ô


  while(j==0){


   /* Åª¨úget_lotto_now.txtªº¸ê®Æ */

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now_menu = fopen(buf,"r");
   if(get_now_menu == NULL) {
        pressanykey("¥Ø«e¼È¤£¶}©ñ");
        return ;
   }
   fgets(data_read_menu,79,get_now_menu);
   fclose(get_now_menu);
   data_menu2=strtok(data_read_menu,";");

   while((p_menu=strtok(NULL,";"))){
        i++;
	sprintf(data_menu+i,"%s",p_menu);
   }

   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>¤C¤C¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);
   prints("¥Ø«e¤w¸g¶}¼ú¨ì²Ä [1;33;40m%s[m ´Á",data_menu2);

   move(3,10);
   prints("¶}¼ú¸¹½X¬° : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_menu+i); 
   }
   prints("[m   ¯S§O¸¹ : [1;35;40m%s[m",data_menu+8);
   move(4,10);
   prints("¥Ø«e±z¤w¸gÃ±¤F%d¤äµP,³Ì¤j¥iÅý§AÃ±%d¤ä",lotto_amount(cuser.userid),LOTTO_AMOUNT_MAX);   

	/* ¶}©loption */
	i=20;
        k=6;
	move(k,i);
	prints("[1] ¤â°ÊÃ±¼Ö³z ");
	move(k+1,i);
	prints("[2] ¹q¸£¦Û°Ê¿ï¸¹ ");
	move(k+2,i);
	prints("[3] ¬d¬Ý±zÃ±½ä°O¿ý ");
	move(k+3,i);
	prints("[4] ±N±zªºÃ±½ä°O¿ý±H¦^«H½c");
	move(k+4,i);
	prints("[5] ¥]µP¤Uª`");
	move(k+5,i);
	prints("[6] ¤¤¼ú³qª¾³]©w");
	move(k+6,i);
	prints("[0] Â÷¶}¤C¤C¼Ö³zÃ±½ä¯¸(default)");
	strcpy(buf,"");
	getdata(k+8,i,"½Ð¿é¤J±z­n°õ¦æªº¶µ¥Ø:",buf,2,1,0);
	
	if(strcmp(buf,"")==0){
	   j=1;
	}

	if(strcmp(buf,"0")==0){
	   j=1;
	}
	if(strcmp(buf,"1")==0){
	   p_lotto();
	   j=0;
	}
	if(strcmp(buf,"2")==0){
	   c_lotto();
	   j=0;
	}
	if(strcmp(buf,"3")==0){
	   check_lotto();
           j=0;
	}
        if(strcmp(buf,"4")==0){
	   m_lotto();
           j=0;
        }
        if(strcmp(buf,"5")==0){
	   t_lotto();
           j=0;
        }
        if(strcmp(buf,"6")==0){
	   define_lotto_mail();
           j=0;
        }


  }
return ;


}

/* ®ðªw±Æ§Ç lotto_num[5] */
lotto_stor(char lotto_num[9][10]){

int tmp1;
int tmp2;
int i;
int j;

  for(i=4;i>=0;i=i-1){     
     for(j=0;j<=i;j++){
	if (atoi(lotto_num[j])>atoi(lotto_num[j+1])){
	   tmp1=atoi(lotto_num[j]);
	   tmp2=atoi(lotto_num[j+1]);
	   sprintf(lotto_num[j],"%d",tmp2);
	   sprintf(lotto_num[j+1],"%d",tmp1);
	}
     }
  }

}

/*ºâºâuseridÃ±¤F´X¤ä*/
/*
int lotto_amount(char *userid){

FILE *this_record;	//¶}±Òlotto_record.txt
int i=0;		//Ã±¤F´XªKµP
char buf[100];
char buf2[100];

  sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
  if(!(this_record=fopen(buf,"r")))  this_record=fopen(buf,"a+");

  while(fgets(buf,180,this_record)){
        strcpy(buf2,"");
	sprintf(buf2,"%s",strtok(buf,";"));
        if(strcmp(buf2,userid)==0) i++;
  }

  fclose(this_record);
  return i;

}
*/

/* µn¿ýuserÃ±ªº¸ê®Æµ§¼Æ */
lotto_amount_add(char *userid, unsigned int i_add){
FILE *fp;
FILE *lotto_amount_f;
char buf[200];
char buf2[200];
char buf3[200];
//char *p;
unsigned int i=0;

   /* Åª¨ú¤§«eªº¸ê®Æµ§¼Æ */

   if(fp=fopen(LOTTO_PATH"lotto/tmp/l_amount","r")){
   	while(fgets(buf,180,fp)){
            strcpy(buf2,"");
            sprintf(buf2,"%s",strtok(buf,";"));
            if(strcmp(buf2,userid)==0){
       	       i=atoi(strtok(NULL,";"));
       	       break;       	       
            }   		
   	}
   	fclose(fp);   	
   }
   i=i+i_add;
      
    /* ½T»{¬O§_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/l_amount_lock","r")){
      fclose(fp);
      sleep(1);
   }  
 
   /* ¥[¤JlockÀÉ */   
   sprintf(buf,"/bin/echo lock > %slotto/tmp/l_amount_lock",LOTTO_PATH);
   system (buf);

   /* copy¸ê®Æ¨ìtemp */
   fp=fopen(LOTTO_PATH"lotto/tmp/l_amount_tmp","w");
   sprintf(buf,"%slotto/tmp/l_amount",LOTTO_PATH);
   if(lotto_amount_f=fopen(buf,"r")){
     while(fgets(buf,180,lotto_amount_f)){
       strcpy(buf3,buf);
       strcpy(buf2,"");
       sprintf(buf2,"%s",strtok(buf3,";"));       
       if(strcmp(buf2,userid)!=0){
       	  sprintf(buf,"%s",buf);
       	  fputs(buf,fp);	
       }
     }
     fclose(lotto_amount_f);
   }
   
   /* ±N­×¥¿«áªºdata ¥[¤Jtmp ¨Ã§â tmp rename¦¨LOTTO_MAIL_DEFINE */
   sprintf(buf,"%s;%d;\n",userid,i);
   fputs(buf,fp);
   fclose(fp);
   sprintf(buf,"%slotto/tmp/l_amount",LOTTO_PATH);
   remove(buf);
   rename(LOTTO_PATH"lotto/tmp/l_amount_tmp",buf);

   /*²¾°£lock*/
   remove(LOTTO_PATH"lotto/tmp/l_amount_lock");

}

/* ÅªuseridÃ±¤F´X¤ä */
lotto_amount(char *userid){

FILE *fp;
char buf[200];
char buf2[200];
unsigned int i=0;

   /* Åª¨ú¥Ø«eªº¸ê®Æµ§¼Æ */
   if(fp=fopen(LOTTO_PATH"lotto/tmp/l_amount","r")){
   	while(fgets(buf,180,fp)){
            strcpy(buf2,"");
            sprintf(buf2,"%s",strtok(buf,";"));
            if(strcmp(buf2,userid)==0){
       	       i=atoi(strtok(NULL,";"));
       	       break;       	       
            }   		
   	}
   	fclose(fp);   	
   }
   return i;
}
