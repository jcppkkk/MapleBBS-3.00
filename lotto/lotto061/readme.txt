���v�ŧi

77 Lotto 0.6.1��

���{����h�W�}�񵹤j�a�b�D��Q�����p�U�i�H���N�ק�ƻs,
�Y�n�H����Φ��������{���Чi���@��.

�p�̤~���ǲL, �{������������~���a��, ���ЦU����i���[���!

��󥻵{����������D�w��� telnet://77bbs.com/Worren. �Q��;

�s���U���I:
ftp://77bbs.com/bbs/lotto_src_0.6.tgz
http://worren.panfamily.org/works/program

Version 0.1 	  2002-3-19 
	0.2 	  2002-4-9
	0.2.1 	  2002-4-14
        0.3 	  2002-7-9
	0.5 	  2002-8-14
	0.5.2 	  2002-8-17
        0.6 beta1 2002-8-19
        0.6 beta2 2002-8-21
        0.6 finalbeta 2002-8-22
        0.6.1     2003-2-28

                                        �{�Ҥj�ǤC��C�鴸BBS
                                        Worren Feung �����
                                        worren.feung@msa.hinet.net
                                        worren@77bbs.com
					����: http://worren.77bbs.com



�C�C�ֳzñ�䯸Ū����

install����k:

1.��Ҧ����ɮױq/home/bbs�����Y�b/home/bbs/src/lotto, 

2.�� src/util/statool
  �s��Makefile, �s�W�@��post_open
TOOL =  account poststat birth gnp horoscope topmsg voteboard\
        topquery yearsold toppost toptime toplogin topexp topfive\
        topmoney topboard topgold sexcount toplimit toprpg\
        admin bm_check toppersonal post_open


3.�bbbs���ؿ����s�W(menu.c):

  "lotto/lotto.so:menu_lotto",0,
    "66Lotto     ^^ �C�C�ֳzñ�䯸         �̧C���O $1g",1,
  ��make update

  ��M, �Y�z�Ʊ�O�H�ȹ����O, �i�H�� $1g �令 $1s or other else

4.edit lotto.h

��h�W�u�n�ιw�]�ȴN�i�H�F! �Y�n�ק�߲v, �i�����b�o���ɭק�:
���Y�O�_�Ȧ��ҧ���, �]�����n��ۭק�o����:
(4����}�l�}����n���n��...�n��ۧ�2,5�令3,6; �P���鬰"0")


#define LOTTO_NUM_MAX 42        //�}���d��W��
#define LOTTO_NUM_MIN 1         //�}���d��U��
#define LOTTO_AMOUNT_MAX 2000   //�C�ӤH�i�H�U�`���̰���
#define LOTTO_OPEN_WDAY "2,5"   //�@�g�̦����X�Ѷ}��
#define LOTTO_OPEN_WTIME 2      //�@�g���}������(�n�t�X�W��)
/*
�U�����U�����W�Ҥ� cpu���t�צөw,����ñ��ɶ��N�O����lotto_open�ɶ�
77�D����CPU��k6-2-400,128MB;�}101.x�U����Ʊq20:45�]��j�Ѫ�3:55am
�Y�]�W�L0:00,�h�������{���w���Ũϥ�,�Ч�Hcrontab���� lotto_close.sh
��lotto_run.sh
*/
#define LOTTO_OPEN_BEGIN "1915" //����ñ��ɶ��d��(begin), �榡:hhmm, 24�p�ɨ�
#define LOTTO_OPEN_END "2230"	//(end)
#define LOTTO_ANTE_MIN 1        //�̤p���
#define LOTTO_GOLD_SILVER 0	//����Ϊ���=0 or �ȹ�=1��

/* �H�U���߲v, �����ۦ�ק� */
#define LOTTO_RATE_1 800000             //1���߲v(6�X)
#define LOTTO_RATE_2 100000             //2���߲v(5�X+�S�O��)
#define LOTTO_RATE_3 4000               //3���߲v(5�X)
#define LOTTO_RATE_4 90                 //4���߲v(4�X)
#define LOTTO_RATE_5 6                  //5���߲v(3�X)
#define LOTTO_RATE_6 2                  //6���߲v(2�X)

���߲v�O�̬��έp�L�̦X�z���߲v,���I�C(BBS�Ȧ�í�Ȥ���), 
(ftp://77bbs.com/bbs/77lotto.xls)
��M�z�]�i�H�ۦ洣��!���n�`�N, �Y�O���Ӱ��|�ް_BBS�t�Ϊ��q�f����!
�����߲v�i�l��user�Ӫ�

5.�^�� /home/bbs/src/lotto, �ð���install.sh

6.crontab ��:

# �C�C�ֳz�����{��
# �C�P���G���ߤW8:30�]get_lotto_now, �j��9:00�A�]�@��
30 20 * * 2,5 lotto/get_lotto_now.sh > /dev/null 2>&1
# �C�P���G���ߤW8:45�]lotto_open�}���{��
45 20 * * 2,5 lotto/lotto_open.sh > /dev/null 2>&1
# ��}���O����WLotto.

# �Y�Q�����t�εL�k�b�ɶ����}����(run��lotto_open) �Ч⦹�ɶ�����!
# �q0.6����, ��lotto_open�b�̾A�X���ɭԨӱҰʳo�ӵ{��
#45 22 * * 2,5 bin/post_open > /dev/null 2>&1

# �q0.6���H��, �b�Ұ�lotto_open��|�۰�run lotto_close.sh,
# �æb�̾A���ɭ�run lotto_run.sh
# �Ylotto_open�]���w�n�W�L12�I, �Х��γo��汱����lotto & �} lotto�����ɶ�
# �N lotto_open�b�]����, lotto���N����}!!
# �C�P���G���ߤW7:20��, �j�Ѧ��W6�I�}
#20 19 * * 2,5 lotto/lotto_close.sh > /dev/null 2>&1
#0 6 * * 3,6 lotto/lotto_run.sh > /dev/null 2>&1

7. �bbbs�t�ΤW�}�@��"Lotto."�O, ���}���βέp���Gpost�W�h!

8. �Ĥ@�������, �n��ʧ���e�����}�����X, �Hbbsadm������
   run  ~/lotto/get_lotto_now.sh, �_�h�|�X�{"�ثe�Ȥ��}��"���r��
   �Y�H���M�Q�n�����\��, �u�n��~/lotto/lotto_no_now.txt�屼
   �N�i�H�F! or �H��X�{"�ثe�Ȥ��}��"���r��, �N�O�o���ɤ����F,
   �]�Orun �@��get_lotto_now.sh�N�i�H�F! 
   ps:�}����|�ۤv��@��!

9. edit ~/lotto/lotto_rule.txt 
   �o�O�i�H���b�����q���᪺��, �z�i�H�g�Ǳz�Q�g�F��..
   �p�C���W�h..

10.~/lotto/lotto_no_history.txt�O�έp���̾���,
   �C�C�D�����q�ֳz�Ĥ@���}�l�O��,
   ���Q�ۤv�q�Ĥ@���ۤv����, �i�H�ۦ��77��d/l
   ftp://77bbs.com/bbs/lotto_no_history.txt
   �C���}����|�P�B��s!

11.��������D�q�Ц�bbs://77bbs.com  Worren. �O���X���!



			+------------------+
			|	NOTE	   |
			+------------------+

77 Lotto 

0.3��

  a)what's files updated?
	lotto.c , lotto_open.c , lotto_h
  b)what's new?
    1.�s�W�i��ܥΪ����λȹ��U�`
    2.�s�W�@�Ӽֳz���}��:
      �b~/lotto/lotto_on_off.txt,�o���ɪ����e�Y��on���N�O�}��!
      �Y�O�S�o���� or Ū�쪺���Oon, ���N����!
  c)who needs?
    1.�Q�n�q�쥻�������令�ȹ�������! -->�@�w�n��update�L����over write�L!
    2.�Q�n���o��lotto���Q���N��,�Q�}�N�}������!
      (��K�޲z��~...)
      �Y�z��Lotto���w�g�ק令�z�Q�n���T����r, ���S�Q�n�o�ӥ\��,
      �i�H���H�U���ʰ��]�i�H�F��:
      2.1:�Ѧҷs��src,�blotto.h�̷s�W�@��LOTTO_ON_OFF 
      2.2:�Ѧҷs��src,�blotto.c��lotto_menu() ���[�J�U���@�q:
/*
Ū�� lotto_on_off.txt
if�p�GŪ�o�� lotto_on_off.txt �B���e�� "on" ���N�O�ֳz����~��
�YŪ���� lotto_on_off.txt, ���N�O������
*/

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
   fp = fopen(buf,"r");
   if(fp == NULL) {
   pressanykey("sorry, �t�ξ��, �ثe�Ȥ��}��");
        fclose(fp);
        return 0;
   }

   strcpy(buf,"");
   fgets(buf,20,fp);
   fclose(fp);
   if(strncmp(buf,"on",2)!=0) {
      pressanykey("sorry, �t�ξ��, �ثe�Ȥ��}��....");
      return ;
   }

//�P�_�O�_�}�񤤵���

  d)���Ѥp���Z���A:
    �i�H�bLotto.������ذϤ�,�}�@��"���v�}���O��"����, 
    �A��/home/bbs/man/boards/ �H link���覡�s�� 
    /home/bbs/lotto/lotto_no_history.txt
    �۫H�w�g�����֤H�o�򰵤F!


0.5��

  a) what's files updated?
     lotto.c
  b) what's new?
     1.�s�W�]�P�︹
       ����o�{���o��g�藍��....
  c) �ץ��q�H�e��{�breadme.txt �M install.sh ���p���~.

0.5.1��

  a)�ѨM�]�P�︹�ɿ�13�ӥH�W�����X�h�ƦC��, �ƦC�ƭp�⪺���~!(update lotto.c)

0.5.2��

  a)�ѨM���ƶq�Ӥj��, ���@�ǲέp�ƾڨS�k��show�o���`(update lotto_open.c)
  b)�ץ������ɰ���install.sh���ɾ�.
  c)�s�WFAQ�X�Ӷ���.
  d)�ѨM��lotto_record.txt����ƤӤj��, lotto_open�]�����ҳy�������D!
    (������+crontab+*.sh,�Ԭ� FAQ)
  e)�ץ��]�P�������q����Τ��G���S���[�W"�]�P"�r��!

0.6��

what update?
  lotto.h lotto.c lotto_open.c crontab�Ƶ{ FAQ.txt

what's new?
  a)�s�W��user�ۤv�]�w�n���n�H�����q��.
    �åi��sysop�blotto.h���]�w�w�]��. �ڹw�]6�����H,�䥦���H!
  b)�s�W�@�ӦU�H�Ӵ��U���U�`���ƾڲέp & �H���user�H�c
  c)��i�}����������/�}LOTTO������.
    c-1:lotto.h�̳]�w�ɶ�����user�i��,�H��lotto_open�brun�F�٦�user�b
        ñ�䯸��.(0.1���\��)
    c-2:lotto_open�Ұʮɷ|��lotto_on_off.txt���e�]��off,
        ���}���j���]����N�|��^on. �A�~�򰵭ӤH�έp.
  d)��i�p��C�@��user�U�`�ƪ���k, �H�ɮרt�ΨӥN���j���p��.

note:
  a)�ѩ�ӤH�έp���|�e��lotto�}���ɶ�, so�ӤH�έp�q���ȳ]��"�@�w�|�H"
    user�]�L�k�令"���H". �N��sysop��ʧ�F, �]�S���P�_�n���n�H���{�լq.

  ****�S�O�`�N****
  b)�Х�0.6-beta�H�eupgrade ���������ͪ`�N! �ѩ�p��C�@��user�U�`�ƪ�
    �{���ξ�����j�T�׭ק�, so�Щ�̪�o�@���}������, �buser�٨S���U�`
    ���e�o�q������upgrade, �H�K������T�a�O���C��user���U�`��!

0.6.1��

what's update?
  get_lotto_now.c

note:
  ��09216���_, �_�Ⱥ������G�}�����榡���F�ܰ�, �Gget_lotto_now����
  �n�ץ�! ���L�p�̧�ı�o�H�e���M�{�b���榡�bexplorer�U�ݰ_�ӳ��@��!
  �u�O��lynx dump�X�Ӫ��ɴN�t�ܦh�F! ���L�]���i��|�b���ӫ�_���
  ���Ҽ�. so�p�̯S�O���Ӫ�get_lotto_now.c�d�bpackage��, ��W��
  get_lotto_now_0.6.c �H��Y�����n, �A�ЦU�쯸���ϥ��ª�code.

what's next?
    �b�]�w�O�_�n�H�q�����{���q��, ���ϥθ�Ʈw��lock����,
    ���e���S��! �p���|�[�J�U������Ʋ��ʪ��{����.

