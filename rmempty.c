/*
 * rmempty - remove empty files
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

#define LOG_NORMAL 0
#define LOG_VERBOSE 1
static int verbosity = LOG_NORMAL;

static int rm_empty(const char *file) {
   struct stat st;

   if (verbosity >= LOG_VERBOSE)
      warnx("removing file, '%s'", file);

   if (stat(file, &st) < 0) {
      if (verbosity >= LOG_NORMAL)
         warn("%s", file);
      return 1;
   }

   if (! st.st_size) {
      if (unlink(file) < 0) {
         if (verbosity >= LOG_NORMAL)
            warn("%s", file);
         return 1;
      }
   }
   else {
      if (verbosity >= LOG_NORMAL)
         warnx("failed to remove '%s': file not empty", file);
      return 1;
   }

   return 0;
}

static void print_help(const char *prog) {
   printf("%s: [-h] [-v] [-q] FILES\n", prog);
   puts("Remove empty files\n");
   puts("-h       Show this help");
   puts("-v       Be verbose");
   puts("-q       Supress error messages");
}

int main(int argc, char **argv)
{
   int argi;
   int c;
   int ret_val = 0;
   opterr = 0;

   while ((c = getopt(argc, argv, "hvq")) != -1)
      switch (c)
      {
         case 'h':
            print_help(argv[0]);
            return 0;
         case 'v':
            verbosity = 1;
            break;
         case 'q':
            verbosity = -1;
            break;
         case '?':
            /*
            if (optopt == 'c')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else 
            */
            if (isprint(optopt))
               fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
               fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
         default:
            abort();
      }

   for (argi = optind; argi < argc; argi++)
      ret_val |= rm_empty(argv[argi]);

   return ret_val;
}
