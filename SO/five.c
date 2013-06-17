#include "bbs.h"
#define BBLANK (0)  /* ªÅ¥Õ */
#define BBLACK (1)  /* ¶Â¤l, ¥ý¤â */
#define BWHITE (2)  /* ¥Õ¤l, «á¤â */

#ifndef BRDSIZ
#define BRDSIZ (15) /* ´Ñ½L³æÃä¤j¤p */
#endif
#define BGOTO(x, y)	move( 16 - y , x * 2 + 3)

char ku[BRDSIZ][BRDSIZ];
char *chess[] = { "¡´", "¡³" };

/* pattern and advance map */
/*extern struct RCACHE *ric;*/
uschar  *pat, *adv;
#include "pat.h"

#define	XCIN_LR_REDUCE

#ifdef XCIN_LR_REDUCE		/* ºI¥hXCIN¹ï¤¤¤å¥þ§Î¥ª¥kªº¦¸¼Æ */
int	xcin_mode;
int	xcin_redu;
#endif

typedef struct
{
  char x;
  char y;
} Horder;
  
static Horder *v, pool[225];


static int intrevcmp(const void *a, const void *b)
{
  return (*(int *)b - *(int *)a);
}

/* x,y: 0..BRDSIZ-1 ; color: CBLACK,CWHITE ; dx,dy: -1,0,+1 */
static int 
getindex(x, y, color, dx, dy)
  int x, y, color, dx, dy;
{
  int i, k, n;
  for (n = -1, i = 0, k = 1; i < 5; i++, k <<= 1)
  {
    x += dx;
    y += dy;

    if ((x < 0) || (x >= BRDSIZ) || ( y < 0) || ( y >= BRDSIZ))
    {
      n += k;
      break;
    }
    else if (ku[x][y] != BBLANK)
    {
      n += k;
      if (ku[x][y] != color) break;
    }
  }

  if (i >= 5) n += k;

  return n;
}

/* x,y: 0..BRDSIZ-1 ; color: CBLACK,CWHITE ; limit:1,0 ; dx,dy: 0,1 */
static int 
dirchk(x, y, color, limit, dx, dy)
  int x, y, color, limit, dx, dy;
{
  int chkwin(int, int);
  int getstyle(int, int, int, int);

  int le, ri, loc, style;

  le = getindex(x, y, color, -dx, -dy);
  ri = getindex(x, y, color, dx,  dy);

  loc = (le > ri) ? (((le*(le+1))>>1)+ri):(((ri*(ri+1))>>1)+le);

  style = pat[loc];

  if (limit==0) return (style&0x0f);

  style>>=4;

  if ((style == 3) || (style == 2))
  {
    int i, n, tmp, nx, ny;
    n = adv[loc>>1];

    ((loc&1) == 0)? (n >>= 4):(n &= 0x0f);

    ku[x][y] = color;

    for (i = 0; i < 2; i++)
    {
      if ((tmp = (i == 0) ? (-(n >> 2)):(n & 3)) != 0)
      {
        nx = x + (le > ri ? 1 : -1) * tmp * dx;
        ny = y + (le > ri ? 1 : -1) * tmp * dy;

        if ((dirchk(nx, ny, color, 0, dx, dy) == 0x06) &&
            (chkwin(getstyle(nx, ny, color, limit), limit) >= 0))
          break;

      }
    }
    if (i >= 2) style=0;
    ku[x][y] = BBLANK;
  }
  return style;
}

/* ¨Ò¥~=F ¿ù»~=E ¦³¤l=D ³s¤­=C ³s¤»=B Âù¥|=A ¥|¥|=9 ¤T¤T=8 */
/* ¥|¤T=7 ¬¡¥|=6 Â_¥|=5 ¦º¥|=4 ¬¡¤T=3 Â_¤T=2 «O¯d=1 µL®Ä=0 */

/* x,y: 0..BRDSIZ-1 ; color: CBLACK,CWHITE ; limit: 1,0 */
int 
getstyle(x, y, color, limit)
  int x, y, color, limit;
{
  int i, j, dir[4], style;

  if ((x < 0) || (x >= BRDSIZ) || ( y < 0) || (y >= BRDSIZ)) return 0x0f;

  if (ku[x][y] != BBLANK) return 0x0d;

  for (i = 0; i < 4; i++)
    dir[i] = dirchk(x, y, color, limit, i ? (i>>1) : -1, i ? (i&1) : 1);

  qsort(dir, 4, sizeof(int), intrevcmp);

  if ((style = dir[0]) >= 2)
  {
    for (i = 1, j = 6 + (limit ? 1 : 0); i < 4; i++)
    {
      if ((style > j) || (dir[i] < 2)) break;
      if (dir[i] > 3) style = 9;
      else if ((style < 7) && (style > 3)) style = 7;
      else style = 8;
    }
  }
  return style;
}

int 
chkwin(style, limit)
  int style, limit;
{
  if (style == 0x0c) return 1 /* style */;
  else if (limit == 0)
  {
    if (style == 0x0b) return 1 /* style */;
    return 0;
  }
  if ((style < 0x0c) && (style > 0x07)) return -1 /* -style */;
  return 0;
}

HO_init()
{
  memset ( pool, 0 , sizeof(pool));
  v = pool;
  pat = pat_gomoku;
  adv = adv_gomoku;
  memset( ku, 0, sizeof(ku));

}

static void
HO_add(mv)
 Horder *mv;
{
  *v++ = *mv;
}

static void
HO_undo(mv)
 Horder *mv;
{
  char *str = "¢z¢s¢{¢u¢q¢t¢|¢r¢}";
  int n1, n2, loc;

  *mv = *(--v);
  ku[mv->x][mv->y] = BBLANK;
  BGOTO(mv->x, mv->y);
  n1 = (mv->x == 0) ? 0 : (mv->x == 14) ? 2 : 1;
  n2 = (mv->y == 14)? 0 : (mv->y == 0) ? 2 : 1;  
  loc= 2 * ( n2 * 3 + n1);
  prints("%.2s", str + loc);
/*
  outc(str[loc-1]);
  outc(str[loc]);
*/
}

void
HO_log(title)
  char *title;
{
  int i;
  FILE *log;
  char buf[80];
  char fpath[80];
  Horder *ptr = pool;
  extern screenline *big_picture;

  sprintf(buf, "home/%s/F.%d", cuser.userid, rand() & 65535);
  sprintf(fpath,buf);
  log = fopen(buf, "w");

  for( i = 1; i < 17; i++)
         fprintf(log, "%.*s\n", big_picture[i].len, big_picture[i].data);

  i = 0;

  do
  {
    fprintf(log, "[%2d]%s ==> %c%d%c", i + 1, chess[i % 2],
	 	'A' + ptr->x, ptr->y + 1, (i % 2) ? '\n' : '\t');
    i++;

  } while ( ++ptr < v);

  fclose(log);
//  f2_mbox(buf, title, "[¹ï¾Ô´ÑÃÐ]", cuser.userid, 1);
  bbs_sendmail(fpath,title,cuser.userid);
  unlink(buf);
}

/*
void
HO_outs(pool, v, key, cc)
 Horder *pool, *v;
 char *key, cc;
{
  static odr = 0;

  int size = v - pool;

  if ( cc == HADD ) odr++;
 
  if ( cc == HADD && odr <= HSIZE / 2 )
  {
    HGOTO(odr);
    prints("²Ä%2d¤â %s %c%d", odr++, key, 'A' + v[-1]->x, v[-1]->y + 1);
  }
  else 
  {
    for ( head = pool + (( size - HSIZE / 2 ) & ~0x1) ; head < v; head++)
    {
      HGOTO(i++);
      prints("²Ä%2d¤â %s %c%d", i++, key, 'A' + head->x, head->y + 1);
    } 
      
  }
}
*/

int
chkmv(mv, color, limit)
  Horder *mv;
  int color, limit;
{
  char *xtype[] = { "¸õ¤T", "¬¡¤T", "¦º¥|",
		    "¸õ¥|", "¬¡¥|", "¥|¤T", "Âù¤T", "Âù¥|",
		    "Âù¥|", "³s¤»", "³s¤­" };

#if 0
  int  rule;

  if ( mv->x < 0 || mv->x > 14 || mv->y < 0 || mv->y > 14)
  {
    outs("invalid move");
    return 0;
  }
#endif
  int rule = getstyle( mv->x, mv->y, color, limit );

  if ( rule > 1  && rule < 13 )  
  {
     move ( 15, 40);
     outs( xtype[rule - 2] );
     bell();
  }
  return chkwin(rule, limit); 
}

int
gomo_key(fd, ch, mv)
  int fd, ch;
  Horder *mv;
{
      if ( ch >= 'a' && ch <= 'o')
      {
        char pbuf[4], vx, vy;

        *pbuf = ch;
	if (fd) add_io(0, 0);
        getdata( 17, 0, "ª½±µ«ü©w¦ì¸m :", pbuf, 4, DOECHO,0);
        if (fd) add_io(fd, 0);
        vx = pbuf[0] - 'a';
//        vy = atoi(pbuf + 1) - 1;
        vy = atoi(pbuf + 1);
        if ( vx >= 0 && vx < 15 && vy > 0 && vy <= 15 && ku[vx][vy] == BBLANK)
        {
          mv->x = vx;
          mv->y = vy;
          return 1;
        }
      }
      else
      {

        switch(ch)
        {
          case KEY_RIGHT:
#ifdef	XCIN_LR_REDUCE
	    if ( xcin_mode )
	    {
		xcin_redu = ( xcin_redu ? 0 : 1 );
		if ( xcin_redu )
		break;
	    }
#endif
            mv->x = (mv->x == BRDSIZ - 1) ? mv->x : mv->x + 1;
            break;
          case KEY_LEFT:
#ifdef	XCIN_LR_REDUCE
	    if ( xcin_mode )
	    {
		xcin_redu = ( xcin_redu ? 0 : 1 );
		if ( xcin_redu )
		break;
	    }
#endif
            mv->x = (mv->x == 0 ) ? 0 : mv->x - 1;
            break;
          case KEY_UP:
//            mv->y = (mv->y == BRDSIZ - 1) ? mv->y : mv->y + 1;
            mv->y = (mv->y == BRDSIZ ) ? mv->y : mv->y + 1;
            break;
          case KEY_DOWN:
//            mv->y = (mv->y == 0 ) ? 0 : mv->y - 1;
            mv->y = (mv->y <= 1 ) ? 1 : mv->y - 1;
            break;
          case ' ':
          case '\r':
            if ( ku[mv->x][mv->y] == BBLANK)return 1;
        }
      }
  return 0;
}

int
gomoku(fd)
  int fd;
{
  Horder mv;
  int me, he, win, ch, passflag, fix, writetolog=0;
  user_info *my = currutmp;
  char genbuf[200];

  HO_init();

  me = !(my->turn) + 1;
  he = my->turn + 1;
  win = 1;
  passflag = 0;
  fix = 0;
	
#ifdef	XCIN_LR_REDUCE
  xcin_mode = 0;
#endif

  setutmpmode(FIVE);
  clear();
  
  /* show_film(1, 16, 0); */
  show_file("game/five", 0, 24, NO_RELOAD);
  prints(COLOR2"  ¤­¤l´Ñ¹ï¾Ô  "COLOR1"[1m%30s VS %-30s[m",
	cuser.userid, my->chatid);
 
  move(11, 40);
  prints("§Ú¬O %s", me == BBLACK ? "¥ý¤â ¡´, ¦³¸T¤â" : "«á¤â ¡³");
              
  add_io(fd, 0);

  mv.x = mv.y = 7; 
  for(;;)
  {

#ifdef	XCIN_LR_REDUCE
    move( 5, 40);
    outs( xcin_mode ? "XCIN ¼Ò¦¡" : "         " );
#endif

    move( 13, 40);
    outs(my->turn ? "½ü¨ì¦Û¤v¤U¤F!": "µ¥«Ý¹ï¤è¤U¤l..");

    move(14, 40);
    if (passflag)
    {
      outs(my->turn ? "´£¥X©M´Ñ­n¨D!": "¹ï¤è´£¥X©M´Ñ­n¨D! («öp¦P·N©M´Ñ)");
      bell();
      fix = 1;
    }	
    else
    {
       clrtoeol();
       fix = 0;
    }
   
    BGOTO(mv.x, mv.y);
    ch = igetkey();

#ifdef	XCIN_LR_REDUCE
    if (ch == 'x')
    {
	xcin_mode = ( xcin_mode ? 0 : 1 );
	continue;
    }
#endif

    if (ch == 'q') 
    {
      send(fd, "", 1, 0);
      break;
    }
    else if (ch == 'u' && !my->turn && v > pool)
    {
      mv.x = mv.y = -1;
      ch =send(fd, &mv, sizeof(Horder), 0);
      if (ch == sizeof(Horder))
      {
	HO_undo(&mv);
        my->turn = 1;
  	continue;
      }
      else break;
    }		
    if ( ch == 'p')
    {
	if ( my->turn )
        {
	  passflag = 1;
	  mv.x = mv.y = -2;
	  send(fd, &mv, sizeof(Horder), 0);
	  mv = *(v - 1);
	  continue;
	}
	else if (passflag)
	{
	  win = 0;
	  fix = 1;
	  outmsg("µªÀ³©M´Ñ­n¨D!");
	  break;
	}
    }
    else passflag = 0;
 
    if (ch == I_OTHERDATA)
    {
      ch = recv(fd, &mv, sizeof(Horder), 0);

      if (ch != sizeof(Horder))
      {
        if (fix == 0)
        {
 	  win = 1;
	  outmsg("¹ï¤è»{¿é¤F!!");
          break;
        }
        else
        {
           outmsg("Âù¤è¾Ô©M!");
           break;
        }
      }

      if ( mv.x == -2 && mv.y == -2 )
      {
         if (passflag == 1) 
	 {
	    win = 0;
  	    break;
	 }
	 else
	 {
	    passflag = 1;	
	    mv = *(v - 1);
	    continue;
	 }
      }

      if ( my->turn && mv.x == -1 && mv.y == -1)
      {
	 outmsg("¹ï¤è®¬´Ñ");
	 HO_undo(&mv);
	 my->turn = 0;
	 continue;
      }

      if ( !my->turn )
      {
        win = chkmv(&mv, he, he == BBLACK);
	HO_add(&mv);

        ku[mv.x][mv.y] = he;
	BGOTO(mv.x, mv.y);
	outs(chess[he - 1]);

	if ( win ) 
 	{
	  outmsg( win == 1 ? "¹ï¤è³Ó!" : "¹ï¤è¿é!" );
	  win = 0;
          break; 
	}	
	my->turn = 1;
      }
      continue;
    }

    if (my->turn) 
    {
      if (gomo_key(fd, ch, &mv)) my->turn = 0;	
      else continue;

      if (!my->turn)
      {

        HO_add(&mv);
        BGOTO(mv.x, mv.y);
	outs(chess[me - 1]);
	win = chkmv( &mv, me, me == BBLACK);
        ku[mv.x][mv.y] = me;
        if (send(fd, &mv, sizeof(Horder), 0) != sizeof(Horder))
        break;
	if (win)
	{
	  outmsg( win == 1 ? "§ÚÄ¹Åo Y^o^Y " : "§Ú¿é¤F ~~>_<~~ " );
	  if (win == 1)
   	    writetolog=1;
   	  if (win == -1)
   	    writetolog=-1;
	  break;
	}
	move(15, 40);
	clrtoeol();
      }
    }
  }

  add_io(0, 0);
  close(fd);
  
//  sprintf(genbuf,"win=%d,fix=%d,pass=%d,trun=%d",win,fix,passflag,my->turn);
//  game_log(FIVE,genbuf);

    if(fix == 1 & my->turn)
    {
       sprintf(genbuf,"»P %s ¾Ô©M", my->chatid);
       game_log(FIVE,genbuf);
    }
    else if(win == 1 && writetolog == 1)
    {
      sprintf(genbuf,"À»±Ñ¤F %s ¤@³õ", my->chatid);
      game_log(FIVE,genbuf);
    }
    else if(writetolog == -1)
    {
      sprintf(genbuf,"³Q %s À»±Ñ¤F", my->chatid);
      game_log(FIVE,genbuf);
    }

  igetch();
  if (v > pool && getans("­n¦s¦¨´ÑÃÐ«O¯d¶Ü?[N]") == 'y')
  {
    HO_log("´ÑÃÐ¬ã¨s"); 
  }    
}

five()
{
  Horder mv;
  int turn, win, ch, key;

  HO_init();

  turn = win = 0;
  key = turn + 1;
  mv.x = mv.y = 7;

  /* show_film(1, 18, 1); */

  move(11, 40);
  prints("´ÑÃÐ¬ã¨s");

  do
  {
    BGOTO(mv.x, mv.y);
    ch = igetkey();
    if (ch == 'q') break;
    if (ch == 'u' && v > pool) 
    {
	HO_undo(&mv);
        turn ^= 1;
        key = turn + 1; 
    }
	
    if (!gomo_key(0, ch, &mv)) continue;

    HO_add(&mv);
    BGOTO(mv.x, mv.y);
    outs(chess[turn]);
    win = chkmv( &mv, key, key == BBLACK);
    ku[mv.x][mv.y] = key;
    turn ^= 1;
    key = turn + 1; 
    move(13, 40);
    prints("½ü¨ì %s ¤U¤l..", chess[turn]);
    refresh();

  } while (!win);

  if (v > pool && getans("«O¯d´ÑÃÐ¶Ü?[N]") == 'y')
  HO_log("´ÑÃÐ¬ã¨s");
}

#include<stdarg.h>
int va_gomoku(va_list pvar)
{
  int sock;
  sock = va_arg(pvar, int);
  return gomoku(sock);
}
