/*

77樂透程式:

  ->檔案<-

	path== ~/lotto/
        =>lotto_record.txt  此期簽賭紀錄
           第一行記錄著     期數
           第二行以後記錄著 ID,簽賭時間,賭金,號碼1~6

        =>lotto_record.txt     簽賭記錄
	=>history/lotto_record_[n].txt
			       第n期簽賭記錄

        =>lotto_no_history.txt 開獎號碼歷史記錄(由開獎程式來記錄)
                               期數,開獎日期,開獎號碼,頭獎總獎金,中獎人數

        =>lotto_no_now.txt     即時查詢到的開獎號碼(一行)(由開獎程式來產生)
		               期數,日期,中獎號(6),特別號,頭獎總獎金,中獎人

	=>lotto_stat.txt       統計狀況
	=>lotto_rule.txt       附在中獎通之後面,我用來寫規則
	=>lotto_open.txt       開獎結果

*/
#include "bbs.h"
#include "lotto.h"
#include <time.h>
int
p_lotto()
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
  pressanykey("感熱紙不夠用了!...下期請早...");
  return;
}


/*讀取get_lotto_now.txt的資料*/
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
   prints("[1;33;44m                           >>>>>七七樂透彩簽賭站<<<<<                         [m\n");
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
     prints("您目前共有 %d 七七金幣, 已經簽了七七樂透%d支",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check是否在簽賭時間 */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"此次您要下多少呢賭注呢? 按 Enter 離開>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(10,0);
     prints("您押了了七七金幣[1;33;40m%ld[m元....",gamble_money);
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
        pressanykey("死~~~~豬頭!!...請下77金幣, 本投注站可不接受買煙賠煙!");
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
   do{
     strcpy(buf,"");
//     move(10,0);
//     prints("小秘訣:[1;35;40m 輸入單一個\"c\", 由七七電腦幫您選號ㄛ![m");
     getdata(9, 0,"請輸入要簽賭的號碼(1~42),每個號碼以空白鍵隔開:", buf, 30, 1, 0);
     
     for(i=0;i<6;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<6;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     /* 電腦選號 ==>此功能在二版已另闢*/
/*
     if(strcmp(lotto_num[0],"c")==0) {
         gettimeofday(&tv,&tz);
	    srand(tv.tv_usec);
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
     }
     else if(atoi(lotto_num[0]) == 0) return;
*/
     if(atoi(lotto_num[0]) == 0) return;

     move(10,0);
     prints("                                                                             ");
     move(10,0);
     prints("已選定下列號碼 : %d %d %d %d %d %d",atoi(lotto_num[0]),atoi(lotto_num[1]),
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     /* 
	判斷所輸入的號碼是不是所屬範圍 1~42 and是否有重覆
        i_tmp ,判別是否要continue do迴路
        default i_tmp = 0 ; 正常
		i_tmp = 1 ; 有輸入不正常的!
     */

     i_tmp1=0;
     for(i=0;i<6;i++){

	/* 判斷是否有超出範圍 */
	if(atoi(lotto_num[i]) > (LOTTO_NUM_MAX)) {
	  pressanykey("請輸入6個1~42的整數");
	  i_tmp1=1;
	  break;
	}
	if(atoi(lotto_num[i]) < (LOTTO_NUM_MIN)) {
	  pressanykey("請輸入6個1~42的整數");
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
   getdata(11,0,"請問要簽下去了嗎?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* bug fix ..奇怪...lotto_num[5]在run過lotto_open_time_check後,
      會變成 "2" ..怪怪~~~  */
   strcpy(buf,lotto_num[5]);

   /* 判斷有沒有在不可以簽賭的時間內 */

   if(lotto_open_time_check()==1) return 0 ;

   strcpy(lotto_num[5],buf);	//bug fix

   /* 寫入檔案 */
	/* >>判斷是不是本期第一次簽賭<< */
	/* 此處已知的bug:
		在跨年時的"期數"並不會自行跨年期
	*/

	pt = localtime(&timep);
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
/*
	if(!(this_record = fopen(buf,"r"))){
		sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
		this_record = fopen(buf,"a");
		sprintf(buf,"%d\n",(atoi(data_now[0])+1));	
		fputs(buf,this_record);
		fclose(this_record);
	}
	else fclose(this_record);
*/
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

   degold(gamble_money);

   pressanykey("謝謝您,歡迎下次再來...");


return ;
}

/* 電腦選號程式 */
c_lotto()
{
FILE *get_now ;			//即時查詢的開獎號碼
FILE *this_record ;		//本期簽賭記錄
FILE *rand_tmp;			//電腦亂數暫存檔
char data_now[10][25];          //get_lotto_now.txt的資料
char data_read[79];
char *p;
char buf[200];
char buf2[200];
long gamble_money;
int i=0;	//暫存用的整數0
int i_tmp2=0;	//............2
char lotto_num[9][10];  //簽賭的號碼
int lotto_amount_tmp=0; //電腦選號產生組數

int j;
int k;
struct timeval tv;
struct timezone tz;


time_t timep;
struct tm *pt;
time(&timep);

if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("感熱紙不夠用了!...下期請早...");
  return;
}


/*讀取get_lotto_now.txt的資料*/
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
   prints("[1;33;44m                           >>>>>七七樂透彩簽賭站<<<<<                         [m\n");
   move(2,10);
   prints("目前已經開獎到第 [1;33;40m%s[m 期, 於 %s 開獎了!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("開獎號碼為 : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   特別號 : [1;35;40m%s[m",data_now[8]);

   /* check是否在簽賭時間 */
   if(lotto_open_time_check()==1) return 0;

   pt = localtime(&timep);

   while(i){
     move(4,10);
     pt = localtime(&timep);
     prints("現在時間為 : %d/%d/%d  星期 %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("請在每個星期二,五的7:30pm~9:00pm 開獎以外的時間簽賭");
     move(7,0);
     prints("您目前共有 %d 七七金幣, 亦已簽了七七樂透%d隻",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check是否在簽賭時間 */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"您要以多少賭注來下每一支電腦選號呢? 按 Enter 離開>", buf, 20, 1, 0);
     gamble_money=atol(buf);
//     move(10,0);
//     prints("七七金幣[1;33;40m%ld[m元/注....",gamble_money);
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
        pressanykey("死~~~~豬頭!!...請下77金幣, 本投注站可不接受買煙賠煙!");
	return 0;	
     }
     if(gamble_money > cuser.goldmoney){
        pressanykey("喂喂~...你應該沒那麼多錢吧?...");
//        continue;	
	return;
     }

     strcpy(buf,"");
     move(10,0);
     prints("請注意:[1;35;40m 您的77金幣是否足夠讓您下那麼多注[m");
     getdata(9, 0,"請輸入您想要讓電腦以上面的金額幫您買幾個號碼呢?:", buf, 6, 1, 0);

     if((atoi(buf)+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("ㄟ~...不能簽那麼多啦!...留點感熱紙給別人用!..下次請早~~");
	 return;
     }
     if(atoi(buf) ==0) return;
     if(atoi(buf) < 0) return;
     
     lotto_amount_tmp=atoi(buf);
     if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	pressanykey("喂喂~...你應該沒那麼多錢吧 ?...");
//	continue;
	return;
     }

     move(11,0);
     prints("您總共下了 %ld元/注 * %d注 = %ld元",gamble_money,atoi(buf),gamble_money*atoi(buf));
     strcpy(buf2,"");
     getdata(12,0,"請確定要下注了(y/n)?[y]",buf2,2,1,0);
     if(buf2[0]=='n') return;
      else if(buf2[0]=='N') return;
      else break;

   }


   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"w");
   fputs("電腦選號結果:\n\n",rand_tmp);
    
   i_tmp2=0;
   while(lotto_amount_tmp > i_tmp2){
     i_tmp2++;

     /* 電腦選號 */

         gettimeofday(&tv,&tz);
	    srand(tv.tv_usec);
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

     /* 排序 */
     lotto_stor(lotto_num);


     sprintf(buf,"第%d組,%d,%d,%d,%d,%d,%d,\n",i_tmp2,atoi(lotto_num[0]),atoi(lotto_num[1]), 
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     fputs(buf,rand_tmp);

   }

   fclose(rand_tmp);
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   more(buf);

   strcpy(buf,"y");
   getdata(22,0,"請問要簽下去了嗎?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;
/*先寫到這ㄦ2002/3/8 */

   /* 判斷有沒有在不可以簽賭的時間內 */
//   if(lotto_open_time_check()==1){
//	pressanykey("tttttt");
//	 return ;
//   }
   /* 寫入檔案 */
	/* >>判斷是不是本期第一次簽賭<< */
	/* 此處已知的bug:
		在跨年時的"期數"並不會自行跨年期
	*/

	pt = localtime(&timep);
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
/*
	if(!(this_record = fopen(buf,"r"))){
		sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
		this_record = fopen(buf,"a");
		sprintf(buf,"%d\n",(atoi(data_now[0])+1));	
		fputs(buf,this_record);
		fclose(this_record);
	}
	else fclose(this_record);
*/
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

// here 之前...ok

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"r");
   fgets(buf,79,rand_tmp);
   fgets(buf,79,rand_tmp);   //第一,二行不是資料!

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
     sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;電腦選號;\n",cuser.userid,
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

   degold(gamble_money*lotto_amount_tmp);

   pressanykey("謝謝您,歡迎下次再來...");


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
   sprintf(buf,"         七月七日晴BBS樂透站[1;33;44m第%s期[m投注記錄查詢結果\n\n",perio);
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
       sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 七七金幣 %s元整 --電腦選號\n"
          ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
          ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }
     else{
       sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 七七金幣 %s元整\n"
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



/* 將本期lotto的簽賭記錄寄給user */
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
	pressanykey("目前尚無資料");
	return 0 ;
   }	

   /* 開啟信件暫存檔 */
   sprintf(mail_tmp,"%stmp/mail_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //先讀取第一行的 第xx期
   
   time(&now);
   sprintf(buf,"作者: 七七銀行樂透部 \n"
               "標題:[通知]第%s期簽賭記錄\n"
               "時間: %s\n",perio,ctime(&now));
   fputs(buf,fp);
   sprintf(buf,"         七月七日晴BBS樂透站[1;33;44m第%s期[m投注記錄查詢結果\n\n",perio);
   fputs(buf,fp);

   /* 開始讀取簽賭記錄檔 */
   while(fgets(buf,180,lotto_record)){
     strcpy(lotto_num[10],"");
     sprintf(lotto_num[0],"%s",strtok(buf,";"));
     if ((strcmp(cuser.userid,lotto_num[0]))) continue;
     i=0;
     while(p2=strtok(NULL,";")){
       i++;
       sprintf(lotto_num[i],"%s",p2);
     }
     
     if(strcmp(lotto_num[10],"電腦選號")==0){  
      sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 七七金幣 %s元整 --電腦選號\n"
        ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
        ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }
     else{
     sprintf(buf,"您在%s %s 時, 投注了 %s %s %s %s %s %s 七七金幣 %s元整\n"
	,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
	,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
     }

     fputs(buf,fp);
   }	

   fclose(fp);
   fclose(lotto_record);


   /* 開始寄回user信箱 */
   sprintf(buf,"home/%s",cuser.userid);
   stampfile(buf,&mymail);
   strcpy(mymail.owner,"七七樂透站");
   rename(mail_tmp,buf);
   sprintf(mymail.title,"[通知] 第%s期 簽賭記錄",perio);
   sprintf(buf,"home/%s/.DIR",cuser.userid);
   rec_add(buf,&mymail,sizeof(mymail));

   update_data();
  
   pressanykey("已經查詢結果寄至您的信箱...謝謝您的始用...");
   
   remove(mail_tmp);
   return 0;

}

menu_lotto(){

FILE *get_now_menu;
char data_read_menu[79];
//char data_now_menu[10][25];
char buf[200];
char *p_menu;
char *data_menu[20];	//menu要用到的data...
char *data_menu2;
int i=0;	
int j=0;
int k=0;

  while(j==0){

   /* 讀取get_lotto_now.txt的資料 */

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now_menu = fopen(buf,"r");
   if(get_now_menu == NULL) {
        pressanykey("目前暫不開放");
        return ;
   }
   fgets(data_read_menu,79,get_now_menu);
   fclose(get_now_menu);
//   sprintf(data_now_menu[i],"%s",strtok(data_read_menu,";"));
   data_menu2=strtok(data_read_menu,";");

   while((p_menu=strtok(NULL,";"))){
        i++;
//        sprintf(data_now_menu[i],"%s",p_menu);
	sprintf(data_menu+i,"%s",p_menu);
   }

//  j=0;
//  while(j==0){

   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>七七樂透彩簽賭站<<<<<                         [m\n");
   move(2,10);
//   prints("目前已經開獎到第 [1;33;40m%s[m 期, 於 %s 開獎了!!",data_now_menu[0],data_now_menu[1]);
   prints("目前已經開獎到第 [1;33;40m%s[m 期",data_menu2);

   move(3,10);
   prints("開獎號碼為 : [1;33;44m");
   for(i=2;i<=7;i++){
//        prints ("%s ",data_now_menu[i]);
        prints ("%s ",data_menu+i); 
   }
//   prints("[m   特別號 : [1;35;40m%s[m",data_now_menu[8]);
   prints("[m   特別號 : [1;35;40m%s[m",data_menu+8);
   move(4,10);
   prints("目前您已經簽了%d支牌,最大可讓你簽%d支",lotto_amount(cuser.userid),LOTTO_AMOUNT_MAX);
	
	/* 開始option */
	i=20;
        k=6;
	move(k,i);
	prints("[1] 手動簽樂透 ");
	move(k+1,i);
	prints("[2] 電腦自動選號 ");
	move(k+2,i);
	prints("[3] 查看您簽賭記錄 ");
	move(k+3,i);
	prints("[4] 將您的簽賭記錄寄回信箱");
	move(k+4,i);
	prints("[0] 離開七七樂透簽賭站(default)");
	strcpy(buf,"");
	getdata(k+6,i,"請輸入您要執行的項目:",buf,2,1,0);
	
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

  }
return ;


}

/* 氣泡排序 lotto_num[5] */
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
