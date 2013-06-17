Link(char* src, char* dst)
{
   char cmd[200];
                                                                                  
   if (link(src, dst) == 0)
      return 0;
                                                                                  
   sprintf(cmd, "/bin/cp -R %s %s", src, dst);
   return system(cmd);
}

