/*
SiE��O: 981110, pm 14:20,
meamea ��D file system full
��Ols -l �o�{.BOARDS size = 0,
BOARDS size = 2xx mb, src�ؿ�����,�L�k�״_
�ѨM�覡:
tar -zcf /tmp/brd.tgz ~/BOARDS ���ͤj�p��2xxxxx byte�����Y��
�ñNBOARDS�屼
����/home/bbs�w�g�ܦ�75%..���filesystem���Afull
���L.BOARDS��������reloaad�ɷ|�X���D
�]���ǥѦ��{�� �N���shm dump�U��
*/

#include "bbs.h"
#include "cache.c"

extern struct BCACHE *brdshm;
extern boardheader *bcache;

static void *
attach_shm(shmkey, shmsize)
  int shmkey, shmsize;
{
  void *shmptr;
  int shmid;

  shmid = shmget(shmkey, shmsize, 0);
  if (shmid < 0)
  {
    shmid = shmget(shmkey, shmsize, IPC_CREAT | 0600);
//    if (shmid < 0)
//      attach_err(shmkey, "shmget");
    shmptr = (void *) shmat(shmid, NULL, 0);
//    if (shmptr == (void *) -1)
//      attach_err(shmkey, "shmat");
    memset(shmptr, 0, shmsize);
  }
  else
  {
    shmptr = (void *) shmat(shmid, NULL, 0);
//    if (shmptr == (void *) -1)
//      attach_err(shmkey, "shmat");
  }
  return shmptr;
}

void
attch_xbrd()
{
  if (brdshm == NULL)
  {
    brdshm = shm_new(BRDSHM_KEY, sizeof(*brdshm));
    if (brdshm->touchtime == 0)
      brdshm->touchtime = 1;
    bcache = brdshm->bcache;
  }
}

main()
{
  FILE *fp;
  boardheader *b, *e;

  attch_xbrd();

  b = bcache;
  e = b + brdshm->number;

  do
  {
    printf("%-20s<<%s>>\n", b->brdname, b->title);

  } while (++b < e); 


  if (fp = fopen(BBSHOME"/BOARDS.NEW", "w"))
  {
      fwrite( bcache, sizeof(boardheader), brdshm->number, fp);
  }
  fclose(fp);
}
