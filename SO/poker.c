/**********************************/
/*  ���s�����C�� by dsyan 8/4 98` */
/**********************************/

#include "bbs.h"

int tx(x){ return x*11-8; } /* ��y�� */
int da1(char aa) { return aa/13-(aa%13?0:1); } /* ���� */
int da2(char aa) /* ���I�� */
{
 aa%=13;
 return aa?aa:13;
}

cur(a,b,c,d) /* ���ʽb�� */
{
 move(a,b); prints("  ");
 move(c,d); prints("��");
 refresh();
}

pa (char x, char y, char card) /* �e�P */
{
  char *fl[4]={"��","��","��","��"};
  char *nu[13]={"��","��","��","��","��","��","��",
                    "��","��","��","��","��","��"};
  move(y,x);
  if (card>0)
    prints("�u%s%s�t",fl[da1(card)],nu[da2(card)-1]);
  else if(!card) 
    prints("�u�w�w�t");
  else 
    prints("        ");
  refresh();
}

int
p_dragon()
{
 char dd[8][21],oo[5],ll[25],pp[53],max[9],rmax[8];
 char mode,buffer,bufferx,buffery;
 char j,m,p,x,y,xx,yy;
 int i,k;
 srandom(time(0));

start:
 clear();
 showtitle("���֤k���s", BoardName);
 setutmpmode(DRAGON);
 log_usies("DRAGON",NULL);
 mode=buffer=0; bufferx=79; buffery=2;

 for(i=0;i<=4;i++) oo[i]=0;
 for(i=1;i<=52;i++) pp[i] = i;  /* �~�P */
 for(i=0;i<32000;i++) 
 {
  x=rand()%52+1; y=rand()%52+1;
  j=pp[x]; pp[x]=pp[y]; pp[y]=j;
 }
 max[8]=24; m=0;
 for(i=1;i<=7;i++)
 {
  max[i] = i; rmax[i] = i - 1;
  for(j=1;j<=i;j++)
  {
   dd[i][j]=pp[++m];
   if(i!=j) p=0; else p=dd[i][j];
   pa(tx(i),j+2,p);
  }
 }
 for(i=1;i<=24;i++) ll[i]=pp[++m];
 pa(1, 1, 0);

 m = 0; x = 1; y = 1; xx = 1; yy = 1;
 for(;;)
 {
  if(!mode) /* �b�U�� */
  {
    cur(yy+2,tx(xx)-2,y+2,tx(x)-2);
    xx = x; yy = y;
    k = igetkey();
    switch(k)
    {
      case 'q': return;
      case 'r': goto start;
     case KEY_LEFT:
       x--; if(!x) x=7;
       if(y>max[x]+1) y = max[x]+1;
       break;
     case KEY_RIGHT:
       x++; if(x==8) x=1;
       if(y>max[x]+1) y = max[x]+1;
       break;
     case KEY_DOWN:
       y++;
       if (y == max[x] + 2) y--;
       break;
     case KEY_UP:
       y--;
       if (!y) /* �]��W���h�F */
       {
	mode = 1;
	cur(yy+2,tx(xx)-2,1,9);
       }
       break;
      case 13: /* ���P��k�W�� */
       j=dd[x][y];
       if((da2(j)==oo[da1(j)]+1)&&y==max[x]&&y>rmax[x])
       {
	oo[da1(j)]++; max[x]--;
	pa(da1(j)*10+40,1,j);
	pa(tx(x),y+2,-1);
        if(buffer==j) /* �p�G���O���N���� */
        {
         move(buffery,bufferx); 
         prints(" ");
        }
       }
       break;
      case 32:
       if(y==max[x]&&y==rmax[x]) /* ½�s�P */
       {
	rmax[x]--;
	pa(tx(x),y+2,dd[x][y]);
	break;
       }
       else if(y>rmax[x] && y<=max[x]) /* �ŤU */
       {
	move(buffery,bufferx); prints(" ");
	buffer = dd[x][y]; bufferx=x*11; buffery=y+2;
	move(buffery,bufferx); prints("*");
	break;
       }
       else if (y!=max[x]+1) break; /* �K�W */

       if((max[x]&&(da2(dd[x][max[x]])==da2(buffer)+1)&&(da1(dd[x][max[x]])+da1(buffer))%2) 
        ||(max[x]==0 && da2(buffer)==13) )       
            
	 if(buffery==1) /* �q�W���K�U�Ӫ� */
	 {
	  max[x]++; max[8]--; buffery=2; m--;
	  for(k=m+1;k<=max[8];k++) ll[k]=ll[k+1];
	  dd[x][max[x]]=buffer;
	  pa(tx(x),max[x]+2,buffer);
	  move(1,19); prints(" ");
	  if(m) pa(11, 1, ll[m]); else pa(11, 1, -1);
	 }
	 else if(buffery>2) /* �b�U���K�ӶK�h�� */
	 {
	  char tpx=bufferx/11,tpxx=max[tpx];
	  for(j=buffery-2;j<=tpxx;j++)
	  {
	   max[x]++; max[tpx]--;
	   dd[x][max[x]]=dd[tpx][j];
	   pa(tx(x),max[x]+2,dd[x][max[x]]);
	   pa(tx(tpx), j+2, -1);
	  }
	  move(buffery,bufferx); prints(" "); buffery=2;
	 }
       break;
     }
    
  }
  else /* �b�W�� */
  {
    if(!m) pa(11, 1, -1); else pa(11, 1, ll[m]);
    k = igetkey();
    switch(k)
    {
      case 'q': return;
      case 'r': goto start;
      case 13:
       j=ll[m];
       if(da2(j)==oo[da1(j)]+1)
       {
	oo[da1(j)]++; max[8]--; m--;
	pa(da1(j)*10+40,1,j);
	for(k=m+1;k<=max[8];k++) ll[k]=ll[k+1];
	if(!m) pa(11, 1, -1); else pa(11, 1, ll[m]);
        if(buffery == 1) /* �p�G���O���N�M�� */
        {
         buffery = 2;
         move(1,19); prints(" ");
        }
       }
       break;
     case KEY_DOWN:
       mode = 0; y=1;
       cur(1,9,y+2,tx(x)-2);
       break;
     case KEY_UP:
      if(m==max[8]) m=0; else m+=3;
      if(m>max[8]) m=max[8];
      if(buffery == 1)
      {
       buffery = 2;
       move(1,19); prints(" ");
      }
      if(m==max[8]) pa(1, 1, -1); else pa(1, 1, 0);
      break;
     case 32:
      if(m > 0)
      {
       move(buffery,bufferx); prints(" ");
       buffer = ll[m]; bufferx = 19; buffery = 1;
       move(1,19); prints("*");
      }
      break;
    }
  }
 }
}
