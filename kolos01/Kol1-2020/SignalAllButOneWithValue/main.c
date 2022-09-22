#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>


void sighandler(int sig, siginfo_t* info, void* ucontext) {
    printf("Sygnal %d, wartosc %d\n", sig, info->si_value.sival_int);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    

    int child = fork();
    if(child == 0) {
        action.sa_flags = SA_SIGINFO;
        sigfillset(&action.sa_mask);
        sigdelset(&action.sa_mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &action.sa_mask, NULL);
        sigemptyset(&action.sa_mask);
        sigaddset(&action.sa_mask, SIGUSR1);
        sigaction(SIGUSR1, &action, NULL);
        sleep(2);
    }
    else {
        union sigval val;
        val.sival_int = atoi(argv[1]);
        sleep(1);
        sigqueue(child, atoi(argv[2]), val);
        wait(NULL);
    }

    return 0;
}