#include <string.h>
#include <ctype.h>

int
is_fname(str)
  char *str;
{
  int ch;

  ch = *str;
  if (ch == '/')
    return 0;

  do
  {
    if (!isalnum(ch) && !strchr("-._/+@", ch))
      return 0;
    ch = *++str;
  } while (ch);
  return 1;
}
