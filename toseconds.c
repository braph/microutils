/*
 * toseconds - return a timespan as seconds
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

#define MAX_INT_LENGTH 31

unsigned long to_seconds(const char *duration)
{
   unsigned long seconds = 0;

   char buf[MAX_INT_LENGTH + 1];
   buf[0] = 0;
   short buf_i = 0;

   char c;
   while (c = *duration++)
   {
      if (c >= '0' && c <= '9') {
         if (buf_i > MAX_INT_LENGTH) {
            errx(EXIT_FAILURE, "Integer length exceeded. Max character length is %i", MAX_INT_LENGTH);
         }

         buf[buf_i++] = c;
      }
      else {
         buf[buf_i] = 0;
         int i = atoi(buf);

         if (c == 's' || c == 'S') {
            seconds += i;
         } else if (c == 'm' || c == 'M') {
            seconds += i * 60;
         } else if (c == 'h' || c == 'H') {
            seconds += i * 60 * 60;
         } else if (c == 'd' || c == 'D') {
            seconds += i * 60 * 60 * 24;
         } else if (c == 'w' || c == 'W') {
            seconds += i * 60 * 60 * 24 * 7;
         } else {
            errx(EXIT_FAILURE, "Unrecognized character: %c", c);
         }

         buf[0] = 0;
         buf_i = 0;
      }
   }

   if (buf[0]) 
      seconds += atoi(buf);

   return seconds;
}

void help(const char *prg)
{
   fprintf(stderr,
      "Usage: %s TIMESPAN\n"
      "Returns the sum of TIMESPAN in seconds.\n\n"
      "Example: %s 5h4m11s"
      , prg);

   exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
   if (argc == 1) {
      fprintf(stderr, "Missing argument\n");
      help(argv[0]);
   }

   if (argc > 2) {
      fprintf(stderr, "Too much arguments\n");
      help(argv[0]);
   }

   unsigned long secs = to_seconds(argv[1]);

   printf("%d\n", secs);
}

