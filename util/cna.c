/*-------------------------------------------------------*/
/* util/cna.c       ( NCU EduCenter BBS Ver 0.01 )       */
/*-------------------------------------------------------*/
/* target : �Y�ɷs�D�K�n���ʺA�ݪ�                     */
/* ��  �@ : ARDA.bbs@NaiveAge.me.ncu.edu.tw              */
/* ��  �� : TJSheu.bbs@Education.ncu.edu.tw              */
/*          �������[�W�@�ǰѼơA���{���󦳼u��           */
/*-------------------------------------------------------*/
/* �ϥλ����G ����ɭn���|�ӰѼ�
   Usage: cna <cna board> <etc/out file> <number>
      <cna board>: �s�D�K�n���ݪO
      <etc/out file>: �x��b ~bbs/etc/outfile
      <number>: �˼ƲĴX�g�_�� 9 �ӷs�D�K�n
                �Ҧp�G cnanews Cna.toady news1 9
                     --> �ƲĤE�g�_�� 9 �ӷs�D�K�n
*/

#include "bbs.h"

main(int argc, char **argv)
{
    FILE *fp, *fp2;
    char fname[80], dest[80];
    int  i,j,total = 0;
    struct tm *ptime;
    time_t now;
   
    if (argc != 4) {
        printf("Usage: %s <cna board> <etc/out file> <number>\n", argv[0]);
        exit(-1);
    }

    sprintf(fname, BBSHOME "/boards/%s/.DIR", argv[1]);
    sprintf(dest,  BBSHOME "/etc/%s", argv[2]);

    if (fp = fopen(fname, "r"))
    {
        fileheader fhdr;
        while (fread(&fhdr, sizeof(fhdr), 1, fp) == 1)
            total++;
        fclose(fp);
    }

    if (fp = fopen(fname, "r"))
    {
        fileheader fhdr;
        int color=1;
        i = j = 1;
        if ((fp2 = fopen(dest, "w+")) != NULL)
        {
            time(&now);
            ptime = localtime(&now);
            fprintf(fp2, "[1;37m��[36m�w�w[33m�L�Ʒs�D�ֳ�[36m�w�w�w�w�w�w�w�w�w�w�w�w�w[31m�̫��s�ɶ�:[37m%02d/%02d %02d:%02d[36m�w[37m��[m\n",
                          ptime->tm_mon + 1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min);
            while ( (fread(&fhdr, sizeof(fhdr), 1, fp) == 1) && j <= 8)
            {
               fhdr.title[50] = 0;
               if ( i >= (total-atoi(argv[3])+1) && j<=9 )
               {
                  fprintf(fp2, "        [1;3%dm  %d  %s[m\n"
                           , color,i,fhdr.title);
                  color = ((color+1)%7)+1;
                  j++;
               }
               i++;
            }
            fprintf(fp2,"[1;37m��[36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[33m�����Ա�,�Ш� [31m%s [33m��[36m�w[37m��[m\n",argv[1]);
            fclose(fp2);
            fclose(fp);
       }
       else
            fprintf(stderr, "`%s` opened error (for read)\n", fname);
    }
}
