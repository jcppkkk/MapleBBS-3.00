/* 修正buildir無法排列M.9XXXXXXXX.A的文章問題...@_@*/
/* 用buildir建好.DIR後..在用此程式去修正他..@_@ */
#include <stdio.h>
#include "bbs.h"

main(int argc, char** argv)
{
   FILE* fp;
   char* fname;
   char buf[100];
   char buf1[100];
   char buf2[100];
   char buf3[100];
   char buf4[100];
 //  char buf5[200];
   char genbuf[512], path[256],path1[256], *ptr, *name;
   int fdir , fdir1;
   setuid(BBSUID);
   setgid(BBSGID);
     if (argc < 2)
    {
     printf("Usage: %s <directory> \n", argv[0]);
     exit(1);
    }
   sprintf(buf1,"%s/.DIR",argv[1]);
   sprintf(path, "%s/.DIR.9",argv[1]);
   sprintf(path1,"%s/.DIR.1",argv[1]);
   //sprintf(buf5, "cd %s;mv .DIR .DIR.bak",argv[1]);
   sprintf(buf2, "cd %s;mv .DIR  .DIR.bak",argv[1]);
   sprintf(buf,  "cd %s;cat .DIR.1 >> .DIR.9",argv[1]);
   sprintf(buf3, "cd %s;cat .DIR.9 >> .DIR",argv[1]);
   sprintf(buf4, "cd %s;rm -f .DIR.1 .DIR.9",argv[1]);


   fdir = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
   fdir1 = open(path1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fp = fopen(buf1, "r")) {
      fileheader fhdr;
      int n = 0;
      char type;
        while (fread(&fhdr, sizeof(fhdr), 1, fp) == 1)
      {
         fhdr.title[50] = 0;
         if (fhdr.filename[2] == '9' )
          write(fdir, &fhdr, sizeof(fhdr));
         else
          write(fdir1, &fhdr, sizeof(fhdr));
      }
      fclose(fp);
      close(fdir);
      close(fdir1);
    //  system(buf5);
      system(buf);
      system(buf2);
      system(buf3);
      system(buf4);
}
}

