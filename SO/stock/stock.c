	/***************************/
	/* �Ѳ��o by dsyan 87.6.21 */
	/***************************/

	#include "bbs.h"
	#define stosum 999

	struct st
	{
	 char name[stosum][12];
	 char ud[21];
	 long price[stosum];
	 int max_page;
	 int max;
	};

	struct st *sto;


	int
	p_stock()
	{
	 char tmp[200],tp2[200];
	 int own[stosum],page=0,tt=0,t1,t2;
	 double td,pric,tax;
	 FILE *ff,*fff;
	 time_t now;
	 struct tm *ptime;
	 more("game/stock_wel",YEA);
	 if(sto == NULL)  
	   sto = shm_new(5000,sizeof(*sto));
	// more(BBSHOME"/game/stock_wel",YEA);
	 showtitle("��ù�ѥ�", BoardName);
	 setutmpmode(STOCK);

	 if (count_multi() > 1)
	 {
	   pressanykey("�z���ଣ�������i�J�ѥ��� !");
	   return 1;
	 }

	 sprintf(tmp,BBSHOME"/home/%s/stock",cuser.userid);

	 if(ff=fopen(tmp,"r"))
	  for(tt=0;tt<stosum;tt++) fscanf(ff,"%d\n",&own[tt]);
	 else
	 {
	  ff=fopen(tmp,"w");
	  for(tt=0;tt<stosum;tt++) 
	  {
	   fprintf(ff,"%d\n",0);
	   own[tt]=0;
	  }
	 }
	 fclose(ff);

	 /* Ū����� */
	if(ff=fopen(BBSHOME"/game/stock_now","r"))
	{
	 long tmpp;
	 char tmpn[12];
	 tt=0; t1=0;
	 fgets(tmp,150,ff);
	 tmp[20]=0;
	 if(strcmp(tmp,sto->ud))
	 {
	  strcpy(sto->ud,tmp);
	  if(fff=fopen(BBSHOME"/game/stock_name","r"))
	  {
	   while(fgets(sto->name[t1],12,fff)) 
	   {
	    fgets(tmp,12,fff);
	    move(8,8); prints("%d <%s>",t1,sto->name[t1]);
	    t1++;
	   }
	   fclose(fff);
	  }
	  game_log(STOCK,"[41m��s��� [42m%s",sto->ud);
	  fff=fopen(BBSHOME"/game/stock_name","a+");
	  while(fgets(tmp,200,ff))
	  {
	   if(tmp[0]!='|'||tmp[1]<'0'||tmp[1]>'9') continue;
	   tmp[12]=0; tmp[53]=0; tt++;
	   
	   tmpp=atof(tmp+17);
	   move(10,10);
	   prints("tmpp=%d ",tmpp);
	  
	   strcpy(tmpn,tmp+1);
	   move(11,11);
	   prints("tmpn=<%s> ",tmpn);
	  
	   for(t2=0;t2<t1;t2++) 
	    if(!strcmp(tmpn,sto->name[t2])) break;
	   if(t2==t1)
	   {
	    strcpy(sto->name[t2],tmpn);
	    fprintf(fff,"%s\n",tmpn);
	    t1++;
	   }
	   sto->price[t2]=tmpp;
	  }
	  fclose(fff);
	  fclose(ff);
	  sto->max=tt;
	  sto->max_page=tt/40;
	  if(sto->max_page%40) sto->max_page++;
	 }
	} 
	 while(-1)
	 {  
	  int i,r;
	/* �e���D�{�� */
	 move(1,0); clrtobot();
	 prints("[31;46;1m �s�� [42m �� �� �W �� [43m �� ��"
		" [45m �����i�� [46;1m     �s�� [42m �� �� �W ��"
		" [43m �� �� [45m �����i�� [m");
	 for(i=0;i<20;i++)
	 {
	  move(i+2,0);
	  
	  r=page*40+i;
	  if(r+1<=sto->max) 
	   prints(" %3d)  %11s%5d.%-2d %5d         ",r+1,sto->name[r], 
	     sto->price[r]/100,sto->price[r]%100,own[r]);
	     
	  r=page*40+i+20;
	  if(r+1<=sto->max) 
	   prints( "%3d)  %11s%5d.%-2d %5d\n",r+1,sto->name[r],
	     sto->price[r]/100,sto->price[r]%100,own[r]);
	 }
	 move(22,0);
	 prints("[44mn:�U�� p:�W�� b:�R s:�� v:�� e:�� q:���X ��:%-8d %2d/%2d %19s[m",
	   cuser.silvermoney,page+1,sto->max_page,sto->ud);

	  tt=igetch();
	  switch(tt)
	  {
	  case 'b':
	    now=time(0);
	    ptime = localtime(&now);
	    if(ptime->tm_hour>=3&&ptime->tm_hour<=18)
	    {
	      pressanykey("����ɶ����C�� 7:00pm ��j�Ѧ��W 4:00am .. :)");
	      break;
	    }
	    move(23,0); prints("�n�R���a??(1-%d) ",sto->max);
	    getdata(23,19, " ",tmp, 4, 1, 0); 
	    tt=atoi(tmp);
	    if(tt<1||tt>sto->max) break;
	    t2=cuser.silvermoney/(sto->price[tt-1]*10*1.003);
	    move(23,0); prints("�n�R�X�i??(0-%d) ",t2);
	    getdata(23,19, " ",tmp, 4, 1, 0); 
	    t1=atoi(tmp);
	    if(t1>t2||t1<1) break;
	    {
	      unsigned long int money=t1*sto->price[tt-1]*10*1.001425;
	      if((unsigned long int) cuser.silvermoney <= 
		 (unsigned long int) money) cuser.silvermoney=0;

	      else cuser.silvermoney -= money;
	      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
	      own[tt-1]+=t1;
	    }

	    td=t1*sto->price[tt-1]*10*0.001425;
	    game_log(STOCK,"[32m�R�F [44m%s [m[43m%d �i.. [31;1m%d", 
	      sto->name[tt-1],t1,(int)td);

	    sprintf(tp2,BBSHOME"/home/%s/stock_log",cuser.userid);
	    fff=fopen(tp2,"a+");
	//    td=t1*pric*0.07;
	    fprintf(fff,"%s �H %5.2f ������R�F %s %d �i ����O %d\n", 
	      Cdate(&now), (float) sto->price[tt-1]/100,sto->name[tt-1],t1,(int)td);
	    fclose(fff);

	    sprintf(tmp,BBSHOME"/home/%s/stock",cuser.userid);
	    ff=fopen(tmp,"w");
	    for(tt=0;tt<stosum;tt++) fprintf(ff,"%d\n",own[tt]);
	    fclose(ff);
	    
	    break;
	    
	  case 's':
	    now=time(0);
	    ptime = localtime(&now);
	    if(ptime->tm_hour>=3&&ptime->tm_hour<=18)
	    {
	      pressanykey("����ɶ����C�� 7:00pm ��j�Ѧ��W 4:00am .. :)");
      break;
    }
    move(23,0); prints("�n�樺�a??(1-%d) ",sto->max);
    getdata(23,19, " ",tmp, 4, 1, 0); 
    tt=atoi(tmp);
    if(tt<1||tt>sto->max||!own[tt-1]) break;
    pric=sto->price[tt-1]*10;
    move(23,0); prints("�n��X�i??(0-%d) ",own[tt-1]);
    getdata(23,19, " ",tmp, 4, 1, 0); 
    t1=atoi(tmp);
    if(t1>own[tt-1]||t1<=0) break;
    {
      unsigned long int money = t1*pric*0.995575;
      cuser.silvermoney += money;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
    }
    own[tt-1]-=t1;
    
    now=time(0);
    td=t1*pric*0.004425;   
    game_log(STOCK,"[31m��F [44m%s [m[43m%d �i.. [31;1m%d", 
      sto->name[tt-1],t1,(int)td);
    td=t1*pric*0.001425;
    tax=t1*pric*0.003;
    sprintf(tp2,BBSHOME"/home/%s/stock_log",cuser.userid);
    fff=fopen(tp2,"a+");
    fprintf(fff,"%s �H %5.2f �������F %s %d �i ����O %d �| %d\n", 
      Cdate(&now),(float) sto->price[tt-1]/100,sto->name[tt-1],t1,(int)td,(int)tax);
    fclose(fff);

    sprintf(tmp,BBSHOME"/home/%s/stock",cuser.userid);
    ff=fopen(tmp,"w");
    for(tt=0;tt<stosum;tt++) fprintf(ff,"%d\n",own[tt]);
    fclose(ff);
    
    break;

  case 'v':
   sprintf(tp2,BBSHOME"/home/%s/stock_log",cuser.userid);
   more(tp2,YEA);
   showtitle("�ѥ��j��", BoardName);
   break;

  case 'e':
    sprintf(tp2,BBSHOME"/home/%s/stock_log",cuser.userid);
    vedit(tp2, NA,0);
    showtitle("�ѥ��j��", BoardName);
    break;

  case 'p':
    if(page) page--;
    break;
  
  case 'n':
    if(page+1<sto->max_page) page++;
    break;
  
  case 'q':
    sprintf(tmp,BBSHOME"/home/%s/stock",cuser.userid);
    ff=fopen(tmp,"w");
    for(tt=0;tt<stosum;tt++) fprintf(ff,"%d\n",own[tt]);
    fclose(ff);
    return 1;
  }
 }
}