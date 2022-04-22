#include "header.h"

void checkArguments(int argc, char *argv[])
{
   if (argc != 5)
   {
      perror("SERVER | Missing or too many arguments (4 required)");
      exit(-1);
   }
   if (strcmp(argv[1], "-p") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port] -g [name]");
      exit(-1);
   }

   if (strcmp(argv[3], "-n") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port] -g [name]");
      exit(-1);
   }
   system("clear");
}
