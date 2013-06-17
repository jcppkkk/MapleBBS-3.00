/*

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
     prints("�z�ثe�@�� %d �C�C����, �w�gñ�F�C�C�ֳz%d��",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check�O�_�bñ��ɶ� */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"�����z�n�U�h�֩O��`�O? �� Enter ���}>", buf, 20, 1, 0);
     gamble_money=atol(buf);
     move(10,0);
     prints("�z��F�F�C�C����[1;33;40m%ld[m��....",gamble_money);
     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
	sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�������� %d��...",LOTTO_ANTE_MIN);
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	pressanykey("���Y...�A��U�F�t�������ƩO?..�A�U�L�@���a!");
        return;
     }
     if(!gamble_money){
        pressanykey("��~~~~���Y!!...�ФU77����, ����`���i�������R�Ͻ߷�!");
	return 0;	
     }
     if(gamble_money > cuser.goldmoney){
        pressanykey("�޳�~...�A���ӨS����h���a?...");
        return ;
     }
     if(gamble_money < cuser.goldmoney){
	break;
     }


   }
    
/* ��Jñ�@���X */
   do{
     strcpy(buf,"");
//     move(10,0);
//     prints("�p���Z:[1;35;40m ��J��@��\"c\", �ѤC�C�q�����z�︹��![m");
     getdata(9, 0,"�п�J�nñ�䪺���X(1~42),�C�Ӹ��X�H�ť���j�}:", buf, 30, 1, 0);
     
     for(i=0;i<6;i++) sprintf(lotto_num[i],"","");
     sprintf(lotto_num[0],"%s",strtok(buf," "));
     for(i=1;i<6;i++){
	    sprintf(lotto_num[i],"%s",strtok(NULL," "));
     }

     /* �q���︹ ==>���\��b�G���w�t�P*/
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
     prints("�z�ثe�@�� %d �C�C����, ��wñ�F�C�C�ֳz%d��",cuser.goldmoney,lotto_amount(cuser.userid));

     /* check�O�_�bñ��ɶ� */
     if(lotto_open_time_check()==1) return 0;

     getdata(8, 0,"�z�n�H�h�ֽ�`�ӤU�C�@��q���︹�O? �� Enter ���}>", buf, 20, 1, 0);
     gamble_money=atol(buf);
//     move(10,0);
//     prints("�C�C����[1;33;40m%ld[m��/�`....",gamble_money);
     if(strcmp("",buf)==0){
	return 0;
     }

     if(gamble_money < LOTTO_ANTE_MIN){
        sprintf(buf,"�`�N!!���ֳz���C�`���U�`�U�������� %d��...",LOTTO_ANTE_MIN);
        pressanykey(buf);
        return;
     }
     if(gamble_money<0){
	pressanykey("���Y...�A��U�F�t�������ƩO?..�A�U�L�@���a!");
        return;
     }
     if(!gamble_money){
        pressanykey("��~~~~���Y!!...�ФU77����, ����`���i�������R�Ͻ߷�!");
	return 0;	
     }
     if(gamble_money > cuser.goldmoney){
        pressanykey("�޳�~...�A���ӨS����h���a?...");
//        continue;	
	return;
     }

     strcpy(buf,"");
     move(10,0);
     prints("�Ъ`�N:[1;35;40m �z��77�����O�_�������z�U����h�`[m");
     getdata(9, 0,"�п�J�z�Q�n���q���H�W�������B���z�R�X�Ӹ��X�O?:", buf, 6, 1, 0);

     if((atoi(buf)+lotto_amount(cuser.userid)) > (LOTTO_AMOUNT_MAX)){
	pressanykey("��~...����ñ����h��!...�d�I�P���ȵ��O�H��!..�U���Ц�~~");
	 return;
     }
     if(atoi(buf) ==0) return;
     if(atoi(buf) < 0) return;
     
     lotto_amount_tmp=atoi(buf);
     if((gamble_money * lotto_amount_tmp) > cuser.goldmoney){
	pressanykey("�޳�~...�A���ӨS����h���a ?...");
//	continue;
	return;
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
    
   i_tmp2=0;
   while(lotto_amount_tmp > i_tmp2){
     i_tmp2++;

     /* �q���︹ */

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

   degold(gamble_money*lotto_amount_tmp);

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

menu_lotto(){

FILE *get_now_menu;
char data_read_menu[79];
//char data_now_menu[10][25];
char buf[200];
char *p_menu;
char *data_menu[20];	//menu�n�Ψ쪺data...
char *data_menu2;
int i=0;	
int j=0;
int k=0;

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
   prints("[1;33;44m                           >>>>>�C�C�ֳz�mñ�䯸<<<<<                         [m\n");
   move(2,10);
//   prints("�ثe�w�g�}����� [1;33;40m%s[m ��, �� %s �}���F!!",data_now_menu[0],data_now_menu[1]);
   prints("�ثe�w�g�}����� [1;33;40m%s[m ��",data_menu2);

   move(3,10);
   prints("�}�����X�� : [1;33;44m");
   for(i=2;i<=7;i++){
//        prints ("%s ",data_now_menu[i]);
        prints ("%s ",data_menu+i); 
   }
//   prints("[m   �S�O�� : [1;35;40m%s[m",data_now_menu[8]);
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
	prints("[0] ���}�C�C�ֳzñ�䯸(default)");
	strcpy(buf,"");
	getdata(k+6,i,"�п�J�z�n���檺����:",buf,2,1,0);
	
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