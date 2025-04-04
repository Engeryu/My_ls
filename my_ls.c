/**
 * @Author: Engeryu
 * @Date:   2018-05-04 09:42:11
 * @Last Modified by:   Engeryu
 * @Last Modified time: 2025-04-04 18:05:12
 * @Description: A simplified version of the "ls" command using custom libraries.
 *               It supports the following options:
 *                 -a   : list all entries (including hidden files)
 *                 -A   : list almost all entries (skip "." and "..")
 *                 -l   : long format listing (detailed information)
 *                 -d   : list the directory itself rather than its contents.
 *                          When used with -l (i.e. -ld or -dl), display detailed
 *                          info about the directory.
 *                 -la  / -al: combined long format listing with all entries.
 *
 *               Custom functions from your library (e.g. my_putstr, my_putchar,
 *               my_strcmp, etc.) are used throughout.
 */

#include <dirent.h>
#include "include/my.h"   // Master header including your custom headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>

/* Print detailed information for a file or directory (long format) */
static void ls_long(struct dirent *entry) {
    struct stat st;
    if (stat(entry->d_name, &st) == -1) {
        my_putstr("Error retrieving file info");
        my_putchar('\n');
        return;
    }
    /* Print file type and permissions */
    my_putstr(S_ISDIR(st.st_mode) ? "d" : "-");
    my_putstr((st.st_mode & S_IRUSR) ? "r" : "-");
    my_putstr((st.st_mode & S_IWUSR) ? "w" : "-");
    my_putstr((st.st_mode & S_IXUSR) ? "x" : "-");
    my_putstr((st.st_mode & S_IRGRP) ? "r" : "-");
    my_putstr((st.st_mode & S_IWGRP) ? "w" : "-");
    my_putstr((st.st_mode & S_IXGRP) ? "x" : "-");
    my_putstr((st.st_mode & S_IROTH) ? "r" : "-");
    my_putstr((st.st_mode & S_IWOTH) ? "w" : "-");
    my_putstr((st.st_mode & S_IXOTH) ? "x" : "-");
    my_putstr("  ");
    
    /* Print number of links */
    my_putnbr(st.st_nlink);
    my_putstr("  ");
    
    /* Print owner and group */
    struct passwd *pw = getpwuid(st.st_uid);
    my_putstr(pw ? pw->pw_name : "unknown");
    my_putstr("  ");
    struct group *gr = getgrgid(st.st_gid);
    my_putstr(gr ? gr->gr_name : "unknown");
    my_putstr("  ");
    
    /* Print file size */
    my_putnbr(st.st_size);
    my_putstr("  ");
    
    /* Print modification date (truncated from ctime output) */
    {
        char *time_str = ctime(&st.st_mtime);
        for (int i = 4; i < 16 && time_str[i] != '\0'; i++) {
            my_putchar(time_str[i]);
        }
        my_putstr("  ");
    }
    
    /* Finally, print the file name */
    my_putstr(entry->d_name);
    my_putchar('\n');
}

/* Simple display: prints only the file name followed by 3 spaces.
This function is used for ls, ls -a and ls -A modes.
*/
static void ls_simple(struct dirent *entry) {
    my_putstr(entry->d_name);
    my_putstr("   ");
}

/* Print detailed information for a directory itself (useful for -d with -l, e.g., ls -ld) */
static void ls_long_dir(const char *dirname) {
    struct stat st;
    if (stat(dirname, &st) == -1) {
        my_putstr(strerror(errno));
        my_putchar('\n');
        return;
    }
    /* Display similar to ls_long, but using the directory name directly */
    my_putstr(S_ISDIR(st.st_mode) ? "d" : "-");
    my_putstr((st.st_mode & S_IRUSR) ? "r" : "-");
    my_putstr((st.st_mode & S_IWUSR) ? "w" : "-");
    my_putstr((st.st_mode & S_IXUSR) ? "x" : "-");
    my_putstr((st.st_mode & S_IRGRP) ? "r" : "-");
    my_putstr((st.st_mode & S_IWGRP) ? "w" : "-");
    my_putstr((st.st_mode & S_IXGRP) ? "x" : "-");
    my_putstr((st.st_mode & S_IROTH) ? "r" : "-");
    my_putstr((st.st_mode & S_IWOTH) ? "w" : "-");
    my_putstr((st.st_mode & S_IXOTH) ? "x" : "-");
    my_putstr("  ");
    
    my_putnbr(st.st_nlink);
    my_putstr("  ");
    
    struct passwd *pw = getpwuid(st.st_uid);
    my_putstr(pw ? pw->pw_name : "unknown");
    my_putstr("  ");
    struct group *gr = getgrgid(st.st_gid);
    my_putstr(gr ? gr->gr_name : "unknown");
    my_putstr("  ");
    
    my_putnbr(st.st_size);
    my_putstr("  ");
    
    {
        char *time_str = ctime(&st.st_mtime);
        for (int i = 4; i < 16 && time_str[i] != '\0'; i++) {
            my_putchar(time_str[i]);
        }
        my_putstr("  ");
    }
    
    my_putstr(dirname);
    my_putchar('\n');
}

/* Parse the command line options.
Sets flags for -a, -A, -l, and -d, and determines the directory to list.
*/
static void parse_options(int argc, char *argv[],
                        int *flag_a, int *flag_A,
                        int *flag_l, int *flag_d,
                        char **directory)
{
    *directory = ".";
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'a')
                    *flag_a = 1;
                else if (argv[i][j] == 'A')
                    *flag_A = 1;
                else if (argv[i][j] == 'l')
                    *flag_l = 1;
                else if (argv[i][j] == 'd')
                    *flag_d = 1;
                // Additional options can be added here.
            }
        } else {
            /* Any string not starting with '-' is treated as a directory name */
            *directory = argv[i];
        }
    }
}

/* Main listing function */
static int ls(int argc, char *argv[])
{
    int flag_a = 0, flag_A = 0, flag_l = 0, flag_d = 0;
    char *directory;
    parse_options(argc, argv, &flag_a, &flag_A, &flag_l, &flag_d, &directory);

    /* If the -d option is activated, display the directory itself instead of its contents.
    Also check if the -l option is present.
    */
    if (flag_d) {
        if (flag_l)
            ls_long_dir(directory);  /* Display the directory in long format (e.g., ls -ld) */
        else {
            my_putstr(directory);
            my_putchar('\n');
        }
        return 0;
    }

    DIR *dir = opendir(directory);
    if (dir == NULL) {
        my_putstr(strerror(errno));
        my_putchar('\n');
        return -1;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* By default, ignore hidden files if neither -a nor -A is specified */
        if (!flag_a && !flag_A && entry->d_name[0] == '.')
            continue;
        /* With -A, explicitly ignore "." and ".." */
        if (flag_A && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
            continue;
        
        if (flag_l)
            ls_long(entry);
        else
            ls_simple(entry);
    }
    
    /* For simple listing mode, print a newline after the last entry */
    if (!flag_l)
        my_putchar('\n');
    
    closedir(dir);
    return 0;
}

int main(int argc, char *argv[])
{
    ls(argc, argv);
    flush_buffer();  // Ensure all buffered output is written
    return 0;
}