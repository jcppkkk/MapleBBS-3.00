#include "bbs.h"

char ordersongfile[120];

int orderabort()
{
  vmsg("�����I�q");
  return 0;
}

int ordersong()
     // wrapper of ordersong
{
  add_pos(POS_ALL, "KK�I�q��");
  ordersong_p();
  remove_pos();
}


int ordersong_p()
{
  char userid[80], nick[20], *out, fpath[80], say[80];
  int ans, fd;
  HDR hdr;

  vs_head("KK�I�q��", str_site);
  move(2, 0);
  outs("�Q�n�I���֩O? (EMail �άO���W�� id ���i�H)");
  if(!vget(3, 0, "> ", userid, 75, DOECHO))
    return orderabort();

  out = strchr(userid, '@');
  if(!out) {
    if(!acct_userno(userid)) {
      vmsg("�S���o�ӤH�@, ����I�q");
      return 0;
    }
  } else
    *out = '\0';

  str_ncpy(nick, userid, 20);

  if(!vget(5, 0, "��誺�ʺ٬O����O? ", nick, 20, GCARRY))
    str_ncpy(nick, userid, 20);

  if(out)
    *out = '@';

  move(7, 0);
  outs("�Q�n��L(�o)������ܩO?");
  vget(8, 0, "> ", say, 75, DOECHO);

  ans = vans("�аݭn���n��W�ʺA�ݪO? (Y: �n  n: ���n) [Y] ");

  vmsg("�n�I�q�F, �Ы����N�@��i�J�I�q�ؿ�");
  if(!XoOrderSong("gem/brd/KKCity/Song/@/@songs", "KK �I�q�t��"))
    return orderabort();

  usr_fpath(fpath, cuser.userid, fn_note);
  {
    FILE *from, *to;
    char buf[512], buf2[1024];
    int i = 0, j, k;

    from = fopen(ordersongfile, "r");
    to = fopen(fpath, "w");

    while(i <= 11 && fgets(buf, 512, from)) {
      j = k = 0;
      while(k < 512) {
	if(buf[j] == '<') {
	  register char *tmp = NULL;
	  if(!str_ncmp(&buf[j], "<~src~>", 7))
	    tmp = cuser.userid;
	  else if(!str_ncmp(&buf[j], "<~des~>", 7))
	    tmp = nick;
	  else if(!str_ncmp(&buf[j], "<~say~>", 7))
	    tmp = say;
	  if(tmp) {
	    j += 7;
	    while(*tmp)
	      buf2[k++] = *tmp++;
	    continue;
	  }
	}
	buf2[k++] = buf[j++];
      }
      buf2[k] = 0;
      fputs(buf2, to);
    }
    fclose(to);
    fclose(from);
  }

  if(ans != 'n') {
    hdr_stamp("gem/brd/KKCity/Song/@/@ordered", HDR_LINK | 'A', &hdr, fpath);
    sprintf(hdr.title, "%s �I�� %s", cuser.userid, nick);
    strcpy(hdr.owner, cuser.userid);
    rec_add("gem/brd/KKCity/Song/@/@ordered", &hdr, sizeof(HDR));
  }

  if(out) {
    char title[80];
    sprintf(title, "%s �I���z���q", cuser.userid);
    outs("�еy��, �I�q�ǻ���...\n");
    refresh();
    fd = bsmtp(fpath, title, userid, 0);
    if (fd < 0)
      vmsg("�I�q����L�k�H�F");
    mail_hold(fpath, userid);
  } else {
    char folder[80];

    usr_fpath(folder, userid, fn_dir);
    hdr_stamp(folder, HDR_LINK, &hdr, fpath);
    strcpy(hdr.owner, cuser.userid);
    strcpy(hdr.nick, cuser.username);    /* :chuan: �[�J nick */
    sprintf(hdr.title, "%s �I���z���q", cuser.userid);
    fd = rec_add(folder, &hdr, sizeof(hdr));
    if (!fd) {
      mail_hold(fpath, folder);
    }
  }
  usr_fpath(fpath, cuser.userid, fn_note);
  unlink(fpath);
  vmsg("�I�q����");

  return 0;
}
