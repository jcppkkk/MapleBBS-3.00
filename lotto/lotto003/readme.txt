
七七樂透簽賭站 0.2版 讀我檔

FreeBSD 4.5-STABLE , WD_BBS_001201 tested ok!

written by Worren

 0.1版  3/19
 0.2版  4/9
                                        worren@worren.2y.net
                                        worren@77bbs.com
                                        逢甲大學七月七日晴BBS 77bbs.com


(a)install的方法:

1.把所有的檔案從/home/bbs解壓縮在/home/bbs/src/lotto, 並執行install.sh
2.在bbs的目錄中新增(menu.c):

  "lotto/lotto.so:menu_lotto",0,
    "66Lotto     ^^ 七七樂透簽賭站         最低消費 $1g",1,
  並make update

3.copy post_open.c到 src/util/statool
  編輯Makefile, 新增一項post_open
TOOL =  account poststat birth gnp horoscope topmsg voteboard\
        topquery yearsold toppost toptime toplogin topexp topfive\
        topmoney topboard topgold sexcount toplimit toprpg\
        admin bm_check toppersonal post_open

  cd ~/src/util/statool
  make install之

4.edit lotto.h

原則上只要用預設值就可以了! 若要修改賠率, 可直接在這個檔修改:
但若是北銀有所改變, 也必須要跟著修改這個檔:

#define LOTTO_NUM_MAX 42        //開獎範圍上限
#define LOTTO_NUM_MIN 1         //開獎範圍下限
#define LOTTO_AMOUNT_MAX 2000   //每個人可以下注的最高數
#define LOTTO_OPEN_WDAY "2,5"   //一週裡有那幾天開獎, 星期日為"0"
#define LOTTO_OPEN_WTIME 2      //一週的開獎次數(要配合上面)
#define LOTTO_OPEN_BEGIN "1945" //限制簽賭時間範圍(begin), 格式:hhmm, 24小時制
#define LOTTO_OPEN_END "2130"	//(end)
#define LOTTO_ANTE_MIN 1        //最小押金

/* 以下為賠率, 站長自行修改 */
#define LOTTO_RATE_1 800000             //1獎賠率(6碼)
#define LOTTO_RATE_2 100000             //2獎賠率(5碼+特別號)
#define LOTTO_RATE_3 4000               //3獎賠率(5碼)
#define LOTTO_RATE_4 90                 //4獎賠率(4碼)
#define LOTTO_RATE_5 6                  //5獎賠率(3碼)
#define LOTTO_RATE_6 1                  //6獎賠率(2碼)

此賠率是最為統計過最合理的賠率,有點低(BBS銀行穩賺不賠), 
(ftp://77bbs.com/bbs/77lotto.xls)
當然您也可以自行提高!但要注意, 若是提太高會引起BBS系統的通貨膨脹!
提高賠率可吸引user來玩

5.~/src/lotto/> make clean install

6.crontab 中:

# 七七樂透相關程式
# 每星期二五晚上8:20跑get_lotto_now
20 20 * * 2,5 lotto/get_lotto_now.sh > /dev/null 2>&1
# 每星期二五晚上8:30跑lotto_open開獎程式
30 20 * * 2,5 lotto/lotto_open.sh > /dev/null 2>&1
# 把開獎記錄丟上Lotto.
30 21 * * 2,5 bin/post_open > /dev/null 2>&1

7. 在bbs系統上開一個"Lotto."板, 讓開獎及統計結果post上去!

   /* 把lotto的記錄弄上Lotto.版 */

         keeplog("lotto/lotto_open.txt", "WHAT_YOU_DEFINE", "[七七樂透站開獎] 中獎名單");
         keeplog("lotto/lotto_stat.txt", "WHAT_YOU_DEFINE", "[七七樂透站開獎] 開獎統計");
         exit(0);

   這個檔要copy到 ~/src/util/statool裡, 並在該路徑下再make一次


8. 第一次執行時, 要手動抓取前期的開獎號碼, 以bbsadm的身份
   run  ~/lotto/get_lotto_now.sh, 否則會出現"目前暫不開放"的字樣
   若以後突然想要關閉功能, 只要把~/lotto/lotto_no_now.txt砍掉
   就可以了! or 以後出現"目前暫不開放"的字樣, 就是這個檔不見了,
   也是run 一次get_lotto_now.sh就可以了! 
   ps:開獎後會自己抓一次!

9. edit ~/lotto/lotto_rule.txt 
   這是可以附在中獎通知後的檔, 您可以寫些您想寫東西..
   如遊戲規則(default)..

10.有什麼問題敬請至bbs://77bbs.com  Worren. 板提出!

11.謝謝您的使用



(b)相關函數 -> for advence

All are from WD_BBS 

相關函數
prints() 	:類似printf()
move(x,y)	:把游標移到 x,y
pressanykey(msg):在最底下出現msg並 "按任何鍵繼續..."
getdata()	:類似basic的locate + input 兩函數
clear()		:類似basic cls
stampfile(); +rec_add();+ update_data(); 寄信給user(簽賭結果,記錄)


重點相關函/變數:
cuser.goldmoney :user金幣數 (from struct.h)
cuser.userid	:取得user id
degold(int);	:將user的錢減少

