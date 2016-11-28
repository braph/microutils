/*
 * confirm - execute program with user confirmation
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
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int read_yes_no()
{
   struct termios oldt, newt;

   tcgetattr(STDIN_FILENO, &oldt);
   newt = oldt;
   newt.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newt);

   for (int c = getchar() ;; c = getchar())
      if (c == 'y' || c == 'Y') {
         c = 1;
         break;
      }
      else if (c == 'n' || c == 'N') {
         c = 0;
         break;
      }

   tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
   putchar('\n');
   return c;
}

int main(int argc, char **argv)
{
   for (int i = 1; i < argc; ++i) {
      fputs(argv[i], stdout);
      putchar(' ');
   }
   fputs("[y/n]", stdout);

   if (! read_yes_no())
      return 0;

   execvp(argv[1], &argv[1]);
   perror("execvp");
   return 1;
}
