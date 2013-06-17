#define LOTTO_PATH "/home/bbs/"			//bbs系統的路徑
#define LOTTO_NO_NOW_FILE "lotto/lotto_no_now.txt"
#define LOTTO_ON_OFF_FILE "lotto/lotto_on_off.txt"
#define LOTTO_RECORD "lotto/lotto_record.txt"
#define LOTTO_NO_HISTORY "lotto/lotto_no_history.txt"
#define LOTTO_RULE "lotto/lotto_rule.txt"
#define LOTTO_OPEN "lotto/lotto_open.txt"	//中獎名單
#define LOTTO_MAIL_DEFINE "lotto/lotto_mail_define.txt"	//中獎通知user自定義檔
#define LOTTO_NUM_MAX 42	//開獎範圍上限
#define LOTTO_NUM_MIN 1		//開獎範圍下限
#define LOTTO_AMOUNT_MAX 30000   //每個人可以下注的最高數
#define LOTTO_OPEN_WDAY "2,5"	//一週裡有那幾天開獎
#define LOTTO_OPEN_WTIME 2      //一週的開獎次數(要配合上面)
/*
下兩行視各站的規模及 cpu的速度而定,限制簽賭時間就是執行lotto_open時間
77主站的CPU為k6-2-400,128MB;開101.x萬筆資料從20:45跑到隔天的3:55am
若跑超過0:00,則此部份程式已不敷使用,請改以crontab執行 lotto_close.sh
及lotto_run.sh
*/
#define LOTTO_OPEN_BEGIN "1915"	//限制簽賭時間範圍(begin), 格式:hhmm, 24小時制
#define LOTTO_OPEN_END "2230"
#define LOTTO_ANTE_MIN 1	//最小押金
#define LOTTO_GOLD_SILVER 0	//玩 金幣=0 ; 銀幣=1


/* 以下為賠率, 站長自行修改 */
#define LOTTO_RATE_1 1000000		//1獎賠率(6碼)
#define LOTTO_RATE_2 20000		//2獎賠率(5碼+特別號)
#define LOTTO_RATE_3 4000		//3獎賠率(5碼)
#define LOTTO_RATE_4 90			//4獎賠率(4碼)
#define LOTTO_RATE_5 10			//5獎賠率(3碼)
#define LOTTO_RATE_6 2			//6獎賠率(2碼)

/* 中獎通知預設值 0=不寄 1=寄*/ 
#define MAIL_S 1	//user個人單期簽賭統計
#define MAIL_1 1	//頭獎
#define MAIL_2 1
#define MAIL_3 1
#define MAIL_4 1
#define MAIL_5 0
#define MAIL_6 0	//六獎
