/*-------------------------------------------------------*/
/* money.c       ( WD-BBS Version 1.54 )                 */
/*-------------------------------------------------------*/
/* target : money control function                       */
/* create : 98/08/16                                     */
/* update : 99/03/05                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

int
inumoney(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      xuser.silvermoney += money; 
      substitute_record(fn_passwd, &xuser,sizeof(userec), unum); 
      return xuser.silvermoney;
    }
  else
      return -1;
}

int
inugold(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      xuser.goldmoney += money; 
      substitute_record(fn_passwd, &xuser,sizeof(userec), unum); 
      return xuser.goldmoney;
    }
  else
      return -1;
}

int
deugold(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      
      if(xuser.goldmoney < money)
        xuser.goldmoney = 0;
      
      else
     	xuser.goldmoney -= money;   
       
      substitute_record(fn_passwd, &xuser,sizeof(userec), unum); 
      return xuser.goldmoney;
    }
  else
      return -1;
}

int
deugold1(unsigned long int money)
{
  getuser(cuser.userid);
  if(xuser.goldmoney <= money) 
  {
    if(cuser.silvermoney > (money-cuser.goldmoney)*10000)
      cuser.silvermoney-=(money-cuser.goldmoney)*10000;
    else
      cuser.silvermoney=0;
    cuser.goldmoney=0;
    
  }
  else
    cuser.goldmoney = xuser.goldmoney - money;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.goldmoney;
}


int
inmoney(int money)
{
      getuser(cuser.userid);
      cuser.silvermoney = xuser.silvermoney + money;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.silvermoney;
}

int
ingold(unsigned long int money)
{
      getuser(cuser.userid);
      cuser.goldmoney = xuser.goldmoney + money;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.goldmoney;
}

int
inmailbox(int m)
{
      getuser(cuser.userid);
      cuser.exmailbox = xuser.exmailbox + m;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.exmailbox;
}

int
deumoney(char *tuser, int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      if((unsigned long int)xuser.silvermoney <=
         (unsigned long int) money) xuser.silvermoney=0;
      else xuser.silvermoney -= money;
      substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
      return xuser.silvermoney;
    }
  else
      return -1;
}

int
demoney(unsigned long int money)
{
  getuser(cuser.userid);
  if(xuser.silvermoney <= money) 
  {
    if(cuser.goldmoney > (money-xuser.silvermoney)/10000)
      cuser.goldmoney -= (money-xuser.silvermoney)/10000;
    else
      cuser.goldmoney=0;
    cuser.silvermoney=0;
    pressanykey("�ȹ����� , ��������!");
  }
  else
    cuser.silvermoney = xuser.silvermoney - money;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.silvermoney;
}

int
degold(unsigned long int money)
{
  getuser(cuser.userid);
  if(xuser.goldmoney <= money) 
  {
    if(cuser.silvermoney > (money-cuser.goldmoney)*10000)
      cuser.silvermoney-=(money-cuser.goldmoney)*10000;
    else
      cuser.silvermoney=0;
    cuser.goldmoney=0;
    pressanykey("�������� , �����ȹ�!");
  }
  else
    cuser.goldmoney = xuser.goldmoney - money;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.goldmoney;
}

/* ���� Multi play */
int
count_multiplay(int unmode)
{
  register int i, j;
  register user_info *uentp;
  extern struct UTMPFILE *utmpshm;

  resolve_utmp();
  for (i = j = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->uid == usernum)
     if(uentp->lockmode == unmode)
      j++;
  }
  return j;
}

