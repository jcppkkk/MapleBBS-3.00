
�C�C�ֳzñ�䯸 0.2�� Ū����

FreeBSD 4.5-STABLE , WD_BBS_001201 tested ok!

written by Worren

 0.1��  3/19
 0.2��  4/9
                                        worren@worren.2y.net
                                        worren@77bbs.com
                                        �{�Ҥj�ǤC��C�鴸BBS 77bbs.com


(a)install����k:

1.��Ҧ����ɮױq/home/bbs�����Y�b/home/bbs/src/lotto, �ð���install.sh
2.�bbbs���ؿ����s�W(menu.c):

  "lotto/lotto.so:menu_lotto",0,
    "66Lotto     ^^ �C�C�ֳzñ�䯸         �̧C���O $1g",1,
  ��make update

3.copy post_open.c�� src/util/statool
  �s��Makefile, �s�W�@��post_open
TOOL =  account poststat birth gnp horoscope topmsg voteboard\
        topquery yearsold toppost toptime toplogin topexp topfive\
        topmoney topboard topgold sexcount toplimit toprpg\
        admin bm_check toppersonal post_open

  cd ~/src/util/statool
  make install��

4.edit lotto.h

��h�W�u�n�ιw�]�ȴN�i�H�F! �Y�n�ק�߲v, �i�����b�o���ɭק�:
���Y�O�_�Ȧ��ҧ���, �]�����n��ۭק�o����:

#define LOTTO_NUM_MAX 42        //�}���d��W��
#define LOTTO_NUM_MIN 1         //�}���d��U��
#define LOTTO_AMOUNT_MAX 2000   //�C�ӤH�i�H�U�`���̰���
#define LOTTO_OPEN_WDAY "2,5"   //�@�g�̦����X�Ѷ}��, �P���鬰"0"
#define LOTTO_OPEN_WTIME 2      //�@�g���}������(�n�t�X�W��)
#define LOTTO_OPEN_BEGIN "1945" //����ñ��ɶ��d��(begin), �榡:hhmm, 24�p�ɨ�
#define LOTTO_OPEN_END "2130"	//(end)
#define LOTTO_ANTE_MIN 1        //�̤p���

/* �H�U���߲v, �����ۦ�ק� */
#define LOTTO_RATE_1 800000             //1���߲v(6�X)
#define LOTTO_RATE_2 100000             //2���߲v(5�X+�S�O��)
#define LOTTO_RATE_3 4000               //3���߲v(5�X)
#define LOTTO_RATE_4 90                 //4���߲v(4�X)
#define LOTTO_RATE_5 6                  //5���߲v(3�X)
#define LOTTO_RATE_6 1                  //6���߲v(2�X)

���߲v�O�̬��έp�L�̦X�z���߲v,���I�C(BBS�Ȧ�í�Ȥ���), 
(ftp://77bbs.com/bbs/77lotto.xls)
��M�z�]�i�H�ۦ洣��!���n�`�N, �Y�O���Ӱ��|�ް_BBS�t�Ϊ��q�f����!
�����߲v�i�l��user�Ӫ�

5.~/src/lotto/> make clean install

6.crontab ��:

# �C�C�ֳz�����{��
# �C�P���G���ߤW8:20�]get_lotto_now
20 20 * * 2,5 lotto/get_lotto_now.sh > /dev/null 2>&1
# �C�P���G���ߤW8:30�]lotto_open�}���{��
30 20 * * 2,5 lotto/lotto_open.sh > /dev/null 2>&1
# ��}���O����WLotto.
30 21 * * 2,5 bin/post_open > /dev/null 2>&1

7. �bbbs�t�ΤW�}�@��"Lotto."�O, ���}���βέp���Gpost�W�h!

   /* ��lotto���O���ˤWLotto.�� */

         keeplog("lotto/lotto_open.txt", "WHAT_YOU_DEFINE", "[�C�C�ֳz���}��] �����W��");
         keeplog("lotto/lotto_stat.txt", "WHAT_YOU_DEFINE", "[�C�C�ֳz���}��] �}���έp");
         exit(0);

   �o���ɭncopy�� ~/src/util/statool��, �æb�Ӹ��|�U�Amake�@��


8. �Ĥ@�������, �n��ʧ���e�����}�����X, �Hbbsadm������
   run  ~/lotto/get_lotto_now.sh, �_�h�|�X�{"�ثe�Ȥ��}��"���r��
   �Y�H���M�Q�n�����\��, �u�n��~/lotto/lotto_no_now.txt�屼
   �N�i�H�F! or �H��X�{"�ثe�Ȥ��}��"���r��, �N�O�o���ɤ����F,
   �]�Orun �@��get_lotto_now.sh�N�i�H�F! 
   ps:�}����|�ۤv��@��!

9. edit ~/lotto/lotto_rule.txt 
   �o�O�i�H���b�����q���᪺��, �z�i�H�g�Ǳz�Q�g�F��..
   �p�C���W�h(default)..

10.��������D�q�Ц�bbs://77bbs.com  Worren. �O���X!

11.���±z���ϥ�



(b)������� -> for advence

All are from WD_BBS 

�������
prints() 	:����printf()
move(x,y)	:���в��� x,y
pressanykey(msg):�b�̩��U�X�{msg�� "���������~��..."
getdata()	:����basic��locate + input ����
clear()		:����basic cls
stampfile(); +rec_add();+ update_data(); �H�H��user(ñ�䵲�G,�O��)


���I������/�ܼ�:
cuser.goldmoney :user������ (from struct.h)
cuser.userid	:���ouser id
degold(int);	:�Nuser�������

