#include "bbs.h"

#define  RPG BBSHOME"/.RPGS"
#define  OUTRPG "RPGS.NEW"
char yn[10]="",flag=1;
int coun=0;
rpgrec rpgu;

int
bad_user_id()
{
  register char ch;
  int j;

  if (strlen(rpgu.userid) < 2 || !isalpha(rpgu.userid[0]))
    return 1;

/*
  if (!strcasecmp(rpgu.userid, "new"))
    return 1;
*/

  for(j=1;ch = rpgu.userid[j];j++)
  {
    if (!isalnum(ch))
      return 1;
  }
  return 0;
}


struct new
{
  char userid[IDLEN+1];		  /* User ID     13 bytes */
  usint age;			  /* �~��	  4 bytes */
  uschar race;			  /* �ر�	  1 bytes */
  uschar subrace;		  /* �Ʒ~	  1 bytes */
  ushort level;			  /* ����	  2 bytes */  
  char family[20];		  /* �a��	 20 bytes */
  char nick[20];		  /* �ʸ�	 20 bytes */
  int hp;			  /* ��O	  4 bytes */
  int mp;			  /* �k�O	  4 bytes */
  usint skill;			  /* �ޯ�	  4 bytes */
  ushort str;			  /* �O�q	  2 bytes */
  ushort dex;			  /* �ӱ�	  2 bytes */
  ushort wis;			  /* ���z	  2 bytes */
  ushort con;			  /* ���	  2 bytes */
  ushort kar;			  /* �B��	  2 bytes */
  uschar weapon;		  /* �Z��	  1 bytes */
  uschar armor;			  /* ����	  1 bytes */
  usint object;			  /* ����	  4 bytes */
  char pad[164];
};
typedef struct new new;

char *
Cdate(clock)
  time_t *clock;
{
  static char foo[22];
  struct tm *mytm = localtime(clock);

  strftime(foo, 22, "%D %T %a", mytm);
  return (foo);
}

main() 
{
  new new;
  FILE *fp1=fopen(RPG,"r");
  FILE *fp2=fopen(OUTRPG,"w");

  printf("size [%d] to size [%d]",sizeof(rpgrec),sizeof(new));
  if(sizeof(new) != sizeof(rpgrec)) return;
     while( (fread( &rpgu, sizeof(rpgu), 1, fp1))>0 ) {

  memset(new.pad, 0, sizeof(new.pad));

  strcpy(new.userid,rpgu.userid);
  new.age=0;
  new.race=rpgu.race;
  new.subrace=rpgu.subrace;
  new.level=rpgu.level;
  strcpy(new.family,rpgu.family);
  strcpy(new.nick,rpgu.nick);
  new.hp=rpgu.hp;
  new.mp=rpgu.mp;
  new.skill=rpgu.skill;
  new.str=rpgu.str;
  new.wis=rpgu.wis;
  new.dex=rpgu.dex;
  new.con=rpgu.con;
  new.kar=rpgu.kar;
  new.weapon=rpgu.weapon;
  new.armor=rpgu.armor;
  new.object=rpgu.object;

coun ++;
printf("******** %d******** \n",coun);
printf("userid	  :%s\n",new.userid);
    if(flag)
      {
	 printf("\n Accept the data?(Y:yes/n:no/a:all/q:quit all after) :");
	 scanf("%1s",yn);
	 switch(yn[0])
	  {
	   case 'n':  
	   case 'N':
	     printf("��󦹸��\n");
	     break;
 	  case 'a':
          case 'A':
	      flag =0;
	     printf("�����Ҧ����\n");
		break;
          case 'q':
          case 'Q':
              flag =0;
              printf("����������Ҧ����\n");
	      goto end;
	  default:
	     printf("�������\n");
	     fwrite( &new, sizeof(new), 1, fp2);
	  }
	}
	else	
	{
	     fwrite( &new, sizeof(new), 1, fp2);
	} 
       }
end:
 fclose(fp1);fclose(fp2);
}
