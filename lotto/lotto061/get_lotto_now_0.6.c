/*
��x�_�Ȧ檺������̷s�@�����}�����X~..
��lotto_no_now.txt
ps:�]�}��}�����ɭ��Y���������ç줣��!
   so�o�ӵ{�����ӷ|�b�}���{���̶], ��n�W�ߥX�Ӧb�t�~���ɶ��A�]�@��!
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
        /* clean �}�C */
        for (i=0;i<13;i++){
           strcpy(data_want[i],"�@");
        }
        /* �N�r��Ū�J�}�C */
        i =0 ;
        sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));
        while((p=strtok(NULL," "))){
                i++;
                sprintf(data_want[i],"%s",p);
        }

      if ( strcmp(data_want[0],"����")==0){
        fgets(strREAD_ORG,180,original);
//	printf ("��%d��:%s",readline+1,strREAD_ORG);
	i=0;
	sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));
	while((p=strtok(NULL," "))){
		i++;
		sprintf(data_want[i],"%s",p);
	}

	/* �g�J�ɮ� */
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
