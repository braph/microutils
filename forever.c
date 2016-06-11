/*
 * forever - call a program over and over again
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
#include <sys/wait.h>

int main(int argc, char **argv)
{
   for (;;) {
      switch(fork()) {
         case -1: 
            perror("fork");
            exit(EXIT_FAILURE);
            break;

         case 0:
            if (execvp(argv[1], &argv[1]))
               perror("execvp");
            break;

         default:                     
            wait(NULL);
            break;
      }
   }
}
