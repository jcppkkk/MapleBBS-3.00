//#include "bbs.h"
#include<stdio.h>
#include<stdlib.h>
//#include<conio.h>
#include<string.h>
#include<math.h>
#define MAXUSERS 256

typedef struct userlink userlink;
struct userlink
{
	int id;
	userlink *link;
};

typedef struct fivelog fivelog;
struct fivelog
{
	 char username[20];
	 int  id;
	 int  win;
	 int  lose;
	 int  equ;
	 double percent;
	 int  loserscount;
	 double grades;
	 userlink *link;
};

struct fivelog allman[MAXUSERS];
int strfind(char *, char *, int);
void strget(char *, int, int, char *);

int userlog(char *, int);
void sortlog(void);
void xchglog(fivelog *, fivelog *);

void loserlink_make(userlink **, int);
void loserlink_add(userlink **, int);

int loserscount(userlink *, int);
double user_grades_computer(int);

double grades_add_from_link(userlink *);
double search_grades_from_id(int);

int search_name(char *);
void search_name_from_link(userlink *);
void search_name_from_id(int );

void display(int);

int top=0 ,row=20, mode=0;

int main(int argc, char **argv)
{
  char s1[100],s2[20],s3[20],s4[20];
  FILE *inf,*inf2;
  int i,j,k,w;

  if (argc != 5)
  {
//		printf("usage error!\n");
//		exit(-1);
  }

  inf = fopen("/home/bbs/log/five.log", "r");
//  inf = fopen(argv[1],"r");
  inf2 = fopen("/home/bbs/log/f2.log", "w");
//  inf2 = fopen(argv[2],"w");

  if (inf == NULL || inf2 == NULL)
  {
	  printf("Sorry, the data is not ready.\n");
	  exit(-1);
  }

//  row=atoi(argv[3]);
//  mode=atoi(argv[4]);

  for (i=0; fgets(s1,sizeof(s1),inf) ;i++)
  {
		j = strfind(s1,"[32m",0)+1;
		k = strfind(s1," ",j)-1;
		strget(s1,j,k,s2);
		j = strfind(s1,"[36m",j)+1;
		j = strfind(s1," ",j)+1 ;
		k = strfind(s1," ",j)-1;
		strget(s1,j,k,s3);
		j = strfind(s1," ",k)+1;
		strget (s1,j,j+3,s4);

		if (strcmp(s4,"¾Ô©M")==0)			 w=0;
		else                     			 w=1;
		k=userlog(s2,w);
		j=userlog(s3,-w);
		if (w)   loserlink_make(&(allman[k].link),j);
  }
  fclose(inf);

  for(i=0 ; i<top ; i++)
  {
	  allman[i].loserscount = loserscount(allman[i].link,0);
	  allman[i].percent = (double) allman[i].win / (allman[i].win+allman[i].lose+allman[i].equ);
  }
  for(i=0 ; i<top ; i++)
	  allman[i].grades = user_grades_computer(i) + grades_add_from_link(allman[i].link);

  sortlog();
  display(mode);
  fclose(inf2);
  return 0;
}

void display(int mode)
{
  int i;
//  fprintf(inf2,"¢w¢w¤­¤l´Ñ¹ï¾Ô±Æ¦æº]¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w\n");
  printf("[1;33m¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢s¡÷ [41m     ¤­¤l´Ñ ±Æ¦æº]     [40m ¡ö¢s¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³[m\n");
  printf("[1;33m¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢r¢w¢w¢w¢w¢w¢w¢w ¢w¢w¢w¢w¢w¢w¢w¢r¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³[m\n");
  printf("[1;37;41m¦W¦¸ [35;44m USERID          [32m³Ó    [36m±Ñ  [32m©M  [31mKill   [36m³Ó²v      [33m¤p­p     [37m¥[¤À      [33mÁ`¤À[m\n");
//  if (mode==0)
//	  printf("no.  ID.       wins lose  equ kill  100%     basic +   add =    grades\n");
//  else
//	  printf("no.  ID.           losers list\n");
  for (i=0; i<row; i++)
  {
//		if (i%10==0)
//			printf("¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w\n");
		if (mode==0)
		{
			printf("[1;37m%3d.  [35m%-13s [32m%4d [36m%4d [32m%4d [31m%4d  [36m%6.2lf [33m%9.1lf [m + [1;37m%5.1lf [m =[1;33m%7.1lf[m\n",i+1,
			allman[i].username,
			allman[i].win,
			allman[i].lose,
			allman[i].equ,
			allman[i].loserscount,
			allman[i].percent,
			user_grades_computer(i),
			grades_add_from_link(allman[i].link),
			allman[i].grades);
		}
		else
		{
			printf("%2d,  %-10s -> ",i+1,allman[i].username);
			search_name_from_link(allman[i].link);
			printf("\n");
		}
  }
}

int strfind(char *a, char *b, int st)
{
	 int i, j, k=strlen(a) , l=strlen(b);

	 for (i=st,j=0;i<k;i++)
	 {
		  if (a[i]==b[j])
				j++;
		  if (j==l)
				return i;
	 }
	 return -1;
}

void strget(char *s, int a,int  b,char t[20])
{
	int i;
	 for(i=a ; i<=b ; i++)
		  t[i-a]=s[i];
	 t[i-a]='\0';
}

int userlog(char name[20], int w)
{
	 int i;
	 for (i=0; i<top ; i++)
		  if (strcmp(allman[i].username,name)==0)
				break;

	 if (i==top)
	 {
		  top++;
		  strcpy(allman[i].username,name);
		  allman[i].id=i;
		  allman[i].win=0;
		  allman[i].lose=0;
		  allman[i].equ=0;
		  allman[i].percent=0;
		  allman[i].link=NULL;
		  allman[i].loserscount=0;
	 }

	 if (w == 0)
		  allman[i].equ++;
	 else if (w == 1)
		  allman[i].win++;
	 else
		  allman[i].lose++;
	 return i;
}

void sortlog(void)
{
	 int i,j;
	 fivelog tmpman;

	 for (i=0;i<top-1;i++)
		  for (j=i+1;j<top;j++)
				if (allman[i].grades < allman[j].grades)
				{
				 //	 xchglog(&allman[i], &allman[j]);
					 memcpy(&tmpman,&allman[i],sizeof(fivelog));
					 memcpy(&allman[i],&allman[j],sizeof(fivelog));
					 memcpy(&allman[j],&tmpman,sizeof(fivelog));
				}
}

void xchglog(fivelog *a, fivelog *b)
{
		  struct fivelog tmpman;
		  strcpy(tmpman.username,a->username);
		  tmpman.id=a->id;
		  tmpman.win=a->win;
		  tmpman.lose=a->lose;
		  tmpman.equ=a->equ;
		  tmpman.percent=a->percent;
		  tmpman.link=a->link;
		  tmpman.loserscount=a->loserscount;
		  tmpman.grades=a->grades;
		  strcpy(a->username,b->username);
		  a->id=b->id;
		  a->win=b->win;
		  a->lose=b->lose;
		  a->equ=b->equ;
		  a->percent=b->percent;
		  a->link=b->link;
		  a->loserscount=b->loserscount;
		  a->grades=b->grades;
		  strcpy(b->username,tmpman.username);
		  b->id=tmpman.id;
		  b->win=tmpman.win;
		  b->lose=tmpman.lose;
		  b->equ=tmpman.equ;
		  b->percent=tmpman.percent;
		  b->link=tmpman.link;
		  b->loserscount=tmpman.loserscount;
		  b->grades=tmpman.grades;
}

void loserlink_make(userlink **man, int id)
{
  if (*man==NULL)
	  loserlink_add(man, id);
  else if ((*man)->id != id)
		  loserlink_make(&((*man)->link), id);
}

void loserlink_add(userlink **man, int id)
{
	userlink *tmp;
	tmp = (userlink *) malloc(sizeof(userlink));
	tmp->id = id;
	tmp->link = NULL;
	*man = tmp;
	tmp = NULL;
	free(tmp);
}

int loserscount(userlink *man, int c)
{
	if (man == NULL)
		return c;
	else
		return (loserscount(man->link,c+1));
}

double user_grades_computer(int id)
{
	int j;
	double g;
	g=(double)(allman[id].win);
	g=g+(double)(allman[id].lose) * 0.1;
	g=g+(double)(allman[id].equ) * allman[id].percent;
	g=g+(double)(allman[id].loserscount);
	j=allman[id].win-allman[id].lose;
	j=(j>0)?j:0;
	g=g+(double)j*allman[id].loserscount;
	return g;
}

double grades_add_from_link(userlink *man)
{
	if (man != NULL)
		return (search_grades_from_id(man->id) + grades_add_from_link(man->link));
	else
		return 0;
}

double search_grades_from_id(int id)
{
	int i;
	for (i=0;i<top;i++)
		if (allman[i].id==id)
		{
//			printf("%-5s(%4.2lf) ",allman[i].username,user_grades_computer(i));
			return user_grades_computer(i)/(allman[i].lose+allman[i].equ+1);
		}
	return 0;
}


int search_name(char name[20])
{
  int i;
  for (i=0; i<top ; i++)
	  if (strcmp(allman[i].username,name)==0)
	  {
		  printf("%-10s -> ",name);
		  search_name_from_link(allman[i].link);
		  printf("\n");
		  return i;
	  }
  return -1;
}

void search_name_from_link(userlink *man)
{
	if (man != NULL)
	{
		search_name_from_id(man->id);
		search_name_from_link(man->link);
	}
}


void search_name_from_id(int id)
{
	int i;
	for (i=0;i<top;i++)
		if (allman[i].id==id)
			printf("%s,",allman[i].username,user_grades_computer(i));
}
