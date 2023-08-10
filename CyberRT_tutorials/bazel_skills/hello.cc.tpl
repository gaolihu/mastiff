// Package: @PACKAGE@
// @WARNING@

#include "auto_gen.h"

#ifndef REPO_INFO
#define REPO_INFO "unknow"
#endif

#include <stdio.h>

int main() {
    printf("@NAME@ say: Hello World!\n");
    printf("@NAME@'s greeting: @GREETING@...\n");
    printf("\nRepo infomation { %s }\n\n", REPO_INFO);
    return 0;
}
