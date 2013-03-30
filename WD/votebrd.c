/* Copyright 1998,1999 NTU CSIE

   You should have received a copy of the GNU General Public License
   along with PttBBS; see the file COPYRIGHT.
   If not, write to the Free Software Foundation,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   voteboards' routines
*/
#define TIME_LIMIT	12*60*60

#include "bbs.h"

extern boardheader *bcache;

void
do_voteboardreply(fileheader *fhdr)
{
   char genbuf[1024];
   char reason[60];
   char fpath[80];
   char oldfpath[80];
   char opnion[10];
   char *ptr;
   FILE *fo, *fp;
   fileheader votefile;
   int len;
   int i, j;
   int fd;
   time_t endtime, now = time(NULL);
   int hastime = 0, check = 1;

   if(cuser.totaltime < TIME_LIMIT)
   {
     pressanykey("�W���ɼƤ��� , ���o�ѻP�s�p");
     return;
   }

   log_usies("VB_Reply",NULL);
   clear();

   setbpath(fpath, currboard);
   stampfile(fpath, &votefile);

   setbpath(oldfpath, currboard);

   strcat(oldfpath, "/");
   strcat(oldfpath, fhdr->filename);

   fp = fopen(oldfpath, "r");

   len = strlen(cuser.userid);

   while(fgets(genbuf, sizeof(genbuf), fp))
      {
      if (!strncmp(genbuf, "�s�p�����ɶ�", 12))
         {
         hastime = 1;
         ptr = strchr(genbuf, '(');
         sscanf(ptr+1, "%ld", &endtime);
         if (endtime < now){
            pressanykey("�s�p�ɶ��w�L");
            fclose(fp);
            return;
         }
      }
      else if (!strncmp(genbuf+4, cuser.userid, len))
       {
         move(5, 10);
         prints("�z�w�g�s�p�L���g�F");
         opnion[0] = 'n';
         getdata(7, 0, "�n�ק�z���e���s�p�ܡH(Y/N) [N]", opnion, 3,LCECHO,0);
         if (opnion[0] != 'y')
         {
            fclose(fp);
            return;
         }
         fgets(genbuf, sizeof(genbuf), fp);
         strcpy(reason, genbuf + 4);
         break;
      }
      else if (!strncmp(genbuf+27, cuser.email, strlen(cuser.email)))
      {
      	fclose(fp);
      	pressanykey("�ۦP��E-Mail�w�s�p�L, �U���Ц� ^o^");
      	return;
      }
   }
   fclose(fp);

   if((fd = open(oldfpath, O_RDONLY)) == -1)
      return;
   flock(fd, LOCK_EX);

   fo = fopen(fpath, "w");

   if (!fo)
      return;
   i = 0;
   while(fo)
   {
      j = 0;
      do
      {
        if (read(fd, genbuf+j, 1)<=0)
        {
           flock(fd, LOCK_UN);
           close(fd);
           fclose(fo);
           unlink(fpath);
           return;
        }
        j++;
      }
      while(genbuf[j-1] !='\n');
      genbuf[j] = '\0';
      i++;
      if (!strncmp("----------", genbuf, 10))
         break;
      if (i > 3)
         prints(genbuf);
      fprintf(fo, "%s", genbuf);
   }
   if (!hastime)
   {
      now += 7*24*60*60;
      fprintf(fo, "�s�p�����ɶ�: (%ld)%s", now, ctime(&now));
      now -= 7*24*60*60;
   }

   fprintf(fo, "%s", genbuf);

   do{
      clear();
      if (!getdata(18, 0, "�аݱz (Y)��� (N)�Ϲ� �o��ĳ�D [C]�G", opnion, 3,LCECHO,0))
      {
         flock(fd, LOCK_UN);
         close(fd);
         fclose(fo);
         unlink(fpath);
         return;
      }
   }
   while(opnion[0] != 'y' && opnion[0] != 'n');

   if (!getdata(20, 0, "�аݱz�P�o��ĳ�D�����Y�γs�p�z�Ѭ���G\n", reason,60, DOECHO,0))
    {
      flock(fd, LOCK_UN);
      close(fd);
      fclose(fo);
      unlink(fpath);
      return;
   }

   i = 0;

   while(fo)
   {
      i++;
      j = 0;
      do
      {
        if (read(fd, genbuf+j, 1)<=0)
        {
           flock(fd, LOCK_UN);
           close(fd);
           fclose(fo);
           unlink(fpath);
           return;
        }
        j++;
      }
      while(genbuf[j-1] !='\n');
      genbuf[j] = '\0';
      if (!strncmp("----------", genbuf, 10))
         break;
      if (genbuf[3] == '.' && strncmp(genbuf+4, cuser.userid, len))
      {
         fprintf(fo, "%3d.%s", i, genbuf+4);
         check = 0;
      }
      else if (check == 0 && genbuf[3] != '.')
      {
         fprintf(fo, "%s", genbuf);
         check = 1;
         i--;
      }
      else
         i--;
   }
   if (opnion[0] == 'y')
   {
      fprintf(fo, "%3d.%-15sE-Mail: %-50s\n", i, cuser.userid, cuser.email);
      fprintf(fo, "    [1;37;40m%s[m\n", reason);
   }
   i = 0;
   fprintf(fo, "%s", genbuf);
   while(fo)
   {
      i++;
      j = 0;
      do{
        if (!read(fd, genbuf+j, 1))
           break;
        j++;
        }
      while(genbuf[j-1] !='\n');
      genbuf[j] = '\0';
      if (j <= 3)
         break;
      if (genbuf[3] == '.' && strncmp(genbuf+4, cuser.userid, len))
      {
         fprintf(fo, "%3d.%s", i, genbuf+4);
         check = 0;
      }
      else if (check == 0 && genbuf[3] != '.')
      {
         fprintf(fo, "%s", genbuf);
         check = 1;
         i--;
      }
      else
         i--;
   }
   if (opnion[0] == 'n')
   {
      fprintf(fo, "%3d.%-15sE-Mail: %-50s\n", i, cuser.userid, cuser.email);
      fprintf(fo, "    [1;37;40m%s[m\n", reason);
   }
      
   flock(fd, LOCK_UN);
   close(fd);
   fclose(fo);
   unlink(oldfpath);
   f_mv(fpath, oldfpath);
}

int do_voteboard(void)
{
    fileheader votefile;
    char topic[100];
    char title[80];
    char genbuf[1024];
    char fpath[80];
    FILE* fp;
    int temp, i;
    time_t now = time(NULL);


    clear();
    if(!(currmode & MODE_POST) || cuser.totaltime < TIME_LIMIT)
    {
        pressanykey("�藍�_�A�z�ثe�L�k�b���o��峹�I");
        return RC_FULL;
    }
    log_usies("VB_Make");
    move(0, 0);
    clrtobot();
    prints("\n\n\n�z���b�ϥγs�p�t��\n");
    prints("���s�p�t�αN�߰ݱz�@�ǰ��D�A�Фp�ߦ^���~��}�l�s�p\n");
    prints("���N���X�s�p�ת̡A�N�Q�C�J���t�Τ����w��ϥΪ̳�\n\n\n");
    prints("                    ���{����@�̬�Ptt��CharlieL.\n");
    pressanykey(NULL);
    move(0, 0);
    clrtobot();
    prints("(1)�ӽзs�O (2)�o���ªO (3)�s�p�O�D (4)�}�K�O�D\n");
    if (!strcmp(currboard, VOTEBOARD))
      prints("(5)�s�p�p�ժ� (6)�}�K�p�ժ� ");
    if (!strcmp(currboard, VOTEBOARD) && HAS_PERM(PERM_SYSOP))
       prints("(7)��������");

    do
    {
      getdata(3, 0, "�п�J�s�p���O:", topic, 3, DOECHO,0);
      temp = atoi(topic);
    }while(temp <= 0 && temp >= 9);

    switch(temp)
    {
    case 1:
       do
       {
          if (!getdata(4, 0, "�п�J�ݪO�^��W�١G", topic, IDLEN+1,DOECHO,0))
             return RC_FULL;
          else if (invalid_brdname(topic))
             outs("���O���T���ݪO�W��");
          else if (getbnum(topic) > 0)
             outs("���W�٤w�g�s�b");
          else
             break;
       }while(temp > 0);
       sprintf(title, "[�ӽзs�O] %s", topic);
       sprintf(genbuf, "%s\n\n%s%-15s%s%-5d %s%-5d\n%s%s\n%s", "�ӽзs�O", "�ӽФH: ", cuser.userid, "�W������: ", cuser.numlogins, "�o��峹: ", cuser.numposts, "�^��W��: ", topic, "����W��: ");
       if (!getdata(5, 0, "�п�J�ݪO����W�١G", topic, 20, DOECHO,0))
          return RC_FULL;
       strcat(genbuf, topic);
       strcat(genbuf, "\n�ݪO���O: ");
       if (!getdata(6, 0, "�п�J�ݪO���O�G", topic, 20, DOECHO,0))
          return RC_FULL;
       strcat(genbuf, topic);
       strcat(genbuf, "\n�O�D�W��: ");
       getdata(7, 0, "�п�J�O�D�W��G", topic, IDLEN * 3 + 3, DOECHO,0);
       strcat(genbuf, topic);
       strcat(genbuf, "\n�ӽЭ�]: \n");
       move(8,0);
       outs("�п�J�ӽЭ�](�ܦh�Q��)�A�n�M����g���M���|�֭��");
       for(i= 9;i<19;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    case 2:
       do
       {
          if (!getdata(4, 0, "�п�J�ݪO�^��W�١G", topic, IDLEN+1,DOECHO,0))
             return RC_FULL;
          else if (getbnum(topic) <= 0)
             outs("���W�٨ä��s�b");
          else
             break;
       }
       while(temp > 0);
       sprintf(title, "[�o���ªO] %s", topic);
       sprintf(genbuf, "%s\n\n%s%s\n","�o���ªO", "�^��W��: ", topic);
       strcat(genbuf, "\n�o����]: \n");
       outs("�п�J�o����](�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;

       break;
    case 3:
       do
       {
          if (!getdata(4, 0, "�п�J�ݪO�^��W�١G", topic, IDLEN+1,DOECHO,0))
             return RC_FULL;
          else if (getbnum(topic) <= 0)
             outs("���W�٨ä��s�b");
          else
             break;
       }
       while(temp > 0);
       sprintf(title, "[�s�p�O�D] %s", topic);
       sprintf(genbuf, "%s\n\n%s%s\n%s%s\n%s%-5d %s%-5d","�s�p�O�D", "�^��W��: ", topic, "�ӽ� ID : ", cuser.userid, "�W������: ", cuser.numlogins, "�o��峹: ", cuser.numposts);
       strcat(genbuf, "\n�ӽЬF��: \n");
       outs("�п�J�ӽЬF��(�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    case 4:
       do
       {
          if (!getdata(4, 0, "�п�J�ݪO�^��W�١G", topic, IDLEN+1,DOECHO,0))
             return RC_FULL;
          else if ((i = getbnum(topic)) <= 0)
             outs("���W�٨ä��s�b");
          else
             break;
       }
       while(temp > 0);
       sprintf(title, "[�}�K�O�D] %s", topic);
       sprintf(genbuf, "%s\n\n%s%s\n%s","�}�K�O�D", "�^��W��: ", topic, "�O�D ID : ");
       do
       {
         if (!getdata(6, 0, "�п�J�O�DID�G", topic, IDLEN + 1, DOECHO,0))
            return RC_FULL;
         else if (!userid_is_BM(topic, bcache[i-1].BM))
            outs("���O�ӪO���O�D");
         else
            break;
       }
       while(temp > 0);
       strcat(genbuf, topic);
       strcat(genbuf, "\n�}�K��]: \n");
       outs("�п�J�}�K��](�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++){
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    case 5:
       if (!getdata(4, 0, "�п�J�p�դ��^��W�١G", topic, 30, DOECHO,0))
          return RC_FULL;
       sprintf(title, "[�s�p�p�ժ�] %s", topic);
       sprintf(genbuf, "%s\n\n%s%s\n%s%s\n%s%-5d %s%-5d","�s�p�p�ժ�", "�p�զW��: ", topic, "�ӽ� ID : ", cuser.userid, "�W������: ", cuser.numlogins, "�o��峹: ", cuser.numposts);
       strcat(genbuf, "\n�ӽЬF��: \n");
       outs("�п�J�ӽЬF��(�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    case 6:

       if (!getdata(4, 0, "�п�J�p�դ��^��W�١G", topic, 30, DOECHO,0))
          return RC_FULL;
       sprintf(title, "[�}�K�p�ժ�] %s", topic);
       sprintf(genbuf, "%s\n\n%s%s\n%s","�}�K�p�ժ�", "�p�զW��: ", topic, "�p�ժ� ID : ");
       if (!getdata(6, 0, "�п�J�p�ժ�ID�G", topic, IDLEN + 1, DOECHO,0))
          return RC_FULL;
       strcat(genbuf, topic);
       strcat(genbuf, "\n�}�K��]: \n");
       outs("�п�J�}�K��](�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    case 7:
       if (!HAS_PERM(PERM_SYSOP))
          return RC_FULL;
       if (!getdata(4, 0, "�п�J����D�D�G", topic, 30, DOECHO,0))
          return RC_FULL;
       sprintf(title, "%s %s", "[��������]", topic);
       sprintf(genbuf, "%s\n\n%s%s\n","��������", "����D�D: ", topic);
       strcat(genbuf, "\n�����]: \n");
       outs("�п�J�����](�ܦh����)�A�n�M����g���M���|�֭��");
       for(i= 8;i<13;i++)
       {
          if (!getdata(i, 0, "�G", topic, 60, DOECHO,0))
             break;
          strcat(genbuf, topic);
          strcat(genbuf, "\n");
       }
       if (i==8)
          return RC_FULL;
       break;
    default:
       return RC_FULL;
    }
    strcat(genbuf, "\n�s�p�����ɶ�: ");
    now += 7*24*60*60;
    sprintf(topic, "(%ld)", now);
    strcat(genbuf, topic);
    strcat(genbuf, ctime(&now));
    now -= 7*24*60*60;
    strcat(genbuf, "\n----------���----------\n");
    strcat(genbuf, "----------�Ϲ�----------\n");
    outs("�}�l�s�p��");
    setbpath(fpath, currboard);
    stampfile(fpath, &votefile);

    if (!(fp = fopen(fpath, "w")))
    {
       outs("�}�ɥ��ѡA�еy�ԭ��Ӥ@��");
       return RC_FULL;
    }
    fprintf(fp, "%s%s %s%s\n%s%s\n%s%s", "�@��: ", cuser.userid,
                                         "�ݪO: ", currboard,
                                         "���D: ", title,
                                         "�ɶ�: ", ctime(&now));
    fprintf(fp, "%s\n", genbuf);
    fclose(fp);
    strcpy(votefile.owner, cuser.userid);
    strcpy(votefile.title, title);
    votefile.savemode = 'S';
    setbdir(genbuf, currboard);
    rec_add(genbuf, &votefile, sizeof(votefile));
    do_voteboardreply(&votefile);
    return RC_FULL;
}



int
all_voteboard()
{

}

#include<stdarg.h>
void
va_do_voteboardreply(va_list pvar)
{
  fileheader *fhdr;
  fhdr = va_arg(pvar, fileheader *);
  return do_voteboardreply(fhdr);
}


