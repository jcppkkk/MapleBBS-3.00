/*
77 Lotto 0.7�� get_lotto_now

��x�_�Ȧ檺������̷s�@�����}�����X~..
��lotto_no_now.txt
ps:�]�}��}�����ɭ��Y���������ç줣��!
   so�o�ӵ{�����ӷ|�b�}���{���̶], ��n�W�ߥX�Ӧb�t�~���ɶ��A�]�@��!

change:

77lotto 0.7��
   ��092016���_, �_�Ⱥ������G�}�����G�����c���ҧ���, �G���{�����ݭn
   ���ץ�.

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
        /* clean �}�C */
        for (i=0;i<13;i++){
           strcpy(data_want[i],"�@");
        }
      strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
      sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
      if ( strcmp(strREAD_ORG,"����")==0){

        // 12�����Llunx dump�X�Ӫ��ɪ����,�ƫ�����p�ӭץ�
	for(i=0;i<12;i++) {	
          fgets(strREAD_ORG,180,original);
	}
        i=0;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i],"%s",strREAD_ORG);		//����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i],"%s",strREAD_ORG);		//�}�����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s  buffer",strREAD_ORG);
        sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));	//�}����
        while((p=strtok(NULL,"  "))){
                i++;
                sprintf(data_want[i],"%s",p);
        }
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//�S�O��
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//�Y���`����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);		//�����H��

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
