 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #define  SIZE  80

 void garbe(FILE *fp){
   int i=0;
   char ch[SIZE];

   while((ch[i]=fgetc(fp))!='>')
     i++;
   ch[i]='\n';

//   printf("%s\n",ch);
   if(strcmp(ch,"br")==0 || strcmp(ch,"BR")==0){
      printf("  *\n");
      return;
   }
   return;
 }

 int main(void){

   char file_name[20],tmp[10],cmds_compare[SIZE],ch;
   FILE *fp,*fd;

   tmpnam(file_name);                   //取一個暫存檔檔名
   strcpy(cmds_compare,"lynx -source http://www.chiayis.com.tw/living/movie.htm > ");
   strcat(cmds_compare,file_name);      //串聯指令
   system(cmds_compare);                //system呼叫
   fp=fopen(file_name,"r");
//   fd=fopen("/home/bbs/etc/movie.txt","w");
   while((ch=fgetc(fp))!=EOF){
     if(ch=='<')
        garbe(fp);
//     else if(ch=='\n');
     else
        printf("%c",ch);

   }
   fclose(fp);
//   fclose(fd);
   strcpy(cmds_compare,"rm -f ");
   strcat(cmds_compare,file_name);

   tmpnam(file_name);                   //取一個暫存檔檔名
   strcpy(cmds_compare,"lynx -source http://www.chiayis.com.tw/living/movie.htm > ");
   strcat(cmds_compare,file_name);      //串聯指令
   system(cmds_compare);                //system呼叫
   fp=fopen(file_name,"r");
//   fd=fopen("/home/bbs/etc/movie.txt","w");
   while((ch=fgetc(fp))!=EOF){
     if(ch=='<')
        garbe(fp);
//     else if(ch=='\n');
     else
        printf("%c",ch);

   }
   fclose(fp);
//   fclose(fd);
   strcpy(cmds_compare,"rm -f ");
   strcat(cmds_compare,file_name);
   system(cmds_compare);
 }

