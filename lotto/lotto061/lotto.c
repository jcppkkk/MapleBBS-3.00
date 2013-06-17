/*

���v�ŧi

77 Lotto 0.6��

���{����h�W�}�񵹤j�a�b�D��Q�����p�U�i�H���N�ק�ƻs,
�Y�n�H����Φ��������{���Чi���@��.

�p�̤~���ǲL, �{������������~���a��, ���ЦU����i���[���!

��󥻵{����������D�w��� telnet://77bbs.com/Worren. �Q��;

					�{�Ҥj�ǤC��C�鴸BBS 
					Worren Feung �����
					worren.feung@msa.hinet.net
					worren@77bbs.com
					����: http://worren.panfamily.org
					bbs : telnet://77bbs.com					

					2002/8/16


77�ֳz�{��:

  ->�ɮ�<-

	path== ~/lotto/
        =>lotto_record.txt  ����ñ�����
           �Ĥ@��O����     ����
           �ĤG��H��O���� ID,ñ��ɶ�,���,���X1~6

        =>lotto_record.txt     ñ��O��
	=>history/lotto_record_[n].txt
			       ��n��ñ��O��

        =>lotto_no_history.txt �}�����X���v�O��(�Ѷ}���{���ӰO��)
                               ����,�}�����,�}�����X,�Y���`����,�����H��

        =>lotto_no_now.txt     �Y�ɬd�ߨ쪺�}�����X(�@��)(�Ѷ}���{���Ӳ���)
		               ����,���,������(6),�S�O��,�Y���`����,�����H

	=>lotto_stat.txt       �έp���p
	=>lotto_rule.txt       ���b�����q���᭱,�ڥΨӼg�W�h
	=>lotto_open.txt       �}�����G

*/
#include "bbs.h"
#include "lotto.h"
#include <time.h>
int
p_lotto()
{
FILE *get_now ;			//�Y�ɬd�ߪ��}�����X
FILE *this_record ;		//����ñ��O��
char data_now[10][25];          //get_lotto_now.txt�����
char data_read[79];
char *p;
char buf[200];
long gamble_money;
int i=0;	//�Ȧs�Ϊ����0
int i_tmp1=0;	//�Ȧs�Ϊ����1
int i_tmp2=0;	//............2
char lotto_num[9][10];  //ñ�䪺���X

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
  pressanykey("�P���Ȥ����ΤF!...�U���Ц�...");
  return;
}


/*Ū��get_lotto_now.txt�����*/
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
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
   prints("�ثe�w�g�}����� [1;33;40m%s[m ��, �� %s �}���F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("�}�����X�� : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   �S�O�� : [1;35;40m%s[m",data_now[8]);

   pt = localtime(&timep);

   while(1){
     move(4,10);
     pt = localtime(&timep);
     prints("�{�b�ɶ��� : %d/%d/%d  �P�� %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("�Цb�C�ӬP���G,����7:30pm~9:00pm �}���H�~���ɶ�ñ��");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
          prints("�z�ثe�@�� %d �C�C����, �w�gñ�F�C�C�ֳz%d��",cuser.goldmoney,lotto_amount(cuser.userid));	  
     }
     else if(LOTTO_GOLD_SILVER==1){
          prints("�z�ثe�@�� %d �C�C�ȹ�, �w�gñ�F�C�C�ֳz%d��",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check�O�_�bñ��ɶ� */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"�����z�n�U�h�֩O��`�O? �� Enter ���}>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(10,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("�z��F�F�C�C����[1;33;40m%ld[m��....",gamble_money);
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("�z��F�F�C�C�ȹ�[1;33;40m%ld[m��....",gamble_money);
     }

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
	  sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�������� %d��...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
	  sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�����ȹ� %d��...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("���Y...�A��U�F�t�������ƩO?..�A�U�L�@���a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("���Y...�A��U�F�t���ȹ��ƩO?..�A�U�L�@���a!");
	}
        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("��~~~~���Y!!...�ФU77����, ����`���i�������R�Ͻ߷�!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("��~~~~���Y!!...�ФU77�ȹ�, ����`���i�������R�Ͻ߷�!");
	}
	return 0;	
     }

     if(LOTTO_GOLD_SILVER==0){
       if(gamble_money > cuser.goldmoney){
          pressanykey("�޳�~...�A���ӨS����h���a?...");
          return ;
       }
       if(gamble_money < cuser.goldmoney){
	  break;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if(gamble_money > cuser.silvermoney){
          pressanykey("�޳�~...�A���ӨS����h�ȹ��a?...�Ϊ������ǹL�Ӱ�!!");
          return ;
       }
       if(gamble_money < cuser.silvermoney){
          break;
       }
     }

   }
    
/* ��Jñ�@���X */
   do{
     strcpy(buf,"");
     getdata(9, 0,"�п�J�nñ�䪺���X(1~42),�C�Ӹ��X�H�ť���j�}:", buf, 30, 1, 0);
     
     for(i=0;i<6;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<6;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     if(atoi(lotto_num[0]) == 0) return;

     move(10,0);
     prints("                                                                             ");
     move(10,0);
     prints("�w��w�U�C���X : %d %d %d %d %d %d",atoi(lotto_num[0]),atoi(lotto_num[1]),
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     /* 
	�P�_�ҿ�J�����X�O���O���ݽd�� 1~42 and�O�_������
        i_tmp ,�P�O�O�_�ncontinue do�j��
        default i_tmp = 0 ; ���`
		i_tmp = 1 ; ����J�����`��!
     */

     i_tmp1=0;
     for(i=0;i<6;i++){

	/* �P�_�O�_���W�X�d�� */
	if(atoi(lotto_num[i]) > (LOTTO_NUM_MAX)) {
	  pressanykey("�п�J6��1~42�����");
	  i_tmp1=1;
	  break;
	}
	if(atoi(lotto_num[i]) < (LOTTO_NUM_MIN)) {
	  pressanykey("�п�J6��1~42�����");
	  i_tmp1=1;
	  break;
	}

	/* �P�_�O�_������ñ�������p */
	if(i>0){
	  for(i_tmp2=0;i_tmp2 < i;i_tmp2++){
	     if(atoi(lotto_num[i])==atoi(lotto_num[i_tmp2])){
		pressanykey("...��~~...�O��J���Ъ��Ʀr��!!");
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
   getdata(11,0,"�аݭnñ�U�h�F��?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* bug fix ..�_��...lotto_num[5]�brun�Llotto_open_time_check��,
      �|�ܦ� "2" ..�ǩ�~~~  */
   strcpy(buf,lotto_num[5]);

   /* �P�_���S���b���i�Hñ�䪺�ɶ��� */

   if(lotto_open_time_check()==1) return 0 ;

   strcpy(lotto_num[5],buf);	//bug fix

   /* �g�J�ɮ� */
	/* >>�P�_�O���O�����Ĥ@��ñ��<< */
	/* ���B�w����bug:
		�b��~�ɪ�"����"�ä��|�ۦ��~��
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

   pressanykey("���±z,�w��U���A��...");


return ;
}

/* �q���︹�{�� */
c_lotto()
{
FILE *get_now ;			//�Y�ɬd�ߪ��}�����X
FILE *this_record ;		//����ñ��O��
FILE *rand_tmp;			//�q���üƼȦs��
char data_now[10][25];          //get_lotto_now.txt�����
char data_read[79];
char *p;
char buf[200];
char buf2[200];
long gamble_money;
int i=0;	//�Ȧs�Ϊ����0
int i_tmp2=0;	//............2
char lotto_num[9][10];  //ñ�䪺���X
int lotto_amount_tmp=0; //�q���︹���Ͳռ�

int j;
int k;
struct timeval tv;
struct timezone tz;


time_t timep;
struct tm *pt;
time(&timep);

if (lotto_amount(cuser.userid) > (LOTTO_AMOUNT_MAX-1)){
  pressanykey("�P���Ȥ����ΤF!...�U���Ц�...");
  return;
}


/*Ū��get_lotto_now.txt�����*/
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
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
   prints("�ثe�w�g�}����� [1;33;40m%s[m ��, �� %s �}���F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("�}�����X�� : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   �S�O�� : [1;35;40m%s[m",data_now[8]);

   /* check�O�_�bñ��ɶ� */
   if(lotto_open_time_check()==1) return 0;

   pt = localtime(&timep);

   while(i){
     move(4,10);
     pt = localtime(&timep);
     prints("�{�b�ɶ��� : %d/%d/%d  �P�� %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("�Цb�C�ӬP���G,����7:30pm~9:00pm �}���H�~���ɶ�ñ��");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("�z�ثe�@�� %d �C�C����, ��wñ�F�C�C�ֳz%d��",cuser.goldmoney,lotto_amount(cuser.userid));
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("�z�ثe�@�� %d �C�C�ȹ�, ��wñ�F�C�C�ֳz%d��",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check�O�_�bñ��ɶ� */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"�z�n�H�h�ֽ�`�ӤU�C�@��q���︹�O? �� Enter ���}>", buf, 20, 1, 0);
     gamble_money=atol(buf);

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
          sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�������� %d��...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
          sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�����ȹ� %d��...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("���Y...�A��U�F�t�������ƩO?..�A�U�L�@���a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("���Y...�A��U�F�t���ȹ��ƩO?..�A�U�L�@���a!");
	}

        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("��~~~~���Y!!...�ФU77����, ����`���i�������R�Ͻ߷�!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("��~~~~���Y!!...�ФU77�ȹ�, ����`���i�������R�Ͻ߷�!");
	}
	return 0;	
     }
     if(LOTTO_GOLD_SILVER==0){
        if(gamble_money > cuser.goldmoney){
           pressanykey("�޳�~...�A���ӨS����h���a?...");
	   return;
        }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(gamble_money > cuser.silvermoney){
           pressanykey("�޳�~...�A���ӨS����h���a?...");
	   return;
        }
     }

     strcpy(buf,"");
     move(10,0);
     if(LOTTO_GOLD_SILVER==0){
        prints("�Ъ`�N:[1;35;40m �z��77�����O�_�������z�U����h�`[m");
     }
     else if(LOTTO_GOLD_SILVER==1){
        prints("�Ъ`�N:[1;35;40m �z��77�ȹ��O�_�������z�U����h�`[m");
     }

     getdata(9, 0,"�п�J�z�Q�n���q���H�W�������B���z�R�X�Ӹ��X�O?:", buf, 6, 1, 0);

     if((atoi(buf)+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("��~...����ñ����h��!...�d�I�P���ȵ��O�H��!..�U���Ц�~~");
	 return;
     }
     if(atoi(buf) ==0) return;
     if(atoi(buf) < 0) return;
     
     lotto_amount_tmp=atoi(buf);

     if(LOTTO_GOLD_SILVER==0){
       if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	  pressanykey("�޳�~...�A���ӨS����h���a ?...");
	  return;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if((gamble_money * lotto_amount_tmp) > cuser.silvermoney){
	  pressanykey("�޳�~...�A���ӨS����h���a ?...");
	  return;
       }
     }

     move(11,0);
     prints("�z�`�@�U�F %ld��/�` * %d�` = %ld��",gamble_money,atoi(buf),gamble_money*atoi(buf));
     strcpy(buf2,"");
     getdata(12,0,"�нT�w�n�U�`�F(y/n)?[y]",buf2,2,1,0);
     if(buf2[0]=='n') return;
      else if(buf2[0]=='N') return;
      else break;

   }


   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"w");
   fputs("�q���︹���G:\n\n",rand_tmp);
  
   /*�U�üƺؤl*/
   gettimeofday(&tv,&tz);
   srand(tv.tv_usec);
  
   i_tmp2=0;
   while(lotto_amount_tmp > i_tmp2){
     i_tmp2++;

     /* �q���︹ */

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

     /* �Ƨ� */
     lotto_stor(lotto_num);


     sprintf(buf,"��%d��,%d,%d,%d,%d,%d,%d,\n",i_tmp2,atoi(lotto_num[0]),atoi(lotto_num[1]), 
	     atoi(lotto_num[2]),atoi(lotto_num[3]),atoi(lotto_num[4]),atoi(lotto_num[5]));
     fputs(buf,rand_tmp);

   }

   fclose(rand_tmp);
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   more(buf);

   strcpy(buf,"y");
   getdata(22,0,"�аݭnñ�U�h�F��?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;
/*���g��o��2002/3/8 */

   /* �P�_���S���b���i�Hñ�䪺�ɶ��� */
//   if(lotto_open_time_check()==1){
//	pressanykey("tttttt");
//	 return ;
//   }
   /* �g�J�ɮ� */
	/* >>�P�_�O���O�����Ĥ@��ñ��<< */
	/* ���B�w����bug:
		�b��~�ɪ�"����"�ä��|�ۦ��~��
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

// here ���e...ok

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_RECORD);
   this_record=fopen(buf,"a");
   sprintf(buf,"%slotto/tmp/%s",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"r");
   fgets(buf,79,rand_tmp);
   fgets(buf,79,rand_tmp);   //�Ĥ@,�G�椣�O���!

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
     sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;�q���︹;\n",cuser.userid,
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

   pressanykey("���±z,�w��U���A��...");


//return 0;
}

/* �]�P�U�`�{�� */
t_lotto()
{
FILE *get_now ;			//�Y�ɬd�ߪ��}�����X
FILE *this_record ;		//����ñ��O��
FILE *rand_tmp;			//�]�P�Ȧs��
char data_now[10][25];          //get_lotto_now.txt�����
char data_read[79];
char *p;
char buf[200];
char buf2[200];
long gamble_money;
int i=0;	//�Ȧs�Ϊ����0
int i_tmp2=0;	//............2
char lotto_num[9][10];  //ñ�䪺���X
char lotto_num_t[80][10];  //�n�]�����X
int lotto_amount_n=0; //��J�Fn�Ӹ��X�ӲզX
unsigned long int lotto_amount_tmp=0; //�]�P���Ͳռ�
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
  pressanykey("�P���Ȥ����ΤF!...�U���Ц�...");
  return;
}


/*Ū��get_lotto_now.txt�����*/
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
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
   prints("�ثe�w�g�}����� [1;33;40m%s[m ��, �� %s �}���F!! ",data_now[0],data_now[1]);
   move(3,10);
   prints("�}�����X�� : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_now[i]);
   }
   prints("[m   �S�O�� : [1;35;40m%s[m",data_now[8]);

   /* check�O�_�bñ��ɶ� */
   if(lotto_open_time_check()==1) return 0;

   pt = localtime(&timep);

   while(i){
     move(4,10);
     pt = localtime(&timep);
     prints("�{�b�ɶ��� : %d/%d/%d  �P�� %d  %d:%d:%d ",(1900+pt->tm_year),
       (pt->tm_mon+1),(pt->tm_mday),(pt->tm_wday),(pt->tm_hour),(pt->tm_min),
       (pt->tm_sec));
     move(5,10);
     prints("�Цb�C�ӬP���G,����7:30pm~9:00pm �}���H�~���ɶ�ñ��");
     move(7,0);
     if(LOTTO_GOLD_SILVER==0){
       prints("�z�ثe�@�� %d �C�C����, ��wñ�F�C�C�ֳz%d��",cuser.goldmoney,lotto_amount(cuser.userid));
     }
     else if(LOTTO_GOLD_SILVER==1){
       prints("�z�ثe�@�� %d �C�C�ȹ�, ��wñ�F�C�C�ֳz%d��",cuser.silvermoney,lotto_amount(cuser.userid));
     }

     /* check�O�_�bñ��ɶ� */
     if(lotto_open_time_check()==1) return 0;

     getdata(9, 0,"�п�J�z�Q�n�]�P�����X,�C�Ӹ��X�����ХH�ť���j�}:\n", buf, 79, 1, 0);
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

     lotto_amount_n=i; //�ثelotto_amount_n��"��J�F�X�Ӹ��X"
     if( lotto_amount_n <=6 ) return;

     /* �P�_�O�_������@�ӼƤ��O�b�d�� */

     for (i=0 ; i<lotto_amount_n ; i++){
	if (atoi(lotto_num_t[i]) < LOTTO_NUM_MIN ) {
	  pressanykey("�z�ҤU�����X���������b�d�򤧤�����,���U�a!");
	  return;
	}
	if (atoi(lotto_num_t[i]) > LOTTO_NUM_MAX ) {
	  pressanykey("�z�ҤU�����X���������b�d�򤧤�����,���U�a!");
	  return;
        }
     }

     /* �P�_�O�_�����ФF */

     for (l=0; l<lotto_amount_n;l++){
       for (m=l+1; m<lotto_amount_n;m++){
	 if (atoi(lotto_num_t[l])==atoi(lotto_num_t[m])){
           pressanykey("�z�ҤU�����X���������Ъ���...���U�a!");
	   return;
         }
       }
     }
   
     /* �}�l��ƦC�զX����|���X��(C i_tmp2��6) */
     j=1;
     for(iz=lotto_amount_n;iz>6;iz=iz-1){
     	j=j*iz;
     }
     k=1;
     for(ix=(lotto_amount_n-6);ix>0;ix=ix-1){
     	k=k*ix;
     }
     lotto_amount_tmp=(j/k);   //lotto_amount_tmp =�ƦC��|���X��

     if((lotto_amount_tmp+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("��~...����ñ����h��!...�d�I�P���ȵ��O�H��!..�U���Ц�~~");
	 return;
     }


move (12,1);
printf("�z��F%d�Ӹ��X,�i�ƥX%ld��\n",lotto_amount_n,lotto_amount_tmp);

     move(9,0);
     if(LOTTO_GOLD_SILVER==0){
        prints("�Ъ`�N:[1;35;40m �z��77�����O�_�������z�U����h�`[m          ");
     }
     else if(LOTTO_GOLD_SILVER==1){
        prints("�Ъ`�N:[1;35;40m �z��77�ȹ��O�_�������z�U����h�`[m          ");
     }
     getdata(8, 0,"�z�n�H�h�ֽ�`�ӤU�C�@��]�P�︹�O? �� Enter ���}>", buf, 20, 1, 0);
     gamble_money=atol(buf);

     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	if(LOTTO_GOLD_SILVER==0){
          sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�������� %d��...",LOTTO_ANTE_MIN);
	}
	else if(LOTTO_GOLD_SILVER==1){
          sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�����ȹ� %d��...",LOTTO_ANTE_MIN);
	}
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	if(LOTTO_GOLD_SILVER==0){
	  pressanykey("���Y...�A��U�F�t�������ƩO?..�A�U�L�@���a!");
	}
	else if(LOTTO_GOLD_SILVER==1){
	  pressanykey("���Y...�A��U�F�t���ȹ��ƩO?..�A�U�L�@���a!");
	}

        return;
     }
     if(!gamble_money){
	if(LOTTO_GOLD_SILVER==0){
           pressanykey("��~~~~���Y!!...�ФU77����, ����`���i�������R�Ͻ߷�!");
	}
	else if(LOTTO_GOLD_SILVER==1){
           pressanykey("��~~~~���Y!!...�ФU77�ȹ�, ����`���i�������R�Ͻ߷�!");
	}
	return 0;	
     }
     if(LOTTO_GOLD_SILVER==0){
        if(gamble_money > cuser.goldmoney){
           pressanykey("�޳�~...�A���ӨS����h���a?...");
	   return;
        }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(gamble_money > cuser.silvermoney){
           pressanykey("�޳�~...�A���ӨS����h���a?...");
	   return;
        }
     }


     if(LOTTO_GOLD_SILVER==0){
       if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	  pressanykey("�޳�~...�A���ӨS����h���a ?...");
	  return;
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
       if((gamble_money * lotto_amount_tmp) > cuser.silvermoney){
	  pressanykey("�޳�~...�A���ӨS����h���a ?...");
	  return;
       }
     }

     move(11,0);
     prints("�Ьd��: %ld��/�` * %d�` = %ld��",gamble_money,lotto_amount_tmp,gamble_money * lotto_amount_tmp);
     strcpy(buf2,"");
     getdata(12,0,"�нT�w�n�U�`�F(y/n)?[y]",buf2,2,1,0);
     if(buf2[0]=='n') return;
      else if(buf2[0]=='N') return;
      else break;

   }


   sprintf(buf,"%slotto/tmp/%s_t",LOTTO_PATH,cuser.userid);
   rand_tmp = fopen(buf,"w");
   fputs("�]�P�ƦC�զX�ᵲ�G:\n\n",rand_tmp);
   
   /* �ƦC�զX */
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
		   lotto_stor(lotto_num); //�Ƨ�
     		   sprintf(buf,"��%d��,%d,%d,%d,%d,%d,%d,\n",i_tmp2,
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
   getdata(22,0,"�аݭnñ�U�h�F��?[y]",buf,4,1,0);
   if(buf[0] == 'n') return 0;

   /* �g�J�ɮ� */
	/* >>�P�_�O���O�����Ĥ@��ñ��<< */
	/* ���B�w����bug:
		�b��~�ɪ�"����"�ä��|�ۦ��~��
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
   fgets(buf,79,rand_tmp);   //�Ĥ@,�G�椣�O���!

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
     sprintf(buf,"%s;%d/%d/%d;%d:%d:%d;%ld;%d;%d;%d;%d;%d;%d;�]�P�︹;\n",cuser.userid,
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

   pressanykey("���±z,�w��U���A��...");


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
		   prints("�ثe���ɶ��O �P�� %d %d:%d, �Цb�}���骺 %s��%s �H�~���ɶ���ñ��",
			   pt->tm_wday,pt->tm_hour,pt->tm_min,
			   LOTTO_OPEN_BEGIN,LOTTO_OPEN_END);
		   pressanykey("�Цb�}���ɶ����~ñ��");
		   return 1;
		}
	   }	   
	}
    }
    return 0;
}

/* �d�ߥثeñ�䪺���p */
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
	pressanykey("�ثe�|�L���");
	return 0 ;
   }	

   /* �}�ҼȦs�� */
   sprintf(mail_tmp,"%slotto/tmp/ckeck_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //��Ū���Ĥ@�檺 ��xx��
   
//   time(&now);
   sprintf(buf,"         �C��C�鴸BBS�ֳz��[1;33;44m��%s��[m��`�O���d�ߵ��G\n\n",perio);
   fputs(buf,fp);

   /* �}�lŪ��ñ��O���� */
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
       if(strcmp(lotto_num[10],"�q���︹")==0){     
         sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C���� %s���� --�q���︹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
       else{
         sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C���� %s����\n"
	    ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
	    ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }
     else if(LOTTO_GOLD_SILVER==1){
        if(strcmp(lotto_num[10],"�q���︹")==0){
          sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C�ȹ� %s���� --�q���︹\n"
             ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
             ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
        }
        else{
          sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C�ȹ� %s����\n"
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



/* �N����lotto��ñ��O���H��user */
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
	pressanykey("�ثe�|�L���");
	return 0 ;
   }	

   /* �}�ҫH��Ȧs�� */
   sprintf(mail_tmp,"%stmp/mail_record_%s.txt",LOTTO_PATH,cuser.userid);
   fp=fopen(mail_tmp,"w");

   fgets(perio,6,lotto_record);  //��Ū���Ĥ@�檺 ��xx��
   
   time(&now);
   sprintf(buf,"�@��: �C�C�Ȧ�ֳz�� \n"
               "���D:[�q��]��%s��ñ��O��\n"
               "�ɶ�: %s\n",perio,ctime(&now));
   fputs(buf,fp);
   sprintf(buf,"         �C��C�鴸BBS�ֳz��[1;33;44m��%s��[m��`�O���d�ߵ��G\n\n",perio);
   fputs(buf,fp);

   /* �}�lŪ��ñ��O���� */
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
         if(strcmp(lotto_num[10],"�q���︹")==0){  
          sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C���� %s���� --�q���︹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
         }
         else{
         sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C���� %s����\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }
     if(LOTTO_GOLD_SILVER==1){
         if(strcmp(lotto_num[10],"�q���︹")==0){
          sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C�ȹ� %s���� --�q���︹\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
         }
         else{
         sprintf(buf,"�z�b%s %s ��, ��`�F %s %s %s %s %s %s �C�C�ȹ� %s����\n"
            ,lotto_num[1],lotto_num[2],lotto_num[4],lotto_num[5],lotto_num[6]
            ,lotto_num[7],lotto_num[8],lotto_num[9],lotto_num[3]);
       }
     }


     fputs(buf,fp);
   }	

   fclose(fp);
   fclose(lotto_record);


   /* �}�l�H�^user�H�c */
   sprintf(buf,"home/%s",cuser.userid);
   stampfile(buf,&mymail);
   strcpy(mymail.owner,"�C�C�ֳz��");
   rename(mail_tmp,buf);
   sprintf(mymail.title,"[�q��] ��%s�� ñ��O��",perio);
   sprintf(buf,"home/%s/.DIR",cuser.userid);
   rec_add(buf,&mymail,sizeof(mymail));

   update_data();
  
   pressanykey("�w�g�d�ߵ��G�H�ܱz���H�c...���±z���l��...");
   
   remove(mail_tmp);
   return 0;

}

/* ��user�ۦ�]�w�O�_�n�H�U�ؤ����q�� */
define_lotto_mail(){
   
   char buf[256];
   char buf2[256];
   char buf3[256];
   FILE *fp;
   FILE *lotto_mail_define;
   int i;
   int j;
   int k;
   char lotto_define[8][2]; //�έp ,1,2,3,4,5,6��, 1=�H,0=���H
   char *p;
   char show_define[7][2][10]; //show�X��
   for(j=0;j<7;j++){
      strcpy(show_define[j][0],"���H");
      strcpy(show_define[j][1],"�H");
   }

   /* �T�{�O�_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/define_lock","r")){
      fclose(fp);
      sleep(1);
   }
   
   /*Ū��user�ثe���]�w��*/
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
   //�]�w��Ū������
   
   /* show�X���� */
   clear();
   move(0,0);
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
   prints("�˷R��%s,�z�ثe�H�o�����q�����]�w�Ȧp�U:",cuser.userid);
   i=20;
   k=4;
   move(k,i);
   prints("[%d] �ӤH�Ӵ��U�έp�ƾ� : %s",k-3,show_define[0][atoi(lotto_define[0])]);
   k++;
   move(k,i);
   prints("[%d] �Y�������q��       : %s",k-3,show_define[1][atoi(lotto_define[1])]);
   k++;
   move(k,i);
   prints("[%d] �G�������q��       : %s",k-3,show_define[2][atoi(lotto_define[2])]);
   k++;
   move(k,i);
   prints("[%d] �T�������q��       : %s",k-3,show_define[3][atoi(lotto_define[3])]);
   k++;
   move(k,i);
   prints("[%d] �|�������q��       : %s",k-3,show_define[4][atoi(lotto_define[4])]);
   k++;
   move(k,i);
   prints("[%d] ���������q��       : %s",k-3,show_define[5][atoi(lotto_define[5])]);
   k++;
   move(k,i);
   prints("[%d] ���������q��       : %s",k-3,show_define[6][atoi(lotto_define[6])]);
   k++;
   move(k,i);
   prints("[0] �^Lotto�D�ؿ�(default)");
   k++;
   k++;
   getdata(k,i,"�п�J�z�n�ק諸����:",buf,2,1,0);	

   if(strcmp(buf,"")==0 | strcmp(buf,"0")==0) break;
   if(atoi(buf)>7 | atoi(buf)<1 ) break;
   
   /* update date */
   if(strcmp(lotto_define[atoi(buf)-1],"0")==0) strcpy(lotto_define[atoi(buf)-1],"1");
   else strcpy(lotto_define[atoi(buf)-1],"0");
   /* �ثe�Ȥ���user��ܬO�_�n���n�p�ӤH�έp, �@�߳��H */
   strcpy(lotto_define[0],"1");
   
   /* �T�{�O�_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/define_lock","r")){
      fclose(fp);
      sleep(1);
   }   

   /* �[�Jlock�� */   
   sprintf(buf,"/bin/echo lock > %slotto/tmp/define_lock",LOTTO_PATH);
   system (buf);

   /* copy��ƨ�temp */
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
   
   /* �N�ץ��᪺data �[�Jtmp �ç� tmp rename��LOTTO_MAIL_DEFINE */
   sprintf(buf,"%s;%s;%s;%s;%s;%s;%s;%s;\n",cuser.userid,lotto_define[0],
           lotto_define[1],lotto_define[2],lotto_define[3],
           lotto_define[4],lotto_define[5],lotto_define[6]);
   fputs(buf,fp);
   fclose(fp);
   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_MAIL_DEFINE);
   remove(buf);
   rename(LOTTO_PATH"lotto/tmp/define_tmp",buf);

   /*����lock*/
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
char *data_menu[20];	//menu�n�Ψ쪺data...
char *data_menu2;
int i=0;	
int j=0;
int k=0;

/*
Ū�� lotto_on_off.txt
if�p�GŪ�o�� lotto_on_off.txt �B���e�� "on" ���N�O�ֳz����~��
�YŪ���� lotto_on_off.txt, ���N�O������
*/

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
   fp = fopen(buf,"r");
   if(fp == NULL) {
   pressanykey("sorry, �t�ξ��, �ثe�Ȥ��}��");
        fclose(fp);
        return 0;
   }

   strcpy(buf,"");
   fgets(buf,20,fp);
   fclose(fp);
   if(strncmp(buf,"on",2)!=0) {
      pressanykey("sorry, �t�ξ��, �ثe�Ȥ��}��....");
      return ;
   }
  
//�P�_�O�_�}�񤤵���


  while(j==0){


   /* Ū��get_lotto_now.txt����� */

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_NO_NOW_FILE);
   get_now_menu = fopen(buf,"r");
   if(get_now_menu == NULL) {
        pressanykey("�ثe�Ȥ��}��");
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
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
   prints("�ثe�w�g�}����� [1;33;40m%s[m ��",data_menu2);

   move(3,10);
   prints("�}�����X�� : [1;33;44m");
   for(i=2;i<=7;i++){
        prints ("%s ",data_menu+i); 
   }
   prints("[m   �S�O�� : [1;35;40m%s[m",data_menu+8);
   move(4,10);
   prints("�ثe�z�w�gñ�F%d��P,�̤j�i���Añ%d��",lotto_amount(cuser.userid),LOTTO_AMOUNT_MAX);   

	/* �}�loption */
	i=20;
        k=6;
	move(k,i);
	prints("[1] ���ñ�ֳz ");
	move(k+1,i);
	prints("[2] �q���۰ʿ︹ ");
	move(k+2,i);
	prints("[3] �d�ݱzñ��O�� ");
	move(k+3,i);
	prints("[4] �N�z��ñ��O���H�^�H�c");
	move(k+4,i);
	prints("[5] �]�P�U�`");
	move(k+5,i);
	prints("[6] �����q���]�w");
	move(k+6,i);
	prints("[0] ���}�C�C�ֳzñ�䯸(default)");
	strcpy(buf,"");
	getdata(k+8,i,"�п�J�z�n���檺����:",buf,2,1,0);
	
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

/* ��w�Ƨ� lotto_num[5] */
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

/*���useridñ�F�X��*/
/*
int lotto_amount(char *userid){

FILE *this_record;	//�}��lotto_record.txt
int i=0;		//ñ�F�X�K�P
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

/* �n��userñ����Ƶ��� */
lotto_amount_add(char *userid, unsigned int i_add){
FILE *fp;
FILE *lotto_amount_f;
char buf[200];
char buf2[200];
char buf3[200];
//char *p;
unsigned int i=0;

   /* Ū�����e����Ƶ��� */

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
      
    /* �T�{�O�_lock */   
   while(fp=fopen(LOTTO_PATH"lotto/tmp/l_amount_lock","r")){
      fclose(fp);
      sleep(1);
   }  
 
   /* �[�Jlock�� */   
   sprintf(buf,"/bin/echo lock > %slotto/tmp/l_amount_lock",LOTTO_PATH);
   system (buf);

   /* copy��ƨ�temp */
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
   
   /* �N�ץ��᪺data �[�Jtmp �ç� tmp rename��LOTTO_MAIL_DEFINE */
   sprintf(buf,"%s;%d;\n",userid,i);
   fputs(buf,fp);
   fclose(fp);
   sprintf(buf,"%slotto/tmp/l_amount",LOTTO_PATH);
   remove(buf);
   rename(LOTTO_PATH"lotto/tmp/l_amount_tmp",buf);

   /*����lock*/
   remove(LOTTO_PATH"lotto/tmp/l_amount_lock");

}

/* Ūuseridñ�F�X�� */
lotto_amount(char *userid){

FILE *fp;
char buf[200];
char buf2[200];
unsigned int i=0;

   /* Ū���ثe����Ƶ��� */
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
