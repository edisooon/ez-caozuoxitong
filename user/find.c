#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
find(char *dir, char *filename, int argc, char *argv[])
{
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    if((fd = open(dir, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", dir);
        exit(1);
    }
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir);
        exit(1);
    }
    if(st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", dir);
        exit(1);
    }

    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf ) {
        printf("find: dir (%s) is too deep to search", dir);
        return;
    }

    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        switch(st.type) {
            case T_DEVICE:
            case T_FILE:
                if(strcmp(de.name, filename) == 0) {
                    if(argc>=5 && strcmp(argv[3], "-exec")==0 ) {
                        char *new_argv[MAXARG];
                        int new_argc = argc;
                        int pid;
                        memmove(new_argv, argv+4, sizeof(char**) * (new_argc-=4));
                        new_argv[new_argc++] = buf;

                        pid = fork();
                        if(pid > 0) {
                            wait(0);
                        }else if(pid == 0) {
                            exec(new_argv[0], new_argv);
                            fprintf(2, "find: exec error\n");
                            exit(1);
                        }else{
                            fprintf(2, "find: fork error\n");
                            exit(1);
                        }
                    } else {
                        printf("%s\n", buf); 
                    }
                }
                break;
            case T_DIR:
                if(strcmp(de.name, ".") != 0 && (strcmp(de.name, "..") != 0)) 
                    find(buf, filename, argc, argv);
                break;
        }
        
    }

}

int
main(int argc, char *argv[])
{
    if(argc < 3) {
        fprintf(2, "Usage: find directory file\n");
        exit(1);
    }

    char* dir = argv[1];
    char* filename = argv[2];

    find(dir, filename, argc, argv);
    exit(0);
}