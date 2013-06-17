版權宣告

77 Lotto 0.6.1版

本程式原則上開放給大家在非營利的情況下可以任意修改複製,
若要以任何形式散播本程式請告知作者.

小弟才疏學淺, 程式中有任何錯誤的地方, 懇請各位先進不吝賜教!

對於本程式有任何問題歡迎到 telnet://77bbs.com/Worren. 討論;

新版下載點:
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

                                        逢甲大學七月七日晴BBS
                                        Worren Feung 馮國棟
                                        worren.feung@msa.hinet.net
                                        worren@77bbs.com
					陋室: http://worren.77bbs.com



七七樂透簽賭站讀我檔

install的方法:

1.把所有的檔案從/home/bbs解壓縮在/home/bbs/src/lotto, 

2.到 src/util/statool
  編輯Makefile, 新增一項post_open
TOOL =  account poststat birth gnp horoscope topmsg voteboard\
        topquery yearsold toppost toptime toplogin topexp topfive\
        topmoney topboard topgold sexcount toplimit toprpg\
        admin bm_check toppersonal post_open


3.在bbs的目錄中新增(menu.c):

  "lotto/lotto.so:menu_lotto",0,
    "66Lotto     ^^ 七七樂透簽賭站         最低消費 $1g",1,
  並make update

  當然, 若您希望是以銀幣消費, 可以把 $1g 改成 $1s or other else

4.edit lotto.h

原則上只要用預設值就可以了! 若要修改賠率, 可直接在這個檔修改:
但若是北銀有所改變, 也必須要跟著修改這個檔:
(4月份開始開獎日好像要變...要跟著把2,5改成3,6; 星期日為"0")


#define LOTTO_NUM_MAX 42        //開獎範圍上限
#define LOTTO_NUM_MIN 1         //開獎範圍下限
#define LOTTO_AMOUNT_MAX 2000   //每個人可以下注的最高數
#define LOTTO_OPEN_WDAY "2,5"   //一週裡有那幾天開獎
#define LOTTO_OPEN_WTIME 2      //一週的開獎次數(要配合上面)
/*
下兩行視各站的規模及 cpu的速度而定,限制簽賭時間就是執行lotto_open時間
77主站的CPU為k6-2-400,128MB;開101.x萬筆資料從20:45跑到隔天的3:55am
若跑超過0:00,則此部份程式已不敷使用,請改以crontab執行 lotto_close.sh
及lotto_run.sh
*/
#define LOTTO_OPEN_BEGIN "1915" //限制簽賭時間範圍(begin), 格式:hhmm, 24小時制
#define LOTTO_OPEN_END "2230"	//(end)
#define LOTTO_ANTE_MIN 1        //最小押金
#define LOTTO_GOLD_SILVER 0	//打算用金幣=0 or 銀幣=1玩

/* 以下為賠率, 站長自行修改 */
#define LOTTO_RATE_1 800000             //1獎賠率(6碼)
#define LOTTO_RATE_2 100000             //2獎賠率(5碼+特別號)
#define LOTTO_RATE_3 4000               //3獎賠率(5碼)
#define LOTTO_RATE_4 90                 //4獎賠率(4碼)
#define LOTTO_RATE_5 6                  //5獎賠率(3碼)
#define LOTTO_RATE_6 2                  //6獎賠率(2碼)

此賠率是最為統計過最合理的賠率,有點低(BBS銀行穩賺不賠), 
(ftp://77bbs.com/bbs/77lotto.xls)
當然您也可以自行提高!但要注意, 若是提太高會引起BBS系統的通貨膨脹!
提高賠率可吸引user來玩

5.回到 /home/bbs/src/lotto, 並執行install.sh

6.crontab 中:

# 七七樂透相關程式
# 每星期二五晚上8:30跑get_lotto_now, 隔天9:00再跑一次
30 20 * * 2,5 lotto/get_lotto_now.sh > /dev/null 2>&1
# 每星期二五晚上8:45跑lotto_open開獎程式
45 20 * * 2,5 lotto/lotto_open.sh > /dev/null 2>&1
# 把開獎記錄丟上Lotto.

# 若貴站的系統無法在時間內開完獎(run完lotto_open) 請把此時間延後!
# 從0.6版後, 由lotto_open在最適合的時候來啟動這個程式
#45 22 * * 2,5 bin/post_open > /dev/null 2>&1

# 從0.6版以後, 在啟動lotto_open後會自動run lotto_close.sh,
# 並在最適當的時候run lotto_run.sh
# 若lotto_open跑完已要超過12點, 請先用這兩行控制關lotto & 開 lotto站的時間
# 就 lotto_open在跑的話, lotto站就不能開!!
# 每星期二五晚上7:20關, 隔天早上6點開
#20 19 * * 2,5 lotto/lotto_close.sh > /dev/null 2>&1
#0 6 * * 3,6 lotto/lotto_run.sh > /dev/null 2>&1

7. 在bbs系統上開一個"Lotto."板, 讓開獎及統計結果post上去!

8. 第一次執行時, 要手動抓取前期的開獎號碼, 以bbsadm的身份
   run  ~/lotto/get_lotto_now.sh, 否則會出現"目前暫不開放"的字樣
   若以後突然想要關閉功能, 只要把~/lotto/lotto_no_now.txt砍掉
   就可以了! or 以後出現"目前暫不開放"的字樣, 就是這個檔不見了,
   也是run 一次get_lotto_now.sh就可以了! 
   ps:開獎後會自己抓一次!

9. edit ~/lotto/lotto_rule.txt 
   這是可以附在中獎通知後的檔, 您可以寫些您想寫東西..
   如遊戲規則..

10.~/lotto/lotto_no_history.txt是統計的依據檔,
   七七主站有從樂透第一期開始記錄,
   不想自己從第一期自己打的, 可以自行到77來d/l
   ftp://77bbs.com/bbs/lotto_no_history.txt
   每期開號後會同步更新!

11.有什麼問題敬請至bbs://77bbs.com  Worren. 板提出賜教!



			+------------------+
			|	NOTE	   |
			+------------------+

77 Lotto 

0.3版

  a)what's files updated?
	lotto.c , lotto_open.c , lotto_h
  b)what's new?
    1.新增可選擇用金幣或銀幣下注
    2.新增一個樂透站開關:
      在~/lotto/lotto_on_off.txt,這個檔的內容若為on那就是開站!
      若是沒這個檔 or 讀到的不是on, 那就關站!
  c)who needs?
    1.想要從原本的金幣改成銀幣的站長! -->一定要把update過的檔over write過!
    2.想要讓這個lotto站想關就關,想開就開的站長!
      (方便管理啦~...)
      若您的Lotto站已經修改成您想要的訊息文字, 但又想要這個功能,
      可以做以下的動做也可以達到:
      2.1:參考新的src,在lotto.h裡新增一行LOTTO_ON_OFF 
      2.2:參考新的src,在lotto.c的lotto_menu() 中加入下面一段:
/*
讀取 lotto_on_off.txt
if如果讀得到 lotto_on_off.txt 且內容為 "on" 那就是樂透站營業中
若讀不到 lotto_on_off.txt, 那就是關閉中
*/

   sprintf(buf,"%s%s",LOTTO_PATH,LOTTO_ON_OFF_FILE);
   fp = fopen(buf,"r");
   if(fp == NULL) {
   pressanykey("sorry, 系統整修, 目前暫不開放");
        fclose(fp);
        return 0;
   }

   strcpy(buf,"");
   fgets(buf,20,fp);
   fclose(fp);
   if(strncmp(buf,"on",2)!=0) {
      pressanykey("sorry, 系統整修, 目前暫不開放....");
      return ;
   }

//判斷是否開放中結束

  d)提供小秘訣給你:
    可以在Lotto.版的精華區中,開一個"歷史開獎記錄"的檔, 
    再到/home/bbs/man/boards/ 以 link的方式連到 
    /home/bbs/lotto/lotto_no_history.txt
    相信已經有不少人這麼做了!


0.5版

  a) what's files updated?
     lotto.c
  b) what's new?
     1.新增包牌選號
       不曉得程式這麼寫對不對....
  c) 修正從以前到現在readme.txt 和 install.sh 的小錯誤.

0.5.1版

  a)解決包牌選號時選13個以上的號碼去排列時, 排列數計算的錯誤!(update lotto.c)

0.5.2版

  a)解決當資料量太大時, 有一些統計數據沒法度show得正常(update lotto_open.c)
  b)修正說明檔執行install.sh的時機.
  c)新增FAQ幾個項目.
  d)解決當lotto_record.txt的資料太大時, lotto_open跑不完所造成的問題!
    (說明檔+crontab+*.sh,詳看 FAQ)
  e)修正包牌中獎的通知函及公佈中沒有加上"包牌"字樣!

0.6版

what update?
  lotto.h lotto.c lotto_open.c crontab排程 FAQ.txt

what's new?
  a)新增讓user自己設定要不要寄中獎通知.
    並可由sysop在lotto.h中設定預設值. 我預設6獎不寄,其它都寄!
  b)新增一個各人該期各項下注的數據統計 & 寄到該user信箱
  c)改進開獎期間的關/開LOTTO站機制.
    c-1:lotto.h裡設定時間不讓user進來,以防lotto_open在run了還有user在
        簽賭站內.(0.1版功能)
    c-2:lotto_open啟動時會把lotto_on_off.txt內容設為off,
        等開獎迴路跑完後就會改回on. 再繼續做個人統計.
  d)改進計算每一個user下注數的方法, 以檔案系統來代替迴路計算.

note:
  a)由於個人統計不會占到lotto開站時間, so個人統計通知暫設為"一定會寄"
    user也無法改成"不寄". 就算sysop手動改了, 也沒有判斷要不要寄的程試段.

  ****特別注意****
  b)請由0.6-beta以前upgrade 本版的網友注意! 由於計算每一個user下注數的
    程式及機制有做大幅度修改, so請於最近這一次開獎完後, 在user還沒有下注
    之前這段期間做upgrade, 以便能夠正確地記錄每個user的下注數!

0.6.1版

what's update?
  get_lotto_now.c

note:
  自09216期起, 北銀網站公佈開獎的格式做了變動, 故get_lotto_now必須
  要修正! 不過小弟我覺得以前的和現在的格式在explorer下看起來都一樣!
  只是由lynx dump出來的檔就差很多了! 不過也有可能會在未來恢復原來
  的模樣. so小弟特別把原來的get_lotto_now.c留在package中, 更名為
  get_lotto_now_0.6.c 以後若有必要, 再請各位站長使用舊的code.

what's next?
    在設定是否要寄通知的程式段中, 有使用資料庫的lock機制,
    之前都沒有! 計劃會加入各項有資料異動的程式中.

