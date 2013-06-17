#include "bbs.h"
#include "lotto.h"
#include <time.h>
int
q_lotto()
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

//¥]µP¥Î
FILE *qc_tmp;
int qcnum,qcnum_all,j,k,l,m;  //¥]µP­Ó¼Æ
int lotto_num_time;  //­pºâ¦¸¼Æ
char qbuf[200];
char buf2[200];
char lotto_num_bak[9][10]; 
char lotto_num_all[9][10];

time_t timep;
struct tm *pt;
time(&timep);



if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("·P¼ö¯È¤£°÷¥Î¤F!...¤U´Á½Ð¦­...");
  return;
}


/*Åª¨úget_lotto_now.txtªº¸ê®Æ*/
//   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   sprintf(buf,"/home/bbs/lotto/lotto_no_now.txt");
   get_now = fopen(buf,"r");
   
   if(get_now == NULL) {
        pressanykey("¼È¤£¶}©ñ");
        return 0;
   }
   
   fgets(data_read,79,get_now);
   fclose(get_now);

   sprintf(data_now[i],"%s",strtok(data_read,";"));
   while((p=strtok(NULL,";"))){
        i++;
        sprintf(data_now[i],"%s",p);
   }
   do{
   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>µL¼Æ¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
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
     prints("±z¥Ø«e¦@¦³ %d ª÷¹ô, ¤w¸gÃ±¤FµL¼Æ¼Ö³z%d¤ä",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check¬O§_¦bÃ±½ä®É¶¡ */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"¦¹¦¸±z­n¤U¦h¤Ö½äª`©O? «ö Enter Â÷¶}>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(9,0);

     prints("±z©ã¤F¤Fª÷¹ô[1;33;40m%ld[m¤¸....",gamble_money);
     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	sprintf(buf,"ª`·N!!¥»¼Ö³z¯¸¨Cª`ªº¤Uª`¤U­­¬°ª÷¹ô %d¤¸...",LOTTO_ANTE_MIN);
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	pressanykey("½ÞÀY...§A«ç¤U¤F­tªºª÷¹ô¼Æ©O?..¦A¤U¹L¤@¦¸§a!");
        return;
     }
     if(!gamble_money){
        pressanykey("¦º~~~~½ÞÀY!!...½Ð¤Uª÷¹ô, ¥»§ëª`¯¸¥i¤£±µ¨ü¶R·Ï½ß·Ï!");
	return 0;	
     }
     if(gamble_money > cuser.goldmoney){
        pressanykey("³Þ³Þ~...§AÀ³¸Ó¨S¨º»ò¦h¿ú§a?...");
        return ;
     }
     if(gamble_money < cuser.goldmoney){
	break;
     }


   }
    
/* ¿é¤JÃ±¸@¸¹½X */
//   do{
     strcpy(buf,"");
     getdata(10, 0,"½Ð¿é¤J­n¥]´X­Ó¸¹½X(1~5 or 6~10 or 42)",buf,3,DOECHO,0);
     qcnum = atoi(buf);
     if (qcnum >= 1 && qcnum <=5 || qcnum >= 7 && qcnum <=10)
     {	//¥]¸¹1~10
     sprintf(qbuf,"½Ð¿é¤J­nÃ±½äªº¸¹½X(1~42) %d­Ó,¨C­Ó¸¹½X¥HªÅ¥ÕÁä¹j¶}:\n ",qcnum);
     getdata(11, 0,qbuf, buf, 3*qcnum , DOECHO, 0);
     
     for(i=0;i<qcnum;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<qcnum;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     if(atoi(lotto_num[0]) == 0) return;
     }  //¥]¸¹1~10
     else if (qcnum == 6)
     {
      pressanykey("±þÂû¥Î¤û¤M??");
      return;
     }
     else if (qcnum == 42)
     {
	for(i=0;i<42;i++) sprintf(lotto_num[i],"%d",i+1);
     }
     else
     {
      pressanykey("½Ð¿é¤J¥¿½Tªº­È");
      return;
     }
     move(13,0);
     prints("                                                                             ");
     move(13,0);
     
     prints("¤w¿ï©w¤U¦C¸¹½X : \n");
     for(i=0;i<qcnum;i++)
     {
     prints("%d ",atoi(lotto_num[i]) );
     }

     /* 
	§PÂ_©Ò¿é¤Jªº¸¹½X¬O¤£¬O©ÒÄÝ½d³ò 1~42 and¬O§_¦³­«ÂÐ
        i_tmp ,§P§O¬O§_­ncontinue do°j¸ô
        default i_tmp = 0 ; ¥¿±`
		i_tmp = 1 ; ¦³¿é¤J¤£¥¿±`ªº!
     */

     i_tmp1=0;
     for(i=0;i<qcnum;i++){

	/* §PÂ_¬O§_¦³¶W¥X½d³ò */
	if(atoi(lotto_num[i]) > (LOTTO_NUM_MAX)) {
	  pressanykey("½Ð¿é¤J%d­Ó1~42ªº¾ã¼Æ",qcnum);
	  i_tmp1=1;
	  break;
	}
	if(atoi(lotto_num[i]) < (LOTTO_NUM_MIN)) {
	  pressanykey("½Ð¿é¤J%d­Ó1~42ªº¾ã¼Æ",qcnum);
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
   getdata(16,0,"½Ð°Ý­nÃ±¤U¥h¤F¶Ü?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* bug fix ..©_©Ç...lotto_num[5]¦brun¹Llotto_open_time_check«á,
      ·|ÅÜ¦¨ "2" ..©Ç©Ç~~~  */

   for (i=0;i<42;i++)
   strcpy(lotto_num_bak[i],lotto_num[i]);

   /* §PÂ_¦³¨S¦³¦b¤£¥i¥HÃ±½äªº®É¶¡¤º */

   if(lotto_open_time_check()==1) return 0 ;

   for (i=0;i<42;i++)
   strcpy(lotto_num[i],lotto_num_bak[i]);	//bug fix


   /* ¼g¤JÀÉ®× */
	/* >>§PÂ_¬O¤£¬O¥»´Á²Ä¤@¦¸Ã±½ä<< */
	/* ¦¹³B¤wª¾ªºbug:
		¦b¸ó¦~®Éªº"´Á¼Æ"¨Ã¤£·|¦Û¦æ¸ó¦~´Á
	*/

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

   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   qc_tmp = fopen(buf,"w");
   fputs("¹q¸£¥]¸¹µ²ªG:\n\n",qc_tmp); 
      
//   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
//   this_record=fopen(buf,"a");
//   strcpy(buf,"");
//   pt = localtime(&timep);

   for(i=0;i<42;i++)
    sprintf(lotto_num_all[i],"%d",i+1);

   for(i=0;i<qcnum;i++)
    strcpy(lotto_num_all[i],lotto_num[i]);
    
   for(j=0;j<qcnum;j++)
   {
    qcnum_all = atoi(lotto_num_all[j]);
    sprintf(lotto_num_all[qcnum_all-1],"%d",j+1);
   }
   
   lotto_num_time = 0;
   
   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>µL¼Æ¼Ö³z±mÃ±½ä¯¸<<<<<                         [m\n");
   move(2,10);   
   if (qcnum == 5)
   {
   for(j=qcnum;j<42;j++)
   {
   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,¹q¸£¥]¸¹\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[3]),atoi(lotto_num_all[4]),atoi(lotto_num_all[j]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   } //¥]µPqcnum = 5
   else if (qcnum == 4)
   {
   for(i=qcnum;i<41;i++)
   {
   for(j=i+1;j<42;j++)
   {
   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,¹q¸£¥]¸¹\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[3]),atoi(lotto_num_all[i]),atoi(lotto_num_all[j]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }  
   } //¥]µPqcnum = 4
   else if (qcnum == 3)
   {
   for(i=qcnum;i<40;i++)
   {
   for(j=i+1;j<41;j++)
   {
   for(k=j+1;k<42;k++)
   {
   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,¹q¸£¥]¸¹\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[i]),atoi(lotto_num_all[j]),atoi(lotto_num_all[k]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }  
   }
   } //¥]µPqcnum = 3
   else if (qcnum == 2)
   {
   for(i=qcnum;i<39;i++)
   {
   for(j=i+1;j<40;j++)
   {
   for(k=j+1;k<41;k++)
   {
   for(l=k+1;l<42;l++)
   {
   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,¹q¸£¥]¸¹\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[i]),
           atoi(lotto_num_all[j]),atoi(lotto_num_all[k]),atoi(lotto_num_all[l]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }
   }  
   }
   } //¥]µPqcnum = 2
   else if (qcnum == 1)
   {
   for(i=qcnum;i<39;i++)
   {
   for(j=i+1;j<39;j++)
   {
   for(k=j+1;k<40;k++)
   {
   for(l=k+1;l<41;l++)
   {
   for(m=l+1;m<42;m++)
   {
   sprintf(buf,"²Ä%d²Õ,%d,%d,%d,%d,%d,%d,¹q¸£¥]¸¹\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[i]),atoi(lotto_num_all[j]),
           atoi(lotto_num_all[k]),atoi(lotto_num_all[l]),atoi(lotto_num_all[m]));
   lotto_num_time++; 
   fputs(buf,qc_tmp);
   }
   }
   }
   }  
   }
   } //¥]µPqcnum = 1  

   fclose(qc_tmp);   
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   more(buf);
   
   strcpy(buf,"y");
   getdata(22,0,"½Ð°Ý­nÃ±¤U¥h¤F¶Ü?[y]",buf,4,1,0);
   if(buf[0] == 'n')
   {
    sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
    remove(buf);
    return 0;
   }
   
   degold(gamble_money*lotto_num_time);

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  //¼g¤J¸ê®Æ
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   qc_tmp = fopen(buf,"r");
   fgets(buf,79,qc_tmp);
   fgets(buf,79,qc_tmp);   //²Ä¤@,¤G¦æ¤£¬O¸ê®Æ!
   time(&timep);
   strcpy(buf,""); 

   while(fgets(buf2,79,qc_tmp))
   {
   sprintf(lotto_num[8],"%s",strtok(buf2,","));
   i=0;
    while ((p=strtok(NULL,",")))
     { 
      sprintf(lotto_num[i],"%s",p);
      i++;
     }
     
   pt = localtime(&timep);
   sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;¹q¸£¥]¸¹;\n",cuser.userid,
     1900+pt->tm_year,pt->tm_mon+1,pt->tm_mday,pt->tm_hour,
     pt->tm_min,pt->tm_sec,gamble_money,   
     atoi(lotto_num[0]),atoi(lotto_num[1]),atoi(lotto_num[2]),
     atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
   fputs(buf,this_record); 
   }
   fclose(this_record);
   fclose(qc_tmp); 
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   remove(buf);  

   pressanykey("ÁÂÁÂ±z,±z¤@¦@ªá¤F%d­Óª÷¹ô.Åwªï¤U¦¸¦A¨Ó...",gamble_money*lotto_num_time);

return ;
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
   sprintf(buf,"           µL¼Æ¤£¾ÇBBS¼Ö³z¯¸[1;33;44m²Ä%s´Á[m§ëª`°O¿ý¬d¸ßµ²ªG\n\n",perio);
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


     if(strcmp(lotto_num[10],"¹q¸£¿ï¸¹")==0){     
       sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ª÷¹ô %s¤¸¾ã --¹q¸£¿ï¸¹\n"
          ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
          ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }
     else{
       sprintf(buf,"±z¦b%s %s ®É, §ëª`¤F %s %s %s %s %s %s ª÷¹ô %s¤¸¾ã\n"
	  ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
	  ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }


     fputs(buf,fp);
   }	

   fclose(fp);
   fclose(lotto_record);

   more(mail_tmp);
   remove(mail_tmp);

   return 0;

}




/*ºâºâuseridÃ±¤F´X¤ä*/
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
