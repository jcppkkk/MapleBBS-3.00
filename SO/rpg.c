#include "bbs.h"
#include "rpg.h"


/* �j�Y�� */
void
showpicture(char *uid)
{
  char genbuf[200];

  sethomefile(genbuf, uid, "picture");
  if (!show_file(genbuf, 11, 12, ONLY_COLOR))
    prints("[1;32m�i�j�Y�ӡj[m%s �ثe�S���j�Y��", uid);
}

int
u_editpic()
{
  char genbuf[200];
  stand_title("�s��j�Y��");
  showpicture(cuser.userid);
  getdata(b_lines - 1, 0, "�j�Y�� (D)�R�� (E)�s�� [Q]�����H[Q] ", genbuf, 3, LCECHO,0);

  if (genbuf[0] == 'e')
  {
    int aborted;

    setutmpmode(EDITPIC);
    setuserfile(genbuf, "picture");
    aborted = vedit(genbuf, NA);
    if (aborted != -1)
    pressanykey("�j�Y�ӧ�s����");
    return 0;
  }
  else if (genbuf[0] == 'd')
  {
    setuserfile(genbuf, "picture");
    unlink(genbuf);
    outmsg("�j�Y�ӧR������");
  }
  return 0;
}

/* �p��U���𨾭� */

int
c_att(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.str*10)+(rpgtmp.dex*7)+(rpgtmp.kar*5))/3);
  return i;
}

int
c_def(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.con*10)+(rpgtmp.dex*7)+(rpgtmp.kar*5))/3);
  return i;
}

int
c_ma(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.wis*10)+(rpgtmp.kar*5))/2);
  return i;
}

int
c_md(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.con*10)+(rpgtmp.dex*7)+(rpgtmp.wis*7)+(rpgtmp.kar*5))/4);
  return i;
}

int
rpg_udisplay(char *id)
{
  int MHP,MMP,Att,MA,Def,MD;
  if(!getuser(id)) return 0;
  MHP = rpgtmp.con*30;
  MMP = rpgtmp.wis*10;
  Att = c_att(id);
  Def = c_def(id);
  MA = c_ma(id);
  MD = c_md(id);

  setutmpmode(RINFO);
  clear();
  outs(COLOR1"[1m ���������������������������� �ϥΪ̡@��ޢա@��� ���������������������������� [m\n");
  prints("[1;32m�i�m�W�j[m%-16.16s[1;32m�i¾�~�j[m%-15.15s[1;32m�i���šj[m%-5d[1;32m�i�g��ȡj[m%d\n"
       ,rpgtmp.userid,rname[rpgtmp.race],rpgtmp.level,xuser.exp);
  prints("[1;32m�i�~�֡j[m%6d ��  [1;32m�i��O�j[m%6d/%-6d [1;32m�i�k�O�j[m%6d/%-6d\n",
       (rpgtmp.age/2880)+10,rpgtmp.hp,MHP,rpgtmp.mp,MMP);
  prints("[1;32m�i�ݩʡj[m[1;33m�O�q(STR)[m%-3d [1;33m���z(WIS)[m%-3d\
 [1;33m�ӱ�(DEX)[m%-3d [1;33m���(CON)[m%-3d [1;33m�B��(KAR)[m%-3d\n"
       ,rpgtmp.str,rpgtmp.wis,rpgtmp.dex,rpgtmp.con,rpgtmp.kar);
prints("[1;32m�i�����O�j[31m �@�� [m%3d [1;36m�]�k[m %3d    \
[1;32m�i���m�O�j[31m �@�� [m%3d [1;36m�]�k[m %3d\n"
       ,Att,MA,Def,MD);
  outs("\n\n\n\n");
  outs(COLOR1"[1m ������������������������������������������������������������������������������ [m\n");
  showpicture(rpgtmp.userid);
  pressanykey(NULL);
  return 1;
}

void
rpg_uinfo()
{
  rpg_udisplay(cuser.userid);
  return;
}

void
rpg_race_c()
{
  int strlv1[7] = {0,5,4,3,5,4,4};
  int wislv1[7] = {0,4,5,4,3,3,4};
  int dexlv1[7] = {0,4,4,4,5,5,4};
  int conlv1[7] = {0,4,4,5,3,4,3};
  int karlv1[7] = {0,3,3,4,4,4,5};
  int ans;
  char buf[2],ans2[2];

  setutmpmode(RCHOOSE);
  log_usies("RPG_Choose",NULL);
  more(BBSHOME"/game/rpg/choose_race",YEA);
  stand_title("�D��¾�~");
  if(rpguser.race)
  {
    move(2,4);
    outs("�A�w�g��¾�~�o,�ഫ¾�~�|�q���Ť@���s�}�l��I");
    getdata(10,5,"�O�_�T�w�H (y/N)",buf,3,DOECHO,"n");
    if(!buf[0] || buf[0] == 'n' || buf[0] == 'N') return;
  }
  move(4,0);
  outs(       "�D��¾�~�G (1)����j�H (2)Ū��֤� (3)�`�n�{��");
  getdata(5,0,"           (4)��Ѳr�N (5)���y�a�x (6)�C���g�P (0)���",
    buf,2,DOECHO,0);
  if(!buf[0] || buf[0] < '1' || buf[0] > '7') return;
    else ans = atoi(buf);
  if(ans == 7) ans =0;
  clear();
  prints("\n�A��ܤF %s �I ��U���ƭȦp�U�G\n",rname[ans]);
  prints("����������������������������������\n");
  prints("��      ���Ť@�ɦU���ƭ�        ��\n");
  prints("����������������������������������\n");
  prints("  �O�q�G%d\n",strlv1[ans]);
  prints("  ���z�G%d\n",wislv1[ans]);
  prints("  �ӱ��G%d\n",dexlv1[ans]);
  prints("  ���G%d\n",conlv1[ans]);
  prints("  �B��G%d\n",karlv1[ans]);
  prints("  ��O�G%3d       �k�O�G%d\n",conlv1[ans]*30,wislv1[ans]*10);
  prints("  �@������O�G%3d �@�먾�m�O�G%d\n",
  ((strlv1[ans]*10)+(dexlv1[ans]*7+karlv1[ans]*5))/3,((conlv1[ans]*10)+(dexlv1[ans]*7)+(karlv1[ans]*5))/3);
  prints("  �]�k�����O�G%3d �]�k���m�O�G%d\n",
  ((wislv1[ans]*10)+(karlv1[ans]*5))/2,((conlv1[ans]*7)+(dexlv1[ans]*5)+(wislv1[ans]*5)+(karlv1[ans]*3))/4);

  getdata(14,5,"�O�_�T�w�H (y/N)",ans2,2,LCECHO,0);
  if(ans2[0] == 'y' || ans2[0] == 'Y')
  {
    degold(5);
    rpguser.race = ans;
    rpguser.level = 1;
    rpguser.str=strlv1[ans];
    rpguser.wis=wislv1[ans];
    rpguser.dex=dexlv1[ans];
    rpguser.con=conlv1[ans];
    rpguser.kar=karlv1[ans];
    rpguser.hp=conlv1[ans]*30;
    rpguser.mp=wislv1[ans]*10;
    rpg_rec(cuser.userid,rpguser);
    rpg_uinfo();
  }
  else
    rpg_race_c();
}

/* ¾�~�u�| */

rpg_guild()
{
  char ans[5],buf[200];
  int l = rpguser.level;
  update_data();
  if (!rpguser.race)
  {
    pressanykey("�z�S��¾�~�A�Х� Join �[�J���C��");
    return 0;
  }
  setutmpmode(RGUILD);
  log_usies("RPG_Guild",NULL);
  clear();
  prints("[1m[33;42m�i %s �P�~�u�|�j[m  �A�{�b���Ŭ� %d�A���g��� %d �I\n\n"
    ,rname[rpguser.race],rpguser.level,cuser.exp);
  prints("\
    ([1;36m0[m)�_��            �_���|��h�Ҧ��ݩʤ@�I,�g��Ȧ��@�b
    ([1;36m1[m)�ɯ�            �ɯŶ��g��� %d
    ([1;36m2[m)��_��O        ��O�� %d�A�[������O %d ��
    ([1;36m3[m)��_�k�O        �k�O�� %d�A�[������O %d ��
    ([1;36m4[m)���@�ݩ�        �z���U���ݩʤW���� %d �I(�C�I�� %d �g���)
    ([1;36m5[m)����d�ߤ���    �d�ߧO�H��ơA�C�� 1000 exp
    ([1;36m6[m)�ӹ��]          ���]�u�t�d�~�Ӥ��A�����Цۦ洣��"
  ,LVUP,rpguser.hp,((rpguser.con*30)-rpguser.hp)*10
  ,rpguser.mp,((rpguser.wis*10)-rpguser.mp)*20,l*3,LVUP/10);
  getdata(10,0,"�п�ܧA�ݭn���A�ȡG ",ans,3,LCECHO,0);
  if(!ans[0]) return 0;
  switch(ans[0])
  {
    default:
      break;
    case '!':
      if(HAS_PERM(PERM_SYSOP))
      {
        rpguser.hp = rpguser.con*30;
        rpguser.mp = rpguser.wis*10;
      }
      break;
    case '0':
      if(rpguser.hp > 0)
      {
        pressanykey("�ưաH�S���٨Ӵ_���I");
        break;
      }
      update_data();
      if(rpguser.str > 1)
        rpguser.str -= 1;
      if(rpguser.wis > 1)
        rpguser.wis -= 1;
      if(rpguser.dex > 1)
        rpguser.dex -= 1;
      if(rpguser.con > 1)
        rpguser.con -= 1;
      if(rpguser.kar > 1)
        rpguser.kar -= 1;
      cuser.exp /= 2;
      rpguser.hp = rpguser.con * 15;
      {
        time_t now = time(0);
        sprintf(buf," %s ��h�F %d �I�g��ȡA�q���`����t�Q��F�^�ӡC%s"
          ,cuser.userid,cuser.exp,Cdate(&now));
        f_cat(BBSHOME"/game/rpg/resurge.log",buf);
      }
      sleep(5);
      pressanykey("�@�D���~Ţ�n�b�A���骺�P��,�A�C�C���F��ı....");
      break;
    case '1':
    {
      if(rpguser.hp <= 0)
      {
        pressanykey("�A�w�g�}�`�F,�n���_���~��ɯ�");
        break;
      }
      if(rpguser.level >= 99)
      {
        pressanykey("�A�w�g�ɨ�ثe���̰����ŤF");
        break;
      }
      if(check_exp(LVUP)) break;
      else deexp(LVUP);
      rpguser.str += lvup[0][rpguser.race];
      rpguser.wis += lvup[1][rpguser.race];
      rpguser.dex += lvup[2][rpguser.race];
      rpguser.con += lvup[3][rpguser.race];
      rpguser.kar += lvup[4][rpguser.race];
      rpguser.hp = rpguser.con*30;
      rpguser.mp = rpguser.wis*10;
      if(rpguser.str > (rpguser.level*3)+5)
        rpguser.str = (rpguser.level*3)+5;
      if(rpguser.dex > (rpguser.level*3)+5)
        rpguser.dex = (rpguser.level*3)+5;
      if(rpguser.wis > (rpguser.level*3)+5)
        rpguser.wis = (rpguser.level*3)+5;
      if(rpguser.con > (rpguser.level*3)+5)
        rpguser.con = (rpguser.level*3)+5;
      if(rpguser.kar > (rpguser.level*3)+5)
        rpguser.kar = (rpguser.level*3)+5;
      rpguser.level++;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      rpg_rec(cuser.userid,rpguser);
      pressanykey("���Ŵ��ɦ� %d",rpguser.level);
      rpg_uinfo();
    }
    break;

    case '2':
      if(rpguser.hp <= 0)
      {
        pressanykey("�A�w�g�}�`�F,�n���_���~�����O");
        break;
      }
      if(check_money(((rpguser.con*30)-rpguser.hp)*10,SILVER)) return 0;
      demoney(((rpguser.con*30)-rpguser.hp)*10);
      rpguser.hp = rpguser.con*30;
      pressanykey("��O��_�ܥ������A�I");
      break;

    case '3':
      if(check_money(((rpguser.wis*10)-rpguser.mp)*20,SILVER)) return 0;
      demoney(((rpguser.wis*10)-rpguser.mp)*20);
      rpguser.mp = rpguser.wis*10;
      pressanykey("�k�O��_�ܥ������A�I");
      break;

    case '4':
    {
      char ans2[3],genbuf[100] = "��J���~�άO�w�F��̤j��";
      if(rpguser.hp <= 0)
      {
        pressanykey("�A�w�g�}�`�F,�n���_���~�ണ�@�ݩ�");
        break;
      }
      getdata(15,0,
        "�n���@ : [1]�O�q [2]���z [3]�ӱ� [4]��� [5]�B�� ?",ans2,2,LCECHO,0);
      if(ans2[0] < '1' || ans2[0] > '5') break;
      if(check_exp(LVUP/10)) break;
      switch(ans2[0])
      {
        case '1':
          if(rpguser.str < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"�O�q���ɬ� %d �I",++rpguser.str);
          }
          break;
        case '2':
          if(rpguser.wis < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"���z���ɬ� %d �I",++rpguser.wis);
          }
          break;
        case '3':
          if(rpguser.dex < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"�ӱ����ɬ� %d �I",++rpguser.dex);
          }
          break;
        case '4':
          if(rpguser.con < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"��责�ɬ� %d �I",++rpguser.con);
          }
          break;
        case '5':
          if(rpguser.kar < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"�B�𴣤ɬ� %d �I",++rpguser.kar);
          }
          break;
      }
      pressanykey(genbuf);
      break;
    }

    case '5':
    {
      char genbuf[20];
      if(check_exp(1000)) break;
      stand_title("�d�ߧO�H�����");
      usercomplete(msg_uid,genbuf);
      if(genbuf[0])
      {
        deexp(1000);
        rpg_udisplay(genbuf);
      }
    }
    break;

    case '6':
      u_editpic();
      break;

  }
  substitute_record(fn_passwd,&cuser,sizeof(userec),usernum);
  rpg_rec(cuser.userid,rpguser);
  rpg_guild();
}

void
rpg_top()
{
  int i;
  char buf[5],genbuf[200];
  stand_title("RPG �Ʀ�]");
  setutmpmode(RTOPLIST);
  log_usies("RPG_Toplist",NULL);
  move(4,0);
  outs(       "�n�ݤ���¾�~���Ʀ�G(1)����j�H (2)Ū��֤� (3)�`�n�{��");
  getdata(5,0,"(t)�έp  (a)����    (4)��Ѳr�N (5)���y�a�x (6)�C���g�P",buf,2,DOECHO,0);
  if(buf[0] == 'a' || buf[0] == 'A') buf[0] = '7';
  if(buf[0] == 't' || buf[0] == 'T') buf[0] = '8';
  if(!buf[0] || buf[0] < '0' || buf[0] > '8') return;
  i = atoi(buf);
  sprintf(genbuf,BBSHOME"/log/toprpg%d",i);
  more(genbuf, YEA);
}

/* �m�\ */
int
rpg_kmob(int mode)
{
  mobattr m;
  int MHP = rpguser.con*30,MMP= rpguser.wis*10;  /* �ۤv���̤jmp,hp */
  int Att = c_att(cuser.userid);  /* �����O */
  int Def = c_def(cuser.userid);  /* ���m�O */
  int MA = c_ma(cuser.userid);    /* �]�k���� */
  int MD = c_md(cuser.userid);    /* �]�k���m */
  int money,exp;  /* ������|�W�[or��֪�$$��exp */
  char ans[4],buf[256]= "\0"; /* ans��user input��,buf���ù�output�� */
  int j=0,k=0,l=0,over=0,attack=0; /* over�����԰�,attack�ĤH���� */

  if(rpguser.hp <= 0)
  {
    pressanykey("�A�w�g�}�`�F����");
    return 0;
  }
//  if(lockutmpmode(RTRAIN)) return 0;  /* ����user multi �� */
  setutmpmode(RTRAIN);
  log_usies("RPG_Train",NULL);
  if(mode == hard)
    m.LV = rpguser.level + rand()%10;
  if(mode == medium)
    m.LV = rand()%2 ? rpguser.level + rand()%3 : rpguser.level - rand()%3;
  if(mode == easy)
    m.LV = rand()%3 < 1 ? rpguser.level : rpguser.level - rand()%5;
  if(m.LV < 1) m.LV = 1;
  m.maxhp = m.hp = m.LV * 100;
  m.Att = m.LV * (10 + rand()%15);
  m.Def = m.LV * (5 + rand()%10);
  m.MA = m.LV * (20 + rand()%5);
  m.MD = m.LV * (5 + rand()%5);
  m.EXP = (m.LV*m.LV + mode) * ((rand()%5) + 5);
  m.money = (m.LV*m.LV + mode) * ((rand()%2) + 5);
  while(!over){
    update_data();
    if(mode == easy)
      strcpy(m.name,"�׸}�Ǫ�");
    else if(mode == medium)
      strcpy(m.name,"�@��Ǫ�");
    else
      strcpy(m.name,"�ܺA�Ǫ�");
    clear();
    if(!HAS_PERM(PERM_SYSOP))
    {
      show_file(buf,1,10,ONLY_COLOR);  /* �ĤH�Ϥ� */
      move(1,0);
      prints("���� : %d",m.LV);
    }
    else
    {
      move(1,0);
      prints(
"���� : %d\n�ͩR : %d\n�� : %d / %d\n�]�k : %d / %d\n�g�� : %d\n���� : %d"
,m.LV,m.maxhp,m.Att,m.Def,m.MA,m.MD,m.EXP,m.money);
    }
    move(11,0);
    prints("[36m%s[m",msg_seperator);  /* ���j�u */
    move(13,0);
    prints("[36m%s[m",msg_seperator);  /* ���j�u */
    over = 0;
    while(over == 0 && rpguser.hp > 0 && m.hp >0)  /* �԰�...���� over */
    {
      /* ����user���D�Ǫ����h�֦�A�ҥH�Ψ��骬�A��� */
      j = (m.hp/(m.maxhp/100) >= 100 ? 0 :
           m.hp/(m.maxhp/100) >= 75  ? 1 :
           m.hp/(m.maxhp/100) >= 50 ? 2 :
           m.hp/(m.maxhp/100) >= 25 ? 3 : 4);
      move(0,0);
      clrtoeol();
      if(!HAS_PERM(PERM_SYSOP))
        prints("[1m[33;46m  �i �� �� �� �j                       \
[44m�� �ĤH�G %8s [40m���d���A�G %s [m",m.name,health[j]);
      else
        prints("[1m[33;46m  �i �� �� �� �j          \
[44m�� �ĤH�G %8s [40m�ͩR�I�ơG%d/%d [m",m.name,m.hp,m.maxhp);
            move(14,0);
      prints("�i�ۤv�����A�j
 ������������������������������������������������������������������������������
 ���i��  �O�j %-4d / %4d  �i�k  �O�j %-4d / %4d  �� �i�˳ƪZ���j%-8s   ��
 ���i�����O�j �@�� %4d  �]�k %4d                 �� �i�g �� �ȡj%-10d ��
 ���i���m�O�j �@�� %4d  �]�k %4d                 �� �i���W�{���j%-10d ��
 ������������������������������������������������������������������������������
      ",rpguser.hp,MHP,rpguser.mp,MMP,"�ŵL�@��",
        Att,MA,cuser.exp,Def,MD,cuser.silvermoney);
      sprintf(buf,"[1m[46;33m\
 (1)[37m�@����� [33m(2)[37m�ϥ��]�k [33m(3)[37m�W�j���m\
 [33m(4)[37m�ϥιD�� [33m(5)[37m�뭰 [33m(6)[37m�k�] [33m(7)[37m�ϥΪ̸�� [m");
      move(20,0);
      outs(buf);
      getdata(22,0,"�n������O�H",ans,3,DOECHO,0);

      switch(ans[0])
      {
        default:
          attack = 0;
        break;

        case '!':
          if(HAS_PERM(PERM_SYSOP))
            m.hp -= 10000;
          break;
        case '4':
          pressanykey("�I�u��");
          attack = 0;
          break;

        /* �@����� */
        case '1':
          j = (rpguser.kar*10)/((rand()%rpguser.kar*2)+2);
          if(j > 50)
          {
            sprintf(buf," [1;31m�A�ξ�K�W�_�@���K�K�A�ΤO�� [33m%s [31m�{�L�h�I�I[m ",m.name);
            j = 5;
          }
          else if(j > 35)
          {
            sprintf(buf," [1;31m�A�V�O���b���W�b�b....�b�X�@�Ӷ¤Y�l�A�ΤO�� [33m%s [31m��L�h�I�I[m ",m.name);
            j = 3;
          }
          else if(j > 20)  /* �B��n�ϥX�j���� */
          {
            sprintf(buf," [1;31m�A�E��|���A�V�O�n�F�@�f�� [33m%s [31m�R�h�I�I[m ",m.name);
            j = 2;
          }
          else if(j <= 20)
          {
            sprintf(buf," [1;37m�A�������� [33m%s [37m�� [31m%s [37m���U�h�I�I[m "
            ,m.name,body[rand()%5]);
            /* �üƶ]�������� */
            j = 1;
          }
          move(12,0);
          outs(buf);
          k = (rand()%((Att-m.Def)>=5 ? Att - m.Def : 5))*j;
          sprintf(buf," %s ���� %d �I�C",m.name,k);
          m.hp -= k;
          pressanykey(buf);
          attack = (m.hp > 0 ? 1 : 0);  /* �ĤH���F���|�A���� */
          break;

        /* �ϥ��]�k */
          case '2':
          sprintf(buf,"[1m[33;46m\
 (1)[37m�����]�k [33m(2)[37m�v���]�k [33m(3)[37m�g���]�k[33m\
 (4)[37m�ۤ��]�k [33m(5)[37m�ǰe�]�k [33m(0)[37m�^�W�h���    [m");
          move(20,0);
          prints("%79s"," ");
          move(20,0);
          outs(buf);
          getdata(22,0,"�n������O�H",ans,3,DOECHO,0);
          switch(ans[0])
          {
            default:
              attack = 0;
              break;

            case '1':
              if(rpguser.mp >= 20)
              {
                rpguser.mp -= 20;
                move(12,0);
                prints(" [1;32m�A��� [33m%s [36m�p�p��[32m���s�Ӥ���...[33m%s ť�����ַw�h�F...[m",m.name,m.name);
                k = rand()%((MA-m.MD)>=30 ? MA - m.MD : 30);
                sprintf(buf," %s ���� %d �I�C",m.name,k);
                m.hp -= k;
                attack = (m.hp > 0 ? 1 : 0);  /* �ĤH���F���|�A���� */
              }
              else
              {
                sprintf(buf,"�k�O�����I");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '2':
              if(rpguser.mp >= 30 && rpguser.hp < MHP)
              {
                rpguser.mp -= 30;
                rpguser.hp += MHP/5;
                if(rpguser.hp > MHP)
                  rpguser.hp = MHP;
                move(12,0);
                outs(" [1;32m�A�����D�G�u[33m�������{�ɬӤj�ҽЪv���ڪ��ˤf�A�����I[32m�v  [m");
                sprintf(buf,"��O��_�� %d �I",rpguser.hp);
                attack = 1;
              }
              else
              {
                if(rpguser.hp >= MHP)
                  sprintf(buf,"�A�{�b��O�R�K�A���ݭn�v��");
                if(rpguser.mp < 30)
                  sprintf(buf,"�k�O�����I");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '3':
              if(rpguser.mp >= 30)
              {
                rpguser.mp -= 30;
                move(12,0);
                outs(" [1;32m�q�A��x���ܥX�X��[36m�¦ӿ�[32m�A�A�G�ܤ������W�]�U�h...[m");
                if(Att >= (c_att(cuser.userid)*3/2)) /* �W�� */
                {
                   sprintf(buf,"�A�������O�w�g���ɨ췥���F");
                   attack = 0;  /* �S�ɨ�ĤH������ */
                }
                else
                {
                  Att += rand()%((rpguser.kar*2+rpguser.con*3+rpguser.dex*5)/10);
                  sprintf(buf,"�A��������O���ɨ� %d �I�C",Att);
                  attack = 1;  /* ���ɫ�ĤH�����@�U */
                }
              }
              else
              {
                sprintf(buf,"�k�O�����I");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '4':
              if(rpguser.mp >= 10)
              {
                rpguser.mp -= 10;
                move(12,0);
                prints(" �A�f�������G�A�ܥX�@���ƨg�������A�� %s ���W��L�h",m.name);
                if((rand()%MD) - m.MD > 0)
                {
                  sprintf(buf,"%s �Q���F�I�w�b����@�ʤ]�����...",m.name);
                  l += 3;
                }
                else
                {
                  sprintf(buf,"%s ���X�@�Ӥ���A������������F...",m.name);
                  attack = 1;
                }
              }
              else
              {
                sprintf(buf,"�k�O�����I");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '5':
              if(rpguser.mp >= 50)
              {
                rpguser.mp -= 50;
                move(12,0);
                prints(" [1;32m�A���ݥ����L [33m%s [32m�F�A�j�ۤ@�n�G[31m�c�F�h���I�I  [m",m.name);
                pressanykey(" �A���}���U��M�h�_�@�}�ǭ��A��A�j���F...");
                rpg_guild();
                unlockutmpmode();
                return 0;
              }
              else
                pressanykey("�k�O�����I");
              attack = 0;
              break;
          }
          break;

      /* ���ɨ��m�O */
        case '3':
          move(12,0);
          outs(" [1;33m�A�j�ۡG�u�U�઺�ѯ��A�н礩�گ��_���O�q....�v[m");
          if(Def >= (c_def(cuser.userid)*3/2)) /* �W�� */
          {
            sprintf(buf,"�A�����m�O�w�g���ɨ췥���F");
            attack = 0;  /* �S�ɨ�ĤH������ */
          }
          else
          {
            Def += (rand()%(rpguser.kar*2+rpguser.con*3+rpguser.dex*5))/10;
            sprintf(buf,"�A�����m��O���ɨ� %d �I�C",Def);
            attack = 1;  /* ���ɫ�ĤH�����@�U */
          }
          pressanykey(buf);
          break;

        /* �뭰 */
        case '5':
          j=(rand()%rpguser.kar) - rand()%m.LV;  /* �I�B��a :p */
          move(12,0);
          prints("[1;35m�A���b %s �e�����G�u�ڦ������Ѯ��s�A���U�ǤF�ڤ@�R�a�I�v[m",m.name);
          money = m.LV*100/((rand()%rpguser.kar/5)+1);
          if(j > 0 && cuser.silvermoney >= money)
          {
            sprintf(buf,"%s �M�w��A�@���A���F�A %d �����O�@�O�C",m.name,money);
            demoney(money);
            over = 1;
            attack = 0;
          }
          else
          {
            sprintf(buf,"%s ı�o�٬O��A���F����֡I",m.name);
            attack = 1;
          }
          pressanykey(buf);
          break;

        /* ���հk�] */
        case '6':
          j=(rand()%rpguser.kar) - m.LV;  /* �I�B��a :p */
          move(12,0);
          outs(" [1;32m�A���հk�]....[m");
          if(j > 0)
          {
            money = m.LV*50/((rand()%rpguser.kar)+1);
            sprintf(buf,"�k�]���\\�A���O��W�����F %d ���C",money);
            demoney(money);
            over = 1;
            attack = 0;
          }
          else
          {
            attack = 1;  /* �k�]���ѼĤH�ɤ@�� */
            sprintf(buf,"�k�]���ѡI");
          }
          pressanykey(buf);
          break;

        case '7':
          rpg_uinfo();
          attack = 0;
          break;
      }

      if(l > 0)
      {
        attack = 0;
        sprintf(buf,"%s �Q�w��F�A�٭n %d �^�X�~��ʧ@....",m.name,l--);
        pressanykey(buf);
      }

      /* �ĤH������ */
      if(attack == 1)
      {
        move(12,0);
        prints("%-255s"," ");
        move(12,0);
        prints(" [1;33m%s [31m�����I�I[m",m.name);
        k = rand()%((m.Att-Def) >=5 ? m.Att - Def :5);  /* �ĤH�S�S�� */
        sprintf(buf," �A ���� %d �I�C",k);
        rpguser.hp -= k;
        if(rpguser.hp < 0) rpguser.hp = 0; // �t���_�u��|�[�^��?
        rpg_rec(cuser.userid, rpguser);
        substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
        /* �קKUser�_�u�S������ */
        pressanykey(buf);
        move(12,0);
        prints("                                                                               ");
      }

      /* �ĤH���F */
      if(m.hp <= 0)
      {
        money = m.money + rand()%(20*m.LV);
        exp = m.EXP + rand()%(10*m.LV);
        sprintf(buf,"�ש�Ĺ�F�I�I�o�� %d �I�g��ȡA�H�� %d ��",exp,money);
        pressanykey(buf);
        inmoney(money);
        inexp(exp);
        over = 1;  /* �j�鵲�� */
      }

      /* �ĤH�k�] */
      if((m.hp*100)/m.maxhp < 3 && rand()%10 < 3 && m.hp > 0)
      {
        money = (rand()%m.money)+20;  /* �ĤH�k�]�Ȩ쪺����exp����� */
        exp = (rand()%m.EXP)+10;
        sprintf(buf,"%s �����L�A�k���F�A�o�� %d �g��� %d ��",m.name,exp,money);
        pressanykey(buf);
        inmoney(money);
        inexp(exp);
        over = 1;  /* �j�鵲�� */
      }
      /* �}�`�o */
      if(rpguser.hp <= 0)
      {
        money = (rand()%(m.LV*100))+100;
        exp = (rand()%(m.LV*m.LV*10)) + 100;
        sprintf(buf,"�����}�`�F�I�I�l��%d�I�g���,%d��",exp,money);
        pressanykey(buf);
        demoney(money);
        deexp(exp);
        rpguser.hp = 0;   /* hp���|�ܭt�� */
        over = 1;       /* �j�鵲�� */
      }
    }
    substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
    rpg_rec(cuser.userid, rpguser);
    return 0;
  }
  return 0;
}


void
rpg_train()
{
  char buf[3];
  int mode;

  if(check_money(10*rpguser.level,SILVER)) return;
  demoney(10*rpguser.level);
  stand_title("�V�m��");
  show_file(BBSHOME"/game/rpg/Welcome_train", 2, 15, ONLY_COLOR);
  getdata(18,0,"�ߦn��� 1.�ۭh�ɦV 2.�V�O���� 3.���P�ۦp ",buf,2,DOECHO,0);
  if(buf[0] == '1') mode = hard;
  else if(buf[0] == '2') mode = medium;
  else if(buf[0] == '3') mode = easy;
  else return;
  do{
    rpg_kmob(mode);
    buf[0] = answer("�O�_�~��H (y/N)");
  }while(buf[0] == 'y');
  return;
}

/* �ק��� */

int
rpg_uquery(char *userid)
{
  char buf[80];
  rpgrec rpg;
  int u;

  if(u = getuser(userid))
    memcpy(&rpg , &rpgtmp, sizeof(rpgrec));
  else return RC_NONE;

  getdata(23,0,"�O�_�n�ק���(Y/N)�H  [N]",buf,3,DOECHO,"N");
  if(buf[0] == 'y')
  {
    clear();
    sprintf(buf,"%i",rpg.race);
    getdata(0,0,"¾�~�G (1)����j�H (2)Ū��֤� (3)�`�n�{��\n"
                "       (4)��Ѳr�N (5)���y�a�x (6)�C���g�P ",
    buf,3,LCECHO,buf);
      if(buf[0] < '7' && buf[0] > '0')
        rpg.race= atoi(buf);
    sprintf(buf,"%i",rpg.level);
    getdata(2,0,"��J���šG",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.level = atol(buf);
    sprintf(buf,"%i",rpg.hp);
    getdata(3,0,"��O�G",buf,8,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.hp = atol(buf);
    sprintf(buf,"%i",rpg.mp);
    getdata(3,40,"�k�O�G",buf,8,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.mp = atol(buf);
    sprintf(buf,"%i",rpg.str);
    getdata(5,0,"�O�q�G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.str = atol(buf);
  sprintf(buf,"%i",rpg.wis);
  getdata(5,40,"���z�G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.wis = atol(buf);
  sprintf(buf,"%i",rpg.dex);
  getdata(6,0,"�ӱ��G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.dex = atol(buf);
  sprintf(buf,"%i",rpg.con);
  getdata(6,40,"���G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.con = atol(buf);
  sprintf(buf,"%i",rpg.kar);
  getdata(7,0,"�B��G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.kar = atol(buf);
  sprintf(buf,"%i",rpg.weapon);
  getdata(9,0,"�Z���N�X�G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.weapon = atol(buf);
  sprintf(buf,"%i",rpg.armor);
  getdata(10,0,"����N�X�G",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.armor=atol(buf);
  sprintf(buf,"%i",rpg.age);
  getdata(11,0,"�~�֡G",buf,9,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.age=atol(buf);
  getdata(20, 0, msg_sure_yn, buf, 3, LCECHO,"y");
    if (buf[0] != 'y')
    {
      pressanykey("���ק�");
    }
    else
    {
      log_usies("SetRPG", rpg.userid);
      rpg_rec(userid, rpg);
      pressanykey("�ק粒��");
    }
  }
  return 0;
}

/* �s��User��� */
rpg_edit()
{
  int id;
  char genbuf[200];
  stand_title("�ק���");
  usercomplete(msg_uid,genbuf);
  if(*genbuf)
  {
    move(2,0);
    if (id = getuser(genbuf))
    {
      rpg_udisplay(genbuf);
      rpg_uquery(genbuf);
    }
    else
    {
      pressanykey(err_uid);
    }
  }
  return 0;
}

/* PK */

int
rpg_pk(fd,enemyid)
  int fd;
  char enemyid[IDLEN+1];
{
  rpgrec enemy;
  userec euser;
  int MHP = rpguser.con*30,MMP= rpguser.wis*10;  /* �ۤv���̤jmp,hp */
  int Att = c_att(cuser.userid);  /* �����O */
  int Def = c_def(cuser.userid);  /* ���m�O */
  int MA = c_ma(cuser.userid);    /* �]�k���� */
  int MD = c_md(cuser.userid);    /* �]�k���m */
  int Emhp,Emmp,Eatt=c_att(enemyid) ,Edef= c_def(enemyid)
      ,Ema=c_ma(enemyid) ,Emd=c_md(enemyid) ;
/*  int money,exp; */ /* ������|�W�[or��֪�$$��exp */
  char buf[256]= "\0"; /* ans��user input��,buf���ù�output�� */
  int j=0,k=0,/*l=0,*/over=0,attack=0; /* over�����԰�,attack�ĤH���� */
  int ans;
  user_info *my = currutmp;

  getuser(enemyid);
  enemy = rpgtmp;
  euser = xuser;
  Emhp = enemy.con*30; Emmp = enemy.wis*10;
  add_io(fd, 0);

  setutmpmode(RPK);
  log_usies("RPG_PK",NULL);

  for(;;)
  {
    if(over == 1)
    {
      add_io(0, 0);
      close(fd);
      break;
    }
    else
    {
      update_data();
      clear();
      if(!HAS_PERM(PERM_SYSOP))
      {
        sethomefile(buf, enemyid, "picture");
        show_file(buf,1,10,ONLY_COLOR);  /* �ĤH�Ϥ� */
      }
      else
      {
        move(1,0);
        prints(
"���� : %d\n�ͩR : %d\n�� : %d / %d\n�]�k : %d / %d\n�g�� : %d\n�ȹ� : %d"
,enemy.level,Emhp,Eatt,Edef,Ema,Emd,euser.exp,euser.silvermoney);
      }
      move(11,0);
      prints("[36m%s[m",msg_seperator);  /* ���j�u */
      move(13,0);
      prints("[36m%s[m",msg_seperator);  /* ���j�u */
      over = 0;
      while(over == 0)  /* �԰�...���� over */
      {
        /* ����user���D�Ǫ����h�֦�A�ҥH�Ψ��骬�A��� */
        j = ((enemy.hp*100)/Emhp >= 100 ? 0 :
            (enemy.hp*100)/Emhp >= 75  ? 1 :
            (enemy.hp*100)/Emhp >= 50 ? 2 :
            (enemy.hp*100)/Emhp >= 25 ? 3 : 4);
        move(0,0);
        clrtoeol();
        if(!HAS_PERM(PERM_SYSOP))
          prints("[1m[33;46m  �i �� �� �� �j                       \
[44m�� �ĤH�G %8s [40m���d���A�G %s [m",euser.userid,health[j]);
        else
          prints("[1m[33;46m  �i �� �� �� �j          \
[44m�� �ĤH�G %8s [40m�ͩR�I�ơG%d/%d [m",euser.userid,enemy.hp,Emhp);
        move(14,0);
        prints("�i�ۤv�����A�j
 ������������������������������������������������������������������������������
 ���i��  �O�j %-4d / %4d  �i�k  �O�j %-4d / %4d  �� �i�˳ƪZ���j%-8s   ��
 ���i�����O�j �@�� %4d  �]�k %4d                 �� �i�g �� �ȡj%-10d ��
 ���i���m�O�j �@�� %4d  �]�k %4d                 �� �i���W�{���j%-10d ��
 ������������������������������������������������������������������������������
        ",rpguser.hp,MHP,rpguser.mp,MMP,"�ŵL�@��",
          Att,MA,cuser.exp,Def,MD,cuser.silvermoney);
        sprintf(buf,"[1m[46;33m (1)[37m�@�����");
        move(20,0);
        outs(buf);
        move(23,0);
        outs(my->turn ? "����A������!" : "��������...");

        if(my->turn)
        {
          move(22,0);
          outs("�n������O�H");
          ans=igetch();
          switch(ans)
          {
            default:
              attack = 0;
              pressanykey("�I�u��");
              break;
            /* �@����� */
            case '1':
              sprintf(buf," [1;37m�A�������� [33m%s [37m�� [31m%s [37m���U�h�I�I[m "
              ,euser.userid,body[rand()%5]);
              /* �üƶ]�������� */
              move(12,0);
              outs(buf);
              k = (rand()%((Att-Edef)>=5 ? Att - Edef : 5))*j;
              sprintf(buf," %s ���� %d �I�C",euser.userid,k);
              enemy.hp -= k;
              pressanykey(buf);
              my->turn = -1;
              send(fd, &my, sizeof(user_info), 0);
              my->turn = 0;
              attack = (enemy.hp > 0 ? 1 : 0);  /* �ĤH���F���|�A���� */
              break;
            case 'q':
              over = 1;  /* �j�鵲�� */
              break;
          }
        }
        do{
          if(ans == I_OTHERDATA)
          {
            recv(fd, &my, sizeof(user_info), 0);
            if(my->turn == -1) my->turn = 1;
          }
        }while(!my->turn);
      }
    }
  }
  return 0;
}
