/*
 * write-file - write arguments to file
 * Copyright (C) 2016 Benjamin Abendroth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define bool char

void help(const char *prg)
{
   fprintf(stderr,
      "Usage: %s [OPTIONS] FILE [ARGS...]\n"
      "Truncates file and writes arguments to it.\n\n"
      "OPTIONS\n"
      "\t-n\tAppend a newline to each argument\n"
      "\t-a\tAppend instead of truncate\n"
      , prg);

   exit(1);
}

int main (int argc, char **argv)
{
   bool append = 0;
   bool newline = 0;
   int c;

   opterr = 0;

   while ((c = getopt(argc, argv, "an")) != -1) {
      switch (c) {
         case 'a':
            append = 1;
            break;

         case 'n':
            newline = 1;
            break;

         case '?':
            if (isprint(optopt))
               fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
               fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);

            help(argv[0]);

         default:
            abort();
      }
   }

   if (argc - optind == 0)
      help(argv[0]);

   const char *file = argv[optind];
   FILE *fh;
   
   if (append)
      fh = fopen(file, "a");
   else
      fh = fopen(file, "w");

   if (! fh) {
      perror(file);
      return 1;
   }

   for (int index = optind + 1; index < argc; index++) {
      fprintf(fh, "%s", argv[index]);

      if (newline)
         fprintf(fh, "\n");
   }

   fclose(fh);
   return 0;
}
