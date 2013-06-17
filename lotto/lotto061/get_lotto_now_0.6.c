/*
到台北銀行的網站抓最新一期的開獎號碼~..
到lotto_no_now.txt
ps:因開剛開獎的時候頭獎的獎金並抓不到!
   so這個程式除來會在開獎程式裡跑, 亦要獨立出來在另外的時間再跑一次!
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

char buf[200];
char strREAD_ORG[180] ;
char strGET_RESULT[180] ;
char data_want[11][20] ;
char data_put[100];
char *p;

//   system ("rm -f ./org_tmp");
   remove(LOTTO_PATH"org_tmp");
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   remove(buf);
   system ("lynx -dump http://www.roclotto.com.tw/query_number.ASP > ~/org_tmp");
   original = fopen(LOTTO_PATH"org_tmp","r");

   while(fgets(strREAD_ORG,180,original)){
      readline ++ ;
        /* clean 陣列 */
        for (i=0;i<13;i++){
           strcpy(data_want[i],"　");
        }
        /* 將字串讀入陣列 */
        i =0 ;
        sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));
        while((p=strtok(NULL," "))){
                i++;
                sprintf(data_want[i],"%s",p);
        }

      if ( strcmp(data_want[0],"期數")==0){
        fgets(strREAD_ORG,180,original);
//	printf ("第%d行:%s",readline+1,strREAD_ORG);
	i=0;
	sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));
	while((p=strtok(NULL," "))){
		i++;
		sprintf(data_want[i],"%s",p);
	}

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
