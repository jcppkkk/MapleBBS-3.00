#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "/home/bbs/src/include/bbs.h"
#include "lotto.h"

/* 登錄user簽的資料筆數 */
lotto_amount_add(char *userid, unsigned int i_add){
FILE *fp;
FILE *lotto_amount_f;
char buf[200];
char buf2[200];
char buf3[200];
//char *p;
unsigned int i=0;

   /* 讀取之前的資料筆數 */

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
      
   /* copy資料到temp */
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
   
   /* 將修正後的data 加入tmp 並把 tmp rename成LOTTO_MAIL_DEFINE */
   sprintf(buf,"%s;%d;\n",userid,i);
   fputs(buf,fp);
   fclose(fp);
   sprintf(buf,"%slotto/tmp/l_amount",LOTTO_PATH);
   remove(buf);
   rename(LOTTO_PATH"lotto/tmp/l_amount_tmp",buf);


}


/*主程式*/
main(){
FILE *fp;
char buf[200];
char buf2[200];
unsigned int i=0;
unsigned long j=0;

   fp=fopen("/home/bbs/lotto/lotto_record.txt","r");
   fgets(buf,180,fp);
   while(fgets(buf,180,fp)){
   	j++;
	sprintf(buf2,"%s",strtok(buf,";"));
	lotto_amount_add(buf2,1);
	printf("%s,",buf2);
   }
   fclose(fp);

}