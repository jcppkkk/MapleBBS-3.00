/*-------------------------------------------------------*/
/*cut_ansistr()�i�H����r��e��, ���Ǧ^�r��==�ҭn�D�e��  */
/*buf�ѼƶǦ^�ű��Ӫ��������r��.                         */
/*str�ѼƩ�J�ݳB�z���r��                                */
/*len�ѼƩ�A�������                                  */
/*count���Ǧ^�ȬO�B�z�L�᪺����=strip_ansi(buf,str,0)    */
/*�@��:herb                                              */
/*-------------------------------------------------------*/

int
cut_ansistr(char *buf, char *str, int len) {
  register int ansi,count=0;

  for (ansi = 0; *str; str++) {
    if (*str == 27) {
      ansi = 1;
    }
    else if (ansi) {
      if (!strchr("[01234567;", *str)) {
         if (*str=='m' || *str=='*') {
            str--;
            for(;*str && strchr("[01234567;", *str) ;str--);
            for(;*str != 'm' && *str != '*';str++) {
              if (buf) *buf++ = *str;
            }
            if (buf) *buf++ = *str;
         }
         ansi = 0;
      }
    }
    else {
      if (count < len) {
        if (buf) *buf++ = *str;
        count++;
      }
    }
  }
  if (buf) *buf = '\0';
  return count;
}
