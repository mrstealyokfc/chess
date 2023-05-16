#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include "command.h"

extern bool shouldContinue;

void process_command(){
    char *line = NULL;
    size_t len = 0;
    getline(&line,&len,stdin);

    if(strcmp("exit\n",line)==0)
        shouldContinue=false;

    free(line);
}
