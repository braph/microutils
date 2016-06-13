/*
 * respawn - restart process by name
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
 */

#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <signal.h>
#include <unistd.h>

#define PROC_FS "/proc"
#define KILL_SIGNAL 15

/*
 * Free all strings in array and the array itself.
 */
inline
void free_argv(char **argv)
{
   for (char **it = argv; *it; ++it)
      free(*it);
   free(argv);
}

/*
 * Insert string into array.
 *
 * argv: array of strings
 * argc: current count of elements
 * arg:  string to be inserted
 */
int insert_argv(char ***argv, int *argc, char *arg)
{
   char **argv_realloc = realloc(*argv, (*argc+1) * sizeof(char *));
   if (! argv_realloc) {
      perror("realloc");
      return 0;
   }

   argv_realloc[ (*argc)++ ] = arg;
   *argv = argv_realloc;

   return 1;
}

/*
 * Read next NUL-delimited field from file stream.
 */
char* read_cmdline_field(FILE *fh)
{
   char *arg = NULL;
   size_t arg_size = 0;

   if (getdelim(&arg, &arg_size, 0, fh) != -1)
      return arg;
   
   return NULL;
}

/*
 * Return argv of process specified in pid.
 */
char** get_proc_argv(const char *pid)
{
   char *cmdline_path = NULL;
   FILE *cmdline_fh   = NULL;

   cmdline_path = malloc(strlen(PROC_FS) + strlen(pid) + 10);
   if (! cmdline_path) {
      perror("malloc");
      goto ERROR;
   }

   strcpy(cmdline_path, PROC_FS);
   strcat(cmdline_path, "/");
   strcat(cmdline_path, pid);
   strcat(cmdline_path, "/cmdline");

   cmdline_fh = fopen(cmdline_path, "rb");
   if (! cmdline_fh) {
      perror(cmdline_path);
      goto ERROR;
   }

   char **argv = NULL;
   int argc = 0;
   char *arg;

   while (arg = read_cmdline_field(cmdline_fh)) {
      if (! insert_argv(&argv, &argc, arg)) {
         free(arg);
         goto ERROR;
      }
   }

   // argv is NULL terminated
   if (! insert_argv(&argv, &argc, NULL))
      goto ERROR;

SUCCESS:
   free(cmdline_path);
   fclose(cmdline_fh);
   return argv;

ERROR:
   free(cmdline_path);
   fclose(cmdline_fh);
   free(argv);
   return NULL;
}

/*
 * Return current working directory of process
 */
char* get_proc_cwd(const char *pid)
{
   char *cwd_path  = NULL;
   char buf[2048];

   cwd_path = malloc(strlen(PROC_FS) + strlen(pid) + 6);
   if (! cwd_path) {
      perror("malloc");
      return NULL;
   }

   strcpy(cwd_path, PROC_FS);
   strcat(cwd_path, "/");
   strcat(cwd_path, pid);
   strcat(cwd_path, "/cwd");

   readlink(cwd_path, buf, 2047);

   free(cwd_path);
   return strdup(buf);
}
   
int str_is_digit(const char *str)
{
   while (*str)
      if (! isdigit(*str++))
         return 0;

   return 1;
}

/*
 * Match program name by its full path or by its basename.
 */
int filter_by_command(const char *search, const char *arg0)
{
   // matched full path
   if (! strcmp(search, arg0))
      return 1;

   // try to match basename
   char *arg0dup = strdup(arg0);
   if (! arg0dup) {
      perror("strdup");
      return 0;
   }

   char *base = basename(arg0dup);
   if (! strcmp(search, base)) {
      free(arg0dup);
      return 1;
   }

   free(arg0dup);
   return 0;
}

/*
 * Create new process
 */
void execvp_fork(const char *cwd, char **argv)
{
   fputs("cwd: ", stderr);
   fputs(cwd, stderr);
   fputs(": exec:", stderr);
   for (char **it = argv; *it; ++it) {
      putc(' ', stderr);
      fputs(*it, stderr);
   }
   putc('\n', stderr);

   switch (fork()) {
      case -1:
         perror("fork");
         return;

      case 0:
         chdir(cwd);
         execvp(argv[0], argv);
         perror("execv");
   }
}

void respawn(const char *command)
{
   int n_found = 0;

   DIR *procfs = opendir(PROC_FS);
   if (! procfs) {
      perror(PROC_FS);
      return;
   }

   struct dirent *proc_file;
   while (proc_file = readdir(procfs)) {

      // sort out non-process directories
      if (! str_is_digit(proc_file->d_name))
         continue;

      char **argv = get_proc_argv(proc_file->d_name);
      if (! argv) {
         perror("get_proc_argv");
         continue;
      }

      // not every PID has a cmdline - it may be empty
      if (! argv[0]) {
         free(argv);
         continue;
      }

      if (! filter_by_command(command, argv[0])) {
         free(argv);
         continue;
      }

      // we found our process
      ++n_found;

      char *cwd = get_proc_cwd(proc_file->d_name);

      if (! kill(atoi(proc_file->d_name), KILL_SIGNAL)) {
         execvp_fork(cwd, argv);
      }
      else {
         perror(proc_file->d_name);
      }

      free(argv);
      free(cwd);
   }

   closedir(procfs);

   if (! n_found) {
      fputs(command, stderr);
      fputs(": No processes found\n", stderr);
   }
}

int main(int argc, char **argv)
{
   for (int i = 1; i < argc; ++i)
      respawn(argv[i]);
}
