#include "bbs.h"
#define ba rpguser.race == 6 ? 10*rpguser.level : 5

/*
  �{���]�p�Gwsyfish
  �ۤv���y�G�g�o����A�üg�@�q�A�Sԣ�`��:)
  �ۮe�{�סGPtt�O�����ӳ���a�A�N��inmoney�Mdemoney�A��L�pSob�N�n��@�U�o
  ���a�@���G�o�{���̭����A��A����Ȥ��O 0
*/
char *dice[6][3] = {"        ",
                    "   ��   ",
                    "        ",
                    "   ��   ",
                    "        ",
                    "   ��   ",
                    "   ��   ",
                    "   ��   ",
                    "   ��   ",
                    "��    ��",
                    "        ",
                    "��    ��",
                    "��    ��",
                    "   ��   ",
                    "��    ��",
                    "��    ��",
                    "��    ��",
                    "��    ��"
};

int
x_dice()
{
  char choice[11],buf[60];
  int  i, money; 
  char tmpchar;    /* �����ﶵ */
  char tmpdice[3]; /* �T�ӻ�l���� */
  char totaldice;
  time_t now = time(0);

  time(&now);

  setutmpmode(DICE);
  while(1)
  {
    stand_title("���� �t�� �{�� ���");
    getdata(1, 0, "�аݭn�U�`�h�֩O�H(�̤� 1 �� /�̦h 250000 ��) ",
            choice, 7, LCECHO, 0);
    money = atoi(choice);
    if(money < 1 || money > 250000 || money > cuser.silvermoney)
    {
      pressanykey("�U�`���B��J���~�I���}���");
      break;
    }
    outs("\n�z�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�{\n"
         "�x ����  1. �j      2. �p                                                 �x\n"
         "�x ����  3. �T�I    4. �|�I     5. ���I    6. ���I    7. �C�I    8. �K�I  �x\n"
         "�x       9. �E�I   10. �Q�I    11. �Q�@�I 12. �Q�G�I 13. �Q�T�I 14. �Q�|�I�x\n"
         "�x      15. �Q���I 16. �Q���I  17. �Q�C�I 18. �Q�K�I                      �x\n"
         "�x ���� 19. �@�@�@ 20. �G�G�G  21. �T�T�T 22. �|�|�| 23. ������ 24. �������x\n"
         "�|�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�}\n");
    getdata(11, 0, "�n����@���O�H(�п�J���X) ", choice, 3, LCECHO, 0);
    tmpchar = atoi(choice);
    if(tmpchar <= 0 || tmpchar > 24)
    {
      pressanykey("�n�㪺���ؿ�J���~�I���}���");
      break;
    }
    demoney(money);
    outs("\n�����@���Y�X��l....\n");
    igetkey();

    do
    {
      totaldice = 0;
      for(i = 0; i < 3; i++)
      {
        tmpdice[i] = rand() % 6 + 1;
        totaldice += tmpdice[i];
      }

      if (((tmpchar == 1) && totaldice > 10) ||
          ((tmpchar == 2) && totaldice <= 10))
      {
        if ((rand() % 10) < 6) /* �@���ΡA�����v����Ӥ�60% */
          break;
      }
      else
        break;

    }while(tmpchar <= 2);

    outs("\n�~�w�w�w�w���~�w�w�w�w���~�w�w�w�w��\n");

    for(i = 0; i < 3; i++)
      prints("�x%s�x�x%s�x�x%s�x\n",
             dice[tmpdice[0] - 1][i],
             dice[tmpdice[1] - 1][i],
             dice[tmpdice[2] - 1][i]);

    outs("���w�w�w�w�����w�w�w�w�����w�w�w�w��\n\n");

    if((tmpchar == 1 && totaldice > 10)
       || (tmpchar == 2 && totaldice <= 10)) /* �B�z�j�p */
    {
//      sprintf(buf,"���F�I�o�좱������ %d ���A�g��� %d �I",
//              money * 2,ba*2);
      sprintf(buf,"���F�I�o�좱������ %d ��",money * 2);
      inmoney(money*2); 
//      inexp(ba*2);
    } 
    else if(tmpchar <= 18 && totaldice == tmpchar) /* �B�z�`�M */
    {
//      sprintf(buf,"���F�I�o�좴������ %d ���A�g��� %d �I",
//              money * 5, ba*5); 
      sprintf(buf,"���F�I�o�좴������ %d ��",money * 5);
      inmoney(money * 5);
//      inexp(ba*5);
    } 
    else if((tmpchar - 18) == tmpdice[0] && (tmpdice[0] == tmpdice[1])
            && (tmpdice[1] == tmpdice[2])) /* �B�z�T�Ӥ@���`�M */
    {
//      sprintf(buf,"���F�I�o�좸������ %d ���A�g��� %d �I",
//            money * 9, ba*9);
      sprintf(buf,"���F�I�o�좸������ %d ��",money * 9);
      inmoney(money * 9);
//      inexp(ba*9);
    } 

    else /* �B�z�S�� */
      sprintf(buf,"�ܥi���S���㤤�I");
    pressanykey(buf);
    game_log(DICE,"%s",buf); 
  }
  return 0;
}
