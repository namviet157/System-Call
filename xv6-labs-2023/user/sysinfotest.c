#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

int testcall() {
    struct sysinfo info;
    if(sysinfo(&info) < 0) {
        printf("sysinfotest: sysinfo failed");
        return 1;
    }
    return 0;
}

int testmem() {
    struct sysinfo info;
    uint64 before, after;
    if(sysinfo(&info) < 0) {
        printf("sysinfotest: sysinfo failed");
        return 1;
    }
    before = info.freemem;
    void *mem = sbrk(4096);
    if (mem == (void *)-1) {
        printf("sysinfotest: sbrk failed\n");
        return 1;
    }

    if (sysinfo(&info) < 0) {
        printf("sysinfotest: sysinfo failed\n");
        return 1;
    }

    after = info.freemem;

    if (after >= before) {
        printf("sysinfotest: free memory did not decrease\n");
        return 1;
    }

    sbrk(-4096);
    return 0;
}

int testproc() {
    struct sysinfo info;
    uint64 before, after;
    int pid;

    if (sysinfo(&info) < 0) {
        printf("sysinfotest: sysinfo failed\n");
        return 1;
    }
    before = info.nproc;

    pid = fork();
    if (pid < 0) {
        printf("sysinfotest: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        exit(0);
    }

    if (sysinfo(&info) < 0) {
        printf("sysinfotest: sysinfo failed\n");
        exit(1);
    }
    after = info.nproc;

    if (after <= before) {
        printf("sysinfotest: process count did not increase\n");
        exit(1);
    }

    wait(0);
    return 0;
}

int main(int argc, char argv[]) {
    printf("sysinfotest: running tests...\n");

    if(testcall() != 0 || testmem() != 0 || testproc() != 0) {
        return 1;
    }

    printf("sysinfotest: OK\n");

    return 0;
}