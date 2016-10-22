#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int main(void)
{
    DIR *cur, *proc;
    struct dirent *cure, *proce;
    FILE *inf;
    int c;

    if (chdir("/proc") == -1) {
        fprintf(stderr, "proclst: failed to chdir to /proc: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((proc = opendir(".")) == NULL) {
        fprintf(stderr, "proclst: failed to open /proc: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((proce = readdir(proc)) != NULL) {
        if (isdigit(proce->d_name[0])) {
            if ((cur = opendir(proce->d_name)) == NULL) {
                fprintf(stderr, "proclst: failed to open /proc/%s: %s\n",
                        proce->d_name, strerror(errno));
                continue;
            }

            while ((cure = readdir(cur)) != NULL) {
                if (!strcmp(cure->d_name, "cmdline")) {
                    if (chdir(proce->d_name) == -1) {
                        fprintf(stderr,
                                "proclst: failed to chdir into /proc/%s: %s\n",
                                proce->d_name, strerror(errno));
                        goto abend;
                    }

                    if ((inf = fopen("cmdline", "rb")) == NULL) {
                        fprintf(stderr,
                                "proclst: failed to open /proc/%s/cmdline: %s\n",
                                proce->d_name, strerror(errno));
                        continue;
                    }

                    printf("%s\t", proce->d_name);

                    while ((c = getc(inf)) != EOF) {
                        if ('\0' == c) {
                            putchar(' ');
                        } else {
                            putchar(c);
                        }
                    }

                    putchar('\n');

                    if (fclose(inf) == EOF) {
                        fprintf(stderr,
                                "proclst: failed to close /proc/%s/cmdline: %s\n",
                                proce->d_name, strerror(errno));
                    }

                    if (chdir("..") == -1) {
                        fprintf(stderr,
                                "proclst: failed to chdir out of /proc/%s: %s\n",
                                proce->d_name, strerror(errno));
                        exit(EXIT_FAILURE);
                    }

                    break;
                }
            }

            if (errno != 0) {
                fprintf(stderr, "proclst: failed to read /proc/%s: %s\n",
                        proce->d_name, strerror(errno));
            }

          abend:
            if (closedir(cur) == -1) {
                fprintf(stderr, "proclst: failed to close /proc/%s: %s\n",
                        proce->d_name, strerror(errno));
            }
        }
    }

    if (errno != 0) {
        fprintf(stderr, "proclst: failed to read /proc: %s\n",
                strerror(errno));
    }

    if (closedir(proc) == -1) {
        fprintf(stderr, "proclst: failed to close /proc: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}
