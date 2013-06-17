#define TAX 3 /* ¨t²Î©âµ|¦Ê¤À¤§´X */
#define CMMTN 3 /* ¶}½LªÌ©âµ|¦Ê¤À¤§´X */
#define maxboard 40
#include "bbs.h"
#include "record.c"
#include "cache.c"
#define DOTPASSWDS	BBSHOME"/.PASSWDS"
#define FN_PASSWD       ".PASSWDS"      /* User records */
char *fn_passwd         = FN_PASSWD;
userec xuser;

userec cuser;

Rename(char* src, char* dst)
{
   char cmd[200];

   if (f_mv(src, dst) == -1)
      return 0;

   sprintf(cmd, "/bin/mv %s %s", src, dst);
   return system(cmd);
}

void
keeplog(fpath, board, title)
char *fpath;
char *board;
char *title;
{
  fileheader fhdr;
  char genbuf[256];
  char *flog;
  FILE *fs;
  if(fs=fopen(fpath,"r"))
  {
   fclose(fs);
  if(!board)
    board = "LocalGame";

  sprintf(genbuf, BBSHOME"/boards/%s", board);
  stampfile(genbuf, &fhdr);
  Rename(fpath, genbuf);

  strcpy(fhdr.title, title);
  strcpy(fhdr.owner, "¨t²Î¶}¼ú­û");
  sprintf(genbuf, BBSHOME"/boards/%s/.DIR", board);
  rec_add(genbuf, &fhdr, sizeof(fhdr));
  }
}

main()
{
  time_t close_time;
  int i,ans,now,all,ch[9],tmp,b,betmoney,money;
  float haha;
  char genbuf[250],dd[100],tmpb[201],id[13],dscrpt[100];
  FILE *inf,*fs,*ouf,*logfile;

  inf=fopen(BBSHOME"/game/bet.lock","w");
  fprintf(inf,"\n¥»ÀÉ®×¦s¦b¬°¨t²Îµo¿ú®É¶¡, µLªkÀ£ª`.\n");
  fclose(inf);

  for(now=1;now<(maxboard+1);now++)
  {
    sprintf(genbuf,BBSHOME"/game/bet.ans%d",now);
    if(inf=fopen(genbuf,"r"))
  {
  fscanf(inf,"%d",&ans);
  fclose(inf);
  logfile=fopen("log/bet.log","w");
  fprintf(logfile,"[44;1m¡i²Ä %d ­Ó¶}§½©v¦®¡j[m\n",now);

  sprintf(genbuf,BBSHOME"/game/bet.scr%d",now);

  if(fs=fopen(genbuf,"r")){
    while(fgets(tmpb,200,fs))
      {fprintf(logfile,tmpb);}
    fclose(fs);
  }

  fs=fopen(BBSHOME"/game/bet.list","r");
  for(i=0;i<maxboard;i++)
  {
    fscanf(fs,"%d %ld %s\n",&betmoney,&close_time,dd);
    fgets(dscrpt,60,fs);
    if(i+1==now) break;
  }
  fclose(fs);

  i=strlen(dscrpt)-1;
  dscrpt[i]=0;
  fprintf(logfile,"\n[44;1m½s¸¹      ¶}§½ªÌ   ³æ»ù  ¶µ¥Ø±Ô­z              [m\n");
  fprintf(logfile,"[32;1m %2d %12s   %5d  %-30s[m\n\n",now,dd,betmoney,dscrpt);

  sprintf(genbuf,BBSHOME"/game/bet.cho%d",now);
  fs=fopen(genbuf,"r");
  fscanf(fs,"%d\n%d\n",&tmp,&all);
  for(i=0;i<tmp;i++)
  {
    fgets(genbuf,200,fs);
    fscanf(fs,"%d\n",&ch[i]);
    if(i+1==ans)
      fprintf(logfile,"[1;33m%d)%6d    %3.3f %s[m",i+1,ch[i],(float)all/ch[i],genbuf);
    else
      fprintf(logfile,"%d)%6d    %3.3f %s",i+1,ch[i],(float)all/ch[i],genbuf);
  }
  fclose(fs);

  haha=(float)all/ch[ans-1];

  fprintf(logfile,"\n\n[¶}¼úµ²ªG]¡G%d\n\n®¥³ß¤U¦C¨Ï¥ÎªÌ!\n",ans);

  sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",now,ans);
  if(fs=fopen(genbuf,"r")){
    while(fscanf(fs,"%s %d\n",id,&b)!=EOF){
     money=b*betmoney*haha*(100-TAX-CMMTN)/100;
     fprintf(logfile,"[%d;1m %12s Ä¹±o %10d[m\n",2+31,id,money);
     inumoney(id,money);
//     sprintf(genbuf, "%s\nºÆ¨g½ä½L±zÄ¹¤F %d ¤¸ (¸Ô±¡¨£ Bet ª©)",dscrpt, money);
//     x_tell("[¯îÂÕ¶}¼ú¤ý]", "¤¤¼ú°O¿ý", genbuf, id);
    }
  fclose(fs);
  sprintf(genbuf,"rm -f game/bet.cho%d.*",now);
  system(genbuf);
  }

  money=all*betmoney*(CMMTN)/100;
  inumoney(dd,money);
  fprintf(logfile,"\n\n[¶}§½ªÌ] [33;1m%s[m ©â¦¨ (%d%%) ==> [32;1m%d[m\n\n\n",dd,CMMTN,money);

  sprintf(genbuf,"rm -f game/bet.cho%d game/bet.ans%d game/bet.scr%d",now,now,now);
  system(genbuf);

   fs=fopen(BBSHOME"/game/bet.list","r");
   inf=fopen(BBSHOME"/game/bet.list.w","w");
   for(i=0;i<maxboard;i++)
   {
     fscanf(fs,"%d %ld %s\n",&tmp,&close_time,dd);
     fgets(genbuf,60,fs);
     if(i+1==now)
       fprintf(inf,"0 111 dsyan\ndsyan\n");
     else
       fprintf(inf,"%d %ld %s\n%s",tmp,close_time,dd,genbuf);
   }
   fclose(fs);
   fclose(inf);
   fclose(logfile);
   system("mv -f game/bet.list.w game/bet.list");
   sprintf(genbuf,"½L¸¹[%d] %s",now,dscrpt);
   keeplog("log/bet.log", "LocalGame.", genbuf);

}}
   system("rm -f game/bet.lock");
}

int
inumoney(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      substitute_record(DOTPASSWDS,&xuser, sizeof(userec), unum);
      xuser.silvermoney += money;
      substitute_record(DOTPASSWDS,&xuser, sizeof(userec), unum);
      return xuser.silvermoney;
    }
  else
      return -1;
}


int getuser(userid)
  char *userid;
{
  int uid;
  if (uid = searchuser(userid))
    rec_get(DOTPASSWDS, &xuser, sizeof(xuser), uid);
  return uid;
}

