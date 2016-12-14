/*
 * uniq_stable - stable version of uniq
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <search.h>

#define MALLOC_INC 1024

int main(int argc, char **argv)
{
   char *line = NULL;
   size_t lineSize = 0;

   char **lines = NULL;
   size_t nlines = 0;

   while (getline(&line, &lineSize, stdin) != -1) {
      if (nlines % MALLOC_INC == 0)
         if (! (lines = realloc(lines, sizeof(char *) * (nlines + MALLOC_INC))))
            return perror("realloc()"), 1;

      lines[nlines++] = line;
      line = NULL;
   }
   if (line)
      free(line);

   if (! hcreate(nlines))
      return perror("hcreate()"), 1;

   ENTRY e, *ep;

   for (int i = 0; i < nlines; ++i) {
      e.key = lines[i];
      e.data = 0;

      if (! (ep = hsearch(e, ENTER)))
         return perror("hsearch()"), 1;

      if (! ep->data++)
         fputs(e.key, stdout);
   }

   while (nlines--)
      free(lines[nlines]);
   free(lines);
   hdestroy();

   return 0;
}
