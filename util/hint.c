/*
[1;31m�~�w�w[37mBBS �p�ޥ�[31m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[33m�̫��s�ɶ�:[32m08/21 16:35[31m�w�w��[m
        [1;36m�A���D�ܡH �o�Ǥp�ޥ��i�H���A���P�A��r�֪��C�C BBS ��~~[m

       [1;33m��[m  �O�D�i�� [1;31mg[0m �N��i�]��[1;33m��K[0m
[0m           �j�a�b[1;36m�\Ū���[0m�U�A�u�n�� [1;31mTAB[0m �N�i�H�i�J[1;32m��K�\Ū�Ҧ�[0m�F
[0m       [1;33m��[m  �Q���D Board �W�O�_�� [1;36m�s�i���峹[0m
[0m           �u���� [1;31mc[0m ��N�i�H�o�I
[0m       [1;33m��[m  �i�H�]�w [1;36m�n�ͦW��[0m �H��K�d�M�z��[1;32m�n��[0m�O�_�b���W[40;0m
[0m           ([1;32mT[0m)alk -> ([1;32mO[0m)verride -> [1;31ma[0m
[0m
[1;31m���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[37m�оǨϥκ��F[m[31m�w��[m
[0m
*/

#include <stdio.h>
#include <stdlib.h>
#include "bbs.h"

#define HINTFILE   BBSHOME"/etc/hint"

main(void)
{
   FILE    *hintp;
   char    msg[136];
   int     i, j,k, msgNum;
   struct timeval  timep;
   struct timezone timezp;
   struct tm *ptime;
   time_t now;

   if (!(hintp = fopen(HINTFILE, "r")))
   {
      printf("Can't open %s\n",HINTFILE);
      exit(0);
   }

   fgets(msg, 135, hintp);
   msgNum = atoi(msg);


   k=1;
   time(&now);
   ptime = localtime(&now);
   printf("[1;31m�~�w�w[37mBBS �p�ޥ�[31m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[33m�̫��s�ɶ�:[32m%02d/%02d %02d:%02d[31m�w�w��[m\n",
           ptime->tm_mon + 1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min);
   printf("        [1;36m�A���D�ܡH �o�Ǥp�ޥ��i�H���A���P�A��r�֪��C�C BBS ��~~[m\n\n");
   while ( k <=3 )
   {
      fseek(hintp,0,SEEK_SET);
      fgets(msg, 135, hintp);
      gettimeofday(&timep, &timezp);
      i = (int) timep.tv_usec%(msgNum + 1);
      if (i == msgNum)
         i--;
      j = 0;

      while (j <= i)
      {
          fgets(msg, 135, hintp);
          msg[1] = '\0';
          if (!strncmp(msg,"#",1))
            j++;
      }
      printf("       [1;33m��[m",k,j);
      fgets(msg, 135, hintp);
      printf("  %s[0m", msg);
      fgets(msg, 135, hintp);
      printf("           %s[0m", msg);
      k++;
   }
   fclose(hintp);
   printf("\n");
   printf("[1;31m���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[37m�оǨϥκ��F[m[31m�w��[m\n");
}