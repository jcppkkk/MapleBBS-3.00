/*
�N�Ҧ� bad_user_id �M�� (�]�A new)�A�o�N�� .PASSWDS ���b�ǡC

�n�b share memory �٨S���J�ɰ���A�̦n�b�}���ɰ���A�_�h�|�y
����ƿ��ê��{�H�A�Y�n�^�_��ק�e�����A�A�i�H�b�����J
share memory ��:
cp ~bbs/PASSWDS ~bbs/.PASSWDS

�i�ϥ� ipcs �o�� share memory �O�_�w�g���J�A�Ϊ̰��� repasswd
�e���� ipcrm �� share memory �񱼡Afor FreeBSD:
ftp://sob.m7.ntu.edu.tw/sob-version/bin/shutdownbbs
*/

#include <stdio.h>
#include "bbs.h"

#define DOTPASSWDS	BBSHOME"/.PASSWDS"
#define PASSWDSBAK	BBSHOME"/PASSWDS"
#define TMPFILE		BBSHOME"/tmp/tmpfile"


struct userec cuser;

main()
{
    FILE *foo1, *foo2, *foo3, *foo4;
    int cnum,i,match;

    setgid(BBSGID);
    setuid(BBSUID);
    if( ((foo1=fopen(DOTPASSWDS, "r")) == NULL)
                || ((foo2=fopen(TMPFILE,"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    while( (cnum=fread( &cuser, sizeof(cuser), 1, foo1))>0 ) {
       if (bad_user_id(cuser.userid))
          continue;
       fwrite( &cuser, sizeof(cuser), 1, foo2);
    }
    fclose(foo1);
    fclose(foo2);

    if(f_mv(DOTPASSWDS,PASSWDSBAK)==-1){
        puts("replace fails");
        exit(1);
    }
    unlink(DOTPASSWDS);
    f_mv(TMPFILE,DOTPASSWDS);
    unlink("tmpfile");

    return 0;
}


bad_user_id(userid)
  char *userid;
{
  register char ch;

  if (strlen(userid) < 2)
    return 1;

  if (!isalpha(*userid))
    return 1;

  if (!strcasecmp(userid, "new"))
    return 1;

  while (ch = *(++userid))
  {
    if (!isalnum(ch))
      return 1;
  }
  return 0;
}
