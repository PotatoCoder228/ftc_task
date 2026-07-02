#include "test.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int runProcess(const char* cmd){
    if(!cmd){
        errno=EINVAL;
        perror(__func__);
    }
    FILE *fp;
    int status;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    status = pclose(fp);
    if (status == -1) {
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } 

    return -1;
}

bool testEmptyFilenames(){
    return (runProcess("./build/ftc_dump")!=0);
}