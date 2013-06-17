/*
77 Lotto 0.7版 get_lotto_now

到台北銀行的網站抓最新一期的開獎號碼~..
到lotto_no_now.txt
ps:因開剛開獎的時候頭獎的獎金並抓不到!
   so這個程式除來會在開獎程式裡跑, 亦要獨立出來在另外的時間再跑一次!

change:

77lotto 0.7版
   自092016期起, 北銀網站公佈開獎結果頁結構有所改變, 故本程式必需要
   做修正.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lotto.h"


main()
{

FILE *original ;
FILE *get_lotto ;
int readline=0 ;
int i=0;
int j=0;

char buf[200];
char strREAD_ORG[180] ;
char strGET_RESULT[180] ;
char data_want[11][20] ;
char data_put[100];
char *p;

   remove(LOTTO_PATH"org_tmp");
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   remove(buf);
   sprintf(buf,"lynx -dump http://www.roclotto.com.tw/query_number.ASP > %sorg_tmp",LOTTO_PATH);
   system (buf);
   original = fopen(LOTTO_PATH"org_tmp","r");

   while(fgets(strREAD_ORG,180,original)){
      readline++ ;
        /* clean 陣列 */
        for (i=0;i<13;i++){
           strcpy(data_want[i],"　");
        }
      strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
      sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
      if ( strcmp(strREAD_ORG,"期數")==0){

        // 12為跳過lunx dump出來的檔的行數,事後視情況而修正
	for(i=0;i<12;i++) {	
          fgets(strREAD_ORG,180,original);
	}
        i=0;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i],"%s",strREAD_ORG);		//期數
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i],"%s",strREAD_ORG);		//開獎日期
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s  buffer",strREAD_ORG);
        sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));	//開獎號
        while((p=strtok(NULL,"  "))){
                i++;
                sprintf(data_want[i],"%s",p);
        }
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//特別號
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//頭獎總獎金
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//中獎人數

	/* 寫入檔案 */
	sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
	get_lotto = fopen(buf,"w");
	sprintf(data_put,"%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s",
		data_want[0],data_want[1],data_want[2],data_want[3],
		data_want[4],data_want[5],data_want[6],data_want[7],
		data_want[8],data_want[9],data_want[10]);
	fputs(data_put,get_lotto);
      }
   }

   remove(LOTTO_PATH"org_tmp");

}
