#define LOTTO_PATH "/home/bbs/"			//bbs�t�Ϊ����|
#define LOTTO_NO_NOW_FILE "lotto/lotto_no_now.txt"
#define LOTTO_ON_OFF_FILE "lotto/lotto_on_off.txt"
#define LOTTO_RECORD "lotto/lotto_record.txt"
#define LOTTO_NO_HISTORY "lotto/lotto_no_history.txt"
#define LOTTO_RULE "lotto/lotto_rule.txt"
#define LOTTO_OPEN "lotto/lotto_open.txt"	//�����W��
#define LOTTO_MAIL_DEFINE "lotto/lotto_mail_define.txt"	//�����q��user�۩w�q��
#define LOTTO_NUM_MAX 42	//�}���d��W��
#define LOTTO_NUM_MIN 1		//�}���d��U��
#define LOTTO_AMOUNT_MAX 30000   //�C�ӤH�i�H�U�`���̰���
#define LOTTO_OPEN_WDAY "2,5"	//�@�g�̦����X�Ѷ}��
#define LOTTO_OPEN_WTIME 2      //�@�g���}������(�n�t�X�W��)
/*
�U�����U�����W�Ҥ� cpu���t�צөw,����ñ��ɶ��N�O����lotto_open�ɶ�
77�D����CPU��k6-2-400,128MB;�}101.x�U����Ʊq20:45�]��j�Ѫ�3:55am
�Y�]�W�L0:00,�h�������{���w���Ũϥ�,�Ч�Hcrontab���� lotto_close.sh
��lotto_run.sh
*/
#define LOTTO_OPEN_BEGIN "1915"	//����ñ��ɶ��d��(begin), �榡:hhmm, 24�p�ɨ�
#define LOTTO_OPEN_END "2230"
#define LOTTO_ANTE_MIN 1	//�̤p���
#define LOTTO_GOLD_SILVER 0	//�� ����=0 ; �ȹ�=1


/* �H�U���߲v, �����ۦ�ק� */
#define LOTTO_RATE_1 1000000		//1���߲v(6�X)
#define LOTTO_RATE_2 20000		//2���߲v(5�X+�S�O��)
#define LOTTO_RATE_3 4000		//3���߲v(5�X)
#define LOTTO_RATE_4 90			//4���߲v(4�X)
#define LOTTO_RATE_5 10			//5���߲v(3�X)
#define LOTTO_RATE_6 2			//6���߲v(2�X)

/* �����q���w�]�� 0=���H 1=�H*/ 
#define MAIL_S 1	//user�ӤH���ñ��έp
#define MAIL_1 1	//�Y��
#define MAIL_2 1
#define MAIL_3 1
#define MAIL_4 1
#define MAIL_5 0
#define MAIL_6 0	//����
