/*  initsetproctitle() �O�q sendmail �� source code �̭����Ӫ�...
 *  setproctitle() �ڭ��g�L... countproctitle() �O�g�n����...
 *  �ոլ�... ����f�O�o�i�D��... :)
 *                                      -- Beagle
 */
#include <stdlib.h>
#include <stdarg.h>
#define newstr(s)    strcpy(malloc(strlen(s) + 1), s)

/*
**  Pointers for setproctitle.
**This allows "ps" listings to give more useful information.
*/

char            **Argv = NULL;          /* pointer to argument vector */
char            *LastArgv = NULL;       /* end of argv */

void
initsetproctitle(argc, argv, envp)
        int argc;
        char **argv;
        char **envp;
{
        register int i;
        extern char **environ;

        /*
        **  Move the environment so setproctitle can use the space at
        **  the top of memory.
        */

        for (i = 0; envp[i] != NULL; i++)
                continue;
        environ = (char **) malloc(sizeof (char *) * (i + 1));
        for (i = 0; envp[i] != NULL; i++)
                environ[i] = newstr(envp[i]);
        environ[i] = NULL;

        /*
        **  Save start and extent of argv for setproctitle.
        */

        Argv = argv;
        if (i > 0)
                LastArgv = envp[i - 1] + strlen(envp[i - 1]);
        else
                LastArgv = argv[argc - 1] + strlen(argv[argc - 1]);
}

setproctitle(const char* cmdline) {
        char buf[256], *p;
        int i;

        strncpy(buf, cmdline, 256);
        buf[255] = '\0';
        i = strlen(buf);
        if (i > LastArgv - Argv[0] - 2) {
                i = LastArgv - Argv[0] - 2;
        }
        strcpy(Argv[0], buf);
        p=&Argv[0][i];
        while (p < LastArgv) *p++='\0';
        Argv[1] = NULL;
}

printpt(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format,args);
    setproctitle(buf);
    va_end(args);
}

int countproctitle() {
  return (LastArgv - Argv[0] - 2);
}

