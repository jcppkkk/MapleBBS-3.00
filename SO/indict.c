/*===============================*/
/*==       �j�r��  Ptt         ==*/
/*===============================*/
#include "bbs.h"
#define REFER "etc/dicts"

char dict[41],database[41];

int addword(char word[])
{
 char buf[150],temp[150],a[3];
 FILE *fp = fopen(database,"r+");
 fgets(buf,130,fp);
 fseek(fp,0,2);
 if(HAVE_PERM(PERM_LOGINOK))
 {
     clear();
     move(4,0);
     outs(" [31mĵ�i[m:�Y�W�N��g����ƱN[36m��id[m�B��\n");
     sprintf(temp,"\n��J�d�Q\n:[33m%s[m",buf);
     outs(temp);
     outs("\n�Ш̤W�C�d�ҿ�J�@����(����enter���)\n");
      getdata(10, 0,":",buf,65,DOECHO,0);
      if(buf[0])
       {
        getdata(13,0,"�T�w�s�W?(Y/n)",a,2,LCECHO,"Y");
        if(a[0]!='n')
           fprintf(fp,"%-65s[%s]\n",buf,cuser.userid);
       }
 }
 fclose(fp);
 clear();
}

int choose_dict()
{
  int c;
  FILE *fp;
  char buf[10][21],data[10][21],cho[130];
  move(12,0);
  clrtobot();
  outs("                        �� [1;42;33m�r��� �� �n�d���@���H[m ��");

  if (fp = fopen(REFER, "r"))
  {
     for (c=0;fscanf(fp,"%s %s",buf[c],data[c])!=EOF ; c++ )
          {
            sprintf(cho,"\n                     ([36m%d[m) %-20s�j�r��",c+1,buf[c]);
            outs(cho);
          }

     getdata(22 , 14, "          �� �п�ܡA[Enter]���}�G", cho, 3, LCECHO,0);
     cho[0]-='1';
     if(cho[1])
        cho[0]=(cho[0]+1)*10+(cho[1]-'1');

     if( cho[0] >= 0 && cho[0] < c)
      {
        strcpy(dict,buf[cho[0]]);
        strcpy(database,data[cho[0]]);
        return 1;
      }
     else
      {
         return 0;
      }
  }
}
char *
lower(char str[])
{
 int c;
 static char temp[200];
 strcpy(temp,str);
 for (c=0;temp[c]!=0;c++)
  {
    if(temp[c]>='A' && temp[c]<='Z') temp[c]+= 'a'-'A';
  }
 return temp;
}
int x_dict()
{
 if(choose_dict())
	use_dict();
}
int
use_dict()
{
 FILE *fp;
 char lang[150],word[80]="",j,f,buf[120],sys[]="|[31me[m:�s�y�r��";
 int i;
 setutmpmode(DICT);

 if(!HAS_PERM(PERM_SYSOP)) sys[0]=0;

  clear();

sprintf(buf,"[45m                           ��[1;44;33m  %-14s[3;45m ��                              ",dict);
strcpy(&buf[100],"[m\n");
  for(;1;)
  {
move(0,0);
sprintf(lang,"  �п�J����r��(%s) �Ϋ��O(h,t,a)\n",dict);
outs(lang);
sprintf(lang,"[[32m<����r>[m|[32mh[m:help|[32mt[m:�Ҧ����|[32ma[m:�s�W���%s]\n:",sys);
outs(lang);
   getdata(2, 0,":",word,18, DOECHO,0);
outs("��Ʒj�M���еy��....");
   strcpy(word,lower(word));
   if(word[0]==0) return 0;
   clear();
   move(4,0);
   outs(buf);
   if(strlen(word)==1)
     {
      if (word[0]=='a')
          {clear();move(4,0);outs(buf);addword(word);continue;}
      else if(word[0]=='t') word[0]=0;
      else if(word[0]=='h') {more("etc/help/dict.hlp",YEA);clear();continue;}
      else if(word[0]=='e') {vedit(database,NA);clear();continue;}
      else
        {outs("�r��ӵu,�п�J�h�@�I����r");continue;}
     }

   if(fp = fopen(database,"r"))
     {
      i =0;
      while (fgets(lang,150,fp) != NULL)
      {
        if(lang[65]=='[')
                {lang[65]=0;f=1;}
         else
                f=0;
        if(strstr(lower(lang),word))
          {
                if(f==1) lang[65]='[';
                outs(lang);
                i++;
                if(!((i+1)%17))
                 {
                  move(23,0);
                  outs("[45m                               ���N���~��  Q:���}                             [m ");
                  j=igetch();
                        if(j=='q')
                        {
                           break;
                        }
                        else
                        {
                        clear();  move(4,0);outs(buf);
                        }
                 }
            }
        }
     }
      fclose(fp);
    if (i==0)
     {
     getdata(5, 0, "�S�o�Ӹ�ƭC,�s�W��?(y/N)",lang,3,LCECHO,0);
      if(lang[0]=='y')
        {
         clear();move(4,0);outs(buf);addword(word);
        }
     }
  }
}
