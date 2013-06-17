#include "bbs.h"
#include "lotto.h"
#include <time.h>
int
q_lotto()
{
FILE *get_now ;			//即時查詢的開獎號碼
FILE *this_record ;		//本期簽賭記錄
char data_now[10][25];          //get_lotto_now.txt的資料
char data_read[79];
char *p;
char buf[200];
long gamble_money;
int i=0;	//暫存用的整數0
int i_tmp1=0;	//暫存用的整數1
int i_tmp2=0;	//............2
char lotto_num[9][10];  //簽賭的號碼

//包牌用
FILE *qc_tmp;
int qcnum,qcnum_all,j,k,l,m;  //包牌個數
int lotto_num_time;  //計算次數
char qbuf[200];
char buf2[200];
char lotto_num_bak[9][10]; 
char lotto_num_all[9][10];

time_t timep;
struct tm *pt;
time(&timep);



if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("感熱紙不夠用了!...下期請早...");
  return;
}


/*讀取get_lotto_now.txt的資料*/
//   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   sprintf(buf,"/home/bbs/lotto/lotto_no_now.txt");
   get_now = fopen(buf,"r");
   
   if(get_now == NULL) {
        pressanykey("暫不開放");
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
   prints("[1;33;44m                           >>>>>無數樂透彩簽賭站<<<<<                         [m\n");
   move(2,10);
   prints("目前已經開獎到第 [1;33;40m%s[m 期, 於 %s 開獎了!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("開獎號碼為 : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   特別號 : [1;35;40m%s[m",data_now[8]);

   pt = localtime(&timep);

   while(1){
     move(4,10);
     pt = localtime(&timep);
     prints("現在時間為 : %d/%d/%d  星期 %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("請在每個星期二,五的7:30pm~9:00pm 開獎以外的時間簽賭");
     move(7,0);
     prints("您目前共有 %d 金幣, 已經簽了無數樂透%d支",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check是否在簽賭時間 */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"此次您要下多少賭注呢? 按 Enter 離開>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(9,0);

     prints("您押了了金幣[1;33;40m%ld[m元....",gamble_money);
     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	sprintf(buf,"注意!!本樂透站每注的下注下限為金幣 %d元...",LOTTO_ANTE_MIN);
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	pressanykey("豬頭...你怎下了負的金幣數呢?..再下過一次吧!");
        return;
     }
     if(!gamble_money){
        pressanykey("死~~~~豬頭!!...請下金幣, 本投注站可不接受買煙賠煙!");
	return 0;	
     }
     if(gamble_money > cuser.goldmoney){
        pressanykey("喂喂~...你應該沒那麼多錢吧?...");
        return ;
     }
     if(gamble_money < cuser.goldmoney){
	break;
     }


   }
    
/* 輸入簽睹號碼 */
//   do{
     strcpy(buf,"");
     getdata(10, 0,"請輸入要包幾個號碼(1~5 or 6~10 or 42)",buf,3,DOECHO,0);
     qcnum = atoi(buf);
     if (qcnum >= 1 && qcnum <=5 || qcnum >= 7 && qcnum <=10)
     {	//包號1~10
     sprintf(qbuf,"請輸入要簽賭的號碼(1~42) %d個,每個號碼以空白鍵隔開:\n ",qcnum);
     getdata(11, 0,qbuf, buf, 3*qcnum , DOECHO, 0);
     
     for(i=0;i<qcnum;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<qcnum;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     if(atoi(lotto_num[0]) == 0) return;
     }  //包號1~10
     else if (qcnum == 6)
     {
      pressanykey("殺雞用牛刀??");
      return;
     }
     else if (qcnum == 42)
     {
	for(i=0;i<42;i++) sprintf(lotto_num[i],"%d",i+1);
     }
     else
     {
      pressanykey("請輸入正確的值");
      return;
     }
     move(13,0);
     prints("                                                                             ");
     move(13,0);
     
     prints("已選定下列號碼 : \n");
     for(i=0;i<qcnum;i++)
     {
     prints("%d ",atoi(lotto_num[i]) );
     }

     /* 
	判斷所輸入的號碼是不是所屬範圍 1~42 and是否有重覆
        i_tmp ,判別是否要continue do迴路
        default i_tmp = 0 ; 正常
		i_tmp = 1 ; 有輸入不正常的!
     */

     i_tmp1=0;
     for(i=0;i<qcnum;i++){

	/* 判斷是否有超出範圍 */
	if(atoi(lotto_num[i]) > (LOTTO_NUM_MAX)) {
	  pressanykey("請輸入%d個1~42的整數",qcnum);
	  i_tmp1=1;
	  break;
	}
	if(atoi(lotto_num[i]) < (LOTTO_NUM_MIN)) {
	  pressanykey("請輸入%d個1~42的整數",qcnum);
	  i_tmp1=1;
	  break;
	}

	/* 判斷是否有重覆簽號的情況 */
	if(i>0){
	  for(i_tmp2=0;i_tmp2 < i;i_tmp2++){
	     if(atoi(lotto_num[i])==atoi(lotto_num[i_tmp2])){
		pressanykey("...喂~~...別輸入重覆的數字啦!!");
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
   getdata(16,0,"請問要簽下去了嗎?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* bug fix ..奇怪...lotto_num[5]在run過lotto_open_time_check後,
      會變成 "2" ..怪怪~~~  */

   for (i=0;i<42;i++)
   strcpy(lotto_num_bak[i],lotto_num[i]);

   /* 判斷有沒有在不可以簽賭的時間內 */

   if(lotto_open_time_check()==1) return 0 ;

   for (i=0;i<42;i++)
   strcpy(lotto_num[i],lotto_num_bak[i]);	//bug fix


   /* 寫入檔案 */
	/* >>判斷是不是本期第一次簽賭<< */
	/* 此處已知的bug:
		在跨年時的"期數"並不會自行跨年期
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
   fputs("電腦包號結果:\n\n",qc_tmp); 
      
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
   prints("[1;33;44m                           >>>>>無數樂透彩簽賭站<<<<<                         [m\n");
   move(2,10);   
   if (qcnum == 5)
   {
   for(j=qcnum;j<42;j++)
   {
   sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,電腦包號\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[3]),atoi(lotto_num_all[4]),atoi(lotto_num_all[j]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   } //包牌qcnum = 5
   else if (qcnum == 4)
   {
   for(i=qcnum;i<41;i++)
   {
   for(j=i+1;j<42;j++)
   {
   sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,電腦包號\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[3]),atoi(lotto_num_all[i]),atoi(lotto_num_all[j]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }  
   } //包牌qcnum = 4
   else if (qcnum == 3)
   {
   for(i=qcnum;i<40;i++)
   {
   for(j=i+1;j<41;j++)
   {
   for(k=j+1;k<42;k++)
   {
   sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,電腦包號\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[2]),
           atoi(lotto_num_all[i]),atoi(lotto_num_all[j]),atoi(lotto_num_all[k]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }  
   }
   } //包牌qcnum = 3
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
   sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,電腦包號\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[1]),atoi(lotto_num_all[i]),
           atoi(lotto_num_all[j]),atoi(lotto_num_all[k]),atoi(lotto_num_all[l]));
   lotto_num_time++;
   fputs(buf,qc_tmp);
   }
   }
   }  
   }
   } //包牌qcnum = 2
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
   sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,電腦包號\n",lotto_num_time + 1,
           atoi(lotto_num_all[0]),atoi(lotto_num_all[i]),atoi(lotto_num_all[j]),
           atoi(lotto_num_all[k]),atoi(lotto_num_all[l]),atoi(lotto_num_all[m]));
   lotto_num_time++; 
   fputs(buf,qc_tmp);
   }
   }
   }
   }  
   }
   } //包牌qcnum = 1  

   fclose(qc_tmp);   
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   more(buf);
   
   strcpy(buf,"y");
   getdata(22,0,"請問要簽下去了嗎?[y]",buf,4,1,0);
   if(buf[0] == 'n')
   {
    sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
    remove(buf);
    return 0;
   }
   
   degold(gamble_money*lotto_num_time);

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);  //寫入資料
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   qc_tmp = fopen(buf,"r");
   fgets(buf,79,qc_tmp);
   fgets(buf,79,qc_tmp);   //第一,二行不是資料!
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
   sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;電腦包號;\n",cuser.userid,
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

   pressanykey("謝謝您,您一共花了%d個金幣.歡迎下次再來...",gamble_money*lotto_num_time);

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
		   prints("目前的時間是 星期 %d %d:%d, 請在開獎日的 %s∼%s 以外的時間來簽賭",
			   pt->tm_wday,pt->tm_hour,pt->tm_min,
			   LOTTO_OPEN_BEGIN,LOTTO_OPEN_END);
		   pressanykey("請在開獎時間之外簽賭");
		   return 1;
		}
	   }	   
	}
    }
    return 0;
}

/* 查詢目前簽賭的狀況 */
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
	pressanykey("目前尚無資料");
	return 0 ;
   }	

   /* 開啟暫存檔 */
   sprintf(mail_tmp,"%slotto/tmp/ckeck_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //先讀取第一行的 第xx期
   
//   time(&now);
   sprintf(buf,"           無數不學BBS樂透站[1;33;44m第%s期[m投注記錄查詢結果\n\n",perio);
   fputs(buf,fp);

   /* 開始讀取簽賭記錄檔 */
   while(fgets(buf,180,lotto_record)){
     sprintf(lotto_num[0],"%s",strtok(buf,";"));
     strcpy(lotto_num[10],"");
     if ((strcmp(cuser.userid,lotto_num[0]))) continue;
     i=0;
     while(p2=strtok(NULL,";")){
       i++;
       sprintf(lotto_num[i],"%s",p2);
     }


     if(strcmp(lotto_num[10],"電腦選號")==0){     
       sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 金幣 %s元整 --電腦選號\n"
          ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
          ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }
     else{
       sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 金幣 %s元整\n"
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




/*算算userid簽了幾支*/
int lotto_amount(char *userid){

FILE *this_record;	//開啟lotto_record.txt
int i=0;		//簽了幾枝牌
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
