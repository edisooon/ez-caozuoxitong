#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void
flushNumBuf(int valid_num, int ni, char* numbuf) {
    if(valid_num && ni) {
        numbuf[ni] = 0;
        int num = atoi(numbuf);
        if(num % 5 == 0 || num % 6 == 0) {
            printf("%d\n", num);
        }
    }
}

int
main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(2, "Usage: sixfive file...");
        exit(1);
    }
    
    int i;
    for(i = 1; i < argc; i++) {
        char* filename = argv[i];
        int fd = open(filename, O_RDONLY);
        if(fd < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[fd]);
            exit(1);
        }

        char* seps = " -\r\t\n./,";
        char numbuf[32];
        int ni = 0;
        int valid_num = 1;   // starts as true to account for the beginning number
        char ch;

        while(read(fd, &ch, 1)==1) {
            if(strchr(seps, ch)) {
                flushNumBuf(valid_num, ni, numbuf);
                valid_num = 1;
                ni = 0;
            }else if(ch>='0' && ch<='9'){
                if(valid_num) {
                    numbuf[ni++] = ch;
                }
            }else{
                valid_num = 0;
                ni = 0;
            }
        }
        flushNumBuf(valid_num, ni, numbuf);
        
        close(fd);
    }
    exit(0);
}
