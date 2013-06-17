#define	STRICT_FQDN_EMAIL
#include <ctype.h>

int
not_addr(addr)
  char *addr;
{
  int ch, mode;

  mode = -1;

  while ((ch = *addr) != 0)
  {
    if (ch == '@')
    {
      if (++mode)
	break;
    }

#ifdef	STRICT_FQDN_EMAIL
    else if ((ch != '.') && (ch != '-') && (ch != '_') && !isalnum(ch))
#else
    else if (!isalnum(ch) && !strchr(".-_[]%!:", ch))
#endif

      return 1;

    addr++;
  }

  return mode;
}
