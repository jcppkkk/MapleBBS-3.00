/********************************/
/*   �ƨg��L (��W:�U�`�C��)   */
/********************************/

//��c�Q�ε{���� dsyan ���Ѥμ��g..
//��g weiren ��g�����[�c�o���{��..
//��������D�Ϋ�ĳ�w��P�o���s��.. :)

// dsyan �O�Ѫ��a�[(Forever.twbbs.org) ������
// =>dsyan.bbs@Forever.twbbs.org
// weiren �O���դƹ� (weird.twbbs.org.tw) ������
// =>http://www.weiren.net/
// =>weiren@weiren.net   
 
#include "bbs.h"
#define maxboard 40
#define newgame 5 /* �̫�}���X�L�n�ܦ� */
#define LOCK_FILE BBSHOME"/game/bet.lock"

time_t
get_the_time()
{
  char buf[10],ch;
  char *t_max = "0919392959";
  time_t sch_time;
  struct tm ptime;
  int i;

  outs("�п�J�n���`���ɶ�(�Х�24�p�ɨ�):  �~  ��  ��  ��  ��");

  for(i=0;i<10;i++){
     move(3,(33+i/2*4+i%2));
     ch = igetch();
     if(ch == 'q' || ch == 'Q') return 0;
     else if ( ch == '\r') { i = -1; continue;}
     else if ( ch == '\177' || ch == Ctrl('H'))
     {
      if(i)i -= 2;
      else i = -1;
      continue;
     }                     /*���\�˰h���ENTER���s��J*/
     if(ch >= '0' && ch <= t_max[i]){
       outc(ch);
       buf[i] = ch - '0';
     }
     else{ bell(); --i;}
  }
//  ptime.tm_year = 100;ptime.tm_sec=0;
  ptime.tm_year = buf[0]*10+buf[1] + 100;
  ptime.tm_mon  = buf[2]*10+buf[3] - 1;
  ptime.tm_mday = buf[4]*10+buf[5];
  ptime.tm_hour = buf[6]*10+buf[7] + 1;
  ptime.tm_min  = buf[8]*10+buf[9];
  ptime.tm_sec  = 0;
  sch_time      = mktime (&ptime);
  return sch_time;
}

int p_bet()
{
 char i,j,k,num,cont,cc[51], dd[13], genbuf[60],genbuf2[30], cho[9][51];
 int t[maxboard],money,tmp,phh,a,b,newbet[newgame];
 int turn=0,ch_color;
 char ch;
 time_t set_time, close_time;
 FILE *fs,*fss;

 more(BBSHOME"/game/bet.welcome",YEA);
 sprintf(genbuf2,BBSHOME"/game/bet.new");
 if(fss=fopen(genbuf2,"r")){
   for(j=0;j<newgame;j++){
      fscanf(fss,"%d\n",&newbet[j]);
   }
   fclose(fss);
 }

 while(-1)
 {
   setutmpmode(BET);
   log_usies("BET",NULL);
   showtitle("�ƨg��L", BoardName);

   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }

do{
   move(1,0);
   prints("[44;1m�s��  ��  ��           ��    ��    ��    �z            �} �� ��   �|���X�p��  [m\n");
   fs=fopen(BBSHOME"/game/bet.list","r");
   if(turn==0)tmp=0;
   else{
     tmp=20;
     for(k=0;k<20;k++){
       fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
       fgets(genbuf,60,fs);
     }
   }
   for(i=tmp;i<20+tmp;i++)
   {
     fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
     fgets(genbuf,60,fs);
     j=strlen(genbuf)-1;
     genbuf[j]=0;
     a=(close_time-time(0))/3600/24;
     b=(close_time-time(0))/3600%24;

     ch_color=0;
     for(j=0;j<newgame;j++){
       if(newbet[j]==i+1)ch_color=1;
     }

     sprintf(genbuf2,BBSHOME"/game/bet.ans%d",i+1);
     if(t[i]){
       if(fss=fopen(genbuf2,"r")){
         fclose(fss);
         prints("[35;1m %2d    %5d  %-40s %-12s [���ݵo��][m\n"
                ,i+1,t[i],genbuf,dd);
       }
       else if(close_time-time(0)>0)
         prints("[%d;1m %2d    %5d  %-40s %-12s %2d��%2d�p��[m\n"
                ,ch_color?37:32,i+1,t[i],genbuf,dd,a,b);
       else
         prints("[32;1m %2d    %5d  %-40s %-12s     [�ʽL][m\n"
                ,i+1,t[i],genbuf,dd);
     }
     else
       prints("[31;1m %2d    -----      �|�����H�}��[m\n",i+1);
   }
   fclose(fs);
   move(22,0);clrtoeol();
   prints("[1;44;37m���仡��: [n]�U�@�� [p]�W�@�� [b]�U�`/�}��/�}�L [q]���}    ��: %10d    [m",cuser.silvermoney);
   redoscr();
   do{ch=igetch();}while(ch!='q'&&ch!='n'&&ch!='p'&&ch!='b');
   if(ch=='q')return; if(ch=='n')turn=1; if(ch=='p')turn=0;
   j=0;
   if(ch=='b'){
     getdata(22, 0,"�п�J 1-40 �Ϊ����� Enter ���}�G", cc, 3, 1, 0);
     j=atoi(cc);
   }
}while(!j||j>maxboard||j<1);

   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }

   if(t[j-1]) /* �w���H�}�� */
   {
     fs=fopen(BBSHOME"/game/bet.list","r");
     for(i=0;i<j;i++)
     {
       fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
       fgets(genbuf,60,fs);
     }
     fclose(fs);
     if(close_time<time(0))  /* �ɶ��� */
     {
       sprintf(genbuf,BBSHOME"/game/bet.ans%d",j);
       if(fs=fopen(genbuf,"r"))
       {
        fclose(fs);
        pressanykey("���G�w���G..�����N����� 6:30am �o��..:)");
       }
       else if(strcmp(dd,cuser.userid))
        pressanykey("�U�`�����w��..���� %s �}��..",dd);
       else
       {
           int ch[9],all=0;

           clear();
           showtitle("�ƨg��L", BoardName);
           prints("\n\n[44;1m  �m����    ���v      ��  �z           [m\n\n");
           sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
           fs=fopen(genbuf,"r");
           fscanf(fs,"%d\n%d\n",&tmp,&all);
           for(i=0;i<tmp;i++)
           {
             long xx;
             fgets(cho[i],250,fs);
             fscanf(fs,"%d\n",&ch[i]);
             sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,i+1);
             xx=all*100/ch[i];
             prints("%d)%6d   %3d.%-2d   %s",i+1,ch[i],xx/100,xx%100,cho[i]);
           }
           fclose(fs);
           sprintf(genbuf,"�п�J�̫᪺���G(1-%d)�Ϊ����� Enter���}�G",tmp);

           getdata(20,0,genbuf,cc,3,DOECHO,0);
           num=atoi(cc);

   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
           if(num<=tmp && num>=1)
           {
             getdata(21,0,"�T�w��?? (y/N)",cc,3,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
             if (*cc=='y')
             {
               sprintf(genbuf,BBSHOME"/game/bet.ans%d",j);
               fss=fopen(genbuf,"w");
               fprintf(fss,"%d",num);
               fclose(fss);
             }
           }
       }
     }
     else
     {
       sprintf(genbuf,BBSHOME"/game/bet.scr%d",j);
       more(genbuf,YEA);
       while(1)
       {
         int ch[9],all=0,hhall=0;

         clear();
         showtitle("�ƨg��L", BoardName);
         prints("\n\n[44;1m  �m����       ���v      ��  �z           [m\n\n");
         sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
         fs=fopen(genbuf,"r");
         fscanf(fs,"%d\n%d\n",&tmp,&all);
         for(i=0;i<tmp;i++)
         {
           int hh=0;
           long xx;
           fgets(cho[i],250,fs);
           fscanf(fs,"%d\n",&ch[i]);
           sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,i+1);
           if(fss=fopen(genbuf,"r"))
           {
             char id[13];
             phh=0;
             while( fscanf(fss,"%s %d\n",id,&hh) != EOF ){
               if(!strcmp(id,cuser.userid))
               {
                 phh+=hh;
                 hhall+=hh;
               }
             }
             fclose(fss);
           }
           else hh=0;
           xx=all*100/ch[i];
           prints("%2d)%4d/%4d   %3d.%-2d   %s",i+1,phh,ch[i],xx/100,xx%100,cho[i]);
         }
         fclose(fs);
         prints("\n��>%4d/%4d\n",hhall,all);
         prints("\n�����`���: %d ��i�䲼����: %d",all*t[j-1],t[j-1]);

         sprintf(genbuf,"�п�J�A�����(1-%d)�Ϋ� q ���X�G",tmp);
         getdata(20,0,genbuf,cc,3,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
         if(cc[0]=='q') break;
         num=cc[0]-48;
         if(num>tmp||num<1)
           pressanykey("��J�d�򦳻~!!�Э��s��J..");
         else
         {
           sprintf(genbuf,"�@�i�m�� %d ���A�A�ثe�� %d�A�n�R�X�i�H(0-%d)",
             t[j-1],cuser.silvermoney,cuser.silvermoney/t[j-1]);
           getdata(21,0,genbuf,cc,6,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
           if(atoi(cc)>cuser.silvermoney/t[j-1] || !atoi(cc))
             pressanykey("��J�d�򦳻~!!����..");
           else
           {
             int hh=0;
             if(close_time<time(0)){pressanykey("�W�L�ɶ��F!");return;}
             demoney(atoi(cc)*t[j-1]);
             sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
             fs=fopen(genbuf,"r");
             fscanf(fs,"%d\n%d\n",&tmp,&all);
             for(i=0;i<tmp;i++){
               fgets(cho[i],250,fs);
               fscanf(fs,"%d\n",&ch[i]); 
             }
             fclose(fs); 
             ch[num-1]+=atoi(cc);
             all+=atoi(cc);
             fs=fopen(genbuf,"w");
             fprintf(fs,"%d\n%d\n",tmp,all);
             for(i=0;i<tmp;i++)
             {
               char g=strlen(cho[i])-1;
               cho[i][g]=0;
               fprintf(fs,"%s\n%d\n",cho[i],ch[i]);
             }
             fclose(fs);
             sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,num);
             fs=fopen(genbuf,"a+");
             fprintf(fs,"%s %d\n",cuser.userid,atoi(cc));
             fclose(fs);

             sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,num);
             if(fs=fopen(genbuf,"r"))
             {
               fscanf(fs,"%d\n",&hh);
               hh+=atoi(cc);
               fclose(fs);
             }
             else hh=atoi(cc);

             fs=fopen(genbuf,"w");
             fprintf(fs,"%d\n",hh);
             fclose(fs);
           }
         }
       }
     }
   }
   else /* �ۤv�}�� */
   {
     cont=1;
     more(BBSHOME"/game/bet.anno",YEA);
     getdata(22,0,"�T�w�n�}���H(y/N)",cc,3,1,0);
   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
     if(cc[0]!='y' && cc[0]!='Y') cont=0;

     if(cont)
     {
       sprintf(cc,"home/%s/bet.scr",cuser.userid);
       pressanykey("�Ы����@��}�l�s�覹�� [�}���v��]");
       if(vedit(cc,NA,0)==-2) cont=0;
     }

     if(cont)
     {
       move(3,0);set_time=get_the_time();
       if(!set_time) cont=0;
       if(set_time < time(0) + 36000 ){pressanykey("�ɶ��L�u(�̤� 10 hr)");cont=0;}
       if(set_time > time(0) + (86400*14)){pressanykey("�ɶ��L��(�̪� 14 ��)");cont=0;}
//       if(set_time>time(0)+86400&&j<=13){pressanykey("1-13 ��ɶ��̦h 24 hr");cont=0;}
     }

     if(cont)
       if(!getdata(4,0,"��²�u�ԭz�����G",cc,41,1,0)) cont=0;

     if(cont)
     {
       getdata(5,0,"�@���X�ӿﶵ�H(2-9)",genbuf,3,DOECHO,0);
       tmp=atoi(genbuf);
       if(tmp<1 || tmp>9)
       {
         pressanykey("�ﶵ�ƥؿ��~!!");
         cont=0;
       }
     }

     if(cont)
     {
       getdata(6,0,"�m��@�i�h�ֿ��H(100-10000)", genbuf, 6, DOECHO, 0);
       money=atoi(genbuf);
       cont=0;
       if(money<100 || money>10000)
         pressanykey("��J���B���~!!");
       else if(cuser.silvermoney<money*(10*tmp+5))
         pressanykey("�A�������� %d ����!!",money*(10*tmp+5));
       else
        cont=1;
     }

     if(cont)
     {
       for(i=0;i<tmp;i++)
       {
         move(7+i,0); prints("�ﶵ%d�G",i+1);
         do{getdata(7+i,7,"",cho[i],50,DOECHO,0);}while(!cho[i][0]);
       }
       sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
   if(dashf(LOCK_FILE)){
     pressanykey("�o���ɶ�����U�`");
     return;
   }
       if(dashf(genbuf)){
         fclose(fs);pressanykey("���H��A�֤@�B�F");return;
       }

       fs=fopen(genbuf,"w");
       fprintf(fs,"%d\n%d\n",tmp,tmp*10);
       for(i=0;i<tmp;i++)
       {
         fprintf(fs,"%s\n10\n",cho[i]);
         sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,i+1);
         fss=fopen(genbuf,"a+");
         fprintf(fss,"%s %d\n",cuser.userid,10);
         fclose(fss);

         sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,i+1);
         fss=fopen(genbuf,"w");
         fprintf(fss,"10\n");
         fclose(fss);
       }
       fclose(fs);

       sprintf(genbuf2,BBSHOME"/game/bet.new");
       if(fss=fopen(genbuf2,"r")){
         for(i=0;i<newgame;i++){
            fscanf(fss,"%d\n",&newbet[i]);
         }
         fclose(fss);
       }
       if(fss=fopen(genbuf2,"w")){
         fprintf(fss,"%d\n",j);
         for(i=0;i<newgame-1;i++){
            fprintf(fss,"%d\n",newbet[i]);
         }
         fclose(fss);
       }

       demoney(money*(10*tmp+5));
       time(&close_time);
       close_time = set_time;
       while(fs=fopen(BBSHOME"/game/bet.list.w","r"))
       {
         fclose(fs);
       }
       fs=fopen(BBSHOME"/game/bet.list","r");
       fss=fopen(BBSHOME"/game/bet.list.w","w");
       for(i=0;i<maxboard;i++)
       {
         if(i+1==j)
         {
           fgets(genbuf,60,fs);
           fgets(genbuf,60,fs);
           fprintf(fss,"%d %ld %s\n",money,close_time,cuser.userid);
           fprintf(fss,"%s\n",cc);
         }
         else
         {
           fgets(genbuf,60,fs);
           fprintf(fss,genbuf);
           fgets(genbuf,60,fs);
           fprintf(fss,genbuf);
         }
       }
       fclose(fs);
       fclose(fss);
       system("mv -f game/bet.list.w game/bet.list");
       sprintf(genbuf,"mv -f home/%s/bet.scr game/bet.scr%d",cuser.userid,j);
       system(genbuf);
       pressanykey("�}�৹��!!");
     }
     else
     {
      sprintf(cc,"rm -f game/bet.scr%d",j);
      system(cc);
      pressanykey("�}�����!!");
     }
   }
 }
}

