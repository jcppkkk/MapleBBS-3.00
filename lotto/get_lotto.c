/*
get_lotto 0.2��

��x�_�Ȧ檺������̷s�@�����}�����X~..
��p�ݪO

change:

from 77lotto 0.7��
   ��092016���_, �_�Ⱥ������G�}�����G�����c���ҧ���, �G���{�����ݭn
   ���ץ�.


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define RESULT_FILE "/home/bbs/m2/13"
//#define RESULT_FILE "./8"

main()
{

FILE *original ;
FILE *get_lotto ;
FILE *l_top ;
int readline=0 ;
int i=0;

char strREAD_ORG[180] ;
char strGET_RESULT[180] ;
char data_want[11][20] ;
char data_put[100];
char *p;

int lotto_top[100];
char buf[80];

   system ("lynx -dump http://www.roclotto.com.tw/query_number.ASP > /home/bbs/org_tmp");
   original = fopen("/home/bbs/org_tmp","r");

   while(fgets(strREAD_ORG,180,original)){
        readline ++ ;
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
        sprintf(data_want[i],"%s",strREAD_ORG);         //����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i],"%s",strREAD_ORG);         //�}�����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        i++;
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
	sprintf(strREAD_ORG,"%s  buffer",strREAD_ORG);
        sprintf(data_want[i],"%s",strtok(strREAD_ORG," "));     //�}����
        while((p=strtok(NULL," "))){
                i++;
                sprintf(data_want[i],"%s",p);
        }
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);               //�S�O��
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);               //�Y���`����
        fgets(strREAD_ORG,180,original);
        fgets(strREAD_ORG,180,original);
        strREAD_ORG[strlen(strREAD_ORG)-1]='\0';
        sprintf(strREAD_ORG,"%s",strtok(strREAD_ORG," "));
        sprintf(data_want[i++],"%s",strREAD_ORG);               //�����H��

	/* �g�J�ɮ� */
	get_lotto = fopen(RESULT_FILE,"w");
	fputs ("\n",get_lotto);

	sprintf(data_put,"     [1;33;45m�� %s ���m��}�����X[m\n",data_want[0]);
	fputs (data_put,get_lotto);
        sprintf(data_put,"     [1;33;40m       %s       [m\n",data_want[1]);
        fputs (data_put,get_lotto);
        fputs ("\n",get_lotto);
        sprintf(data_put,"     [1;37;40m  %s  %s  %s  %s  %s  %s[m\n",
		data_want[2],data_want[3],data_want[4],
		data_want[5],data_want[6],data_want[7]);
        fputs (data_put,get_lotto);
        sprintf(data_put,"     [1;32;40m  �S�O�� : [1;35;40m%d[m\n",atoi(data_want[8]));
        fputs (data_put,get_lotto);
        sprintf(data_put,"     [1;36;40m  �Y��:%s [m\n",data_want[9]);
        fputs (data_put,get_lotto);
	l_top=fopen("/home/bbs/etc/lotto_top.txt","r");
	i=0;
	while(fgets(buf,79,l_top)){
	   lotto_top[i]=atoi(buf);   
	   i++;
	}
	fclose(l_top);
	sprintf(data_put,"   Top3: [1;33;40m%d[m:%d�� [1;33;40m%d[m:%d�� [1;33;40m%d[m:%d��\n"
	   ,lotto_top[0],lotto_top[1],lotto_top[2],lotto_top[3],lotto_top[4]
	   ,lotto_top[5]);
	fputs (data_put,get_lotto);
        fclose(get_lotto);
      }
   }

//   system("rm -f ./org_tmp");
   remove("/home/bbs/org_tmp");
	printf("\nok\n");

}
