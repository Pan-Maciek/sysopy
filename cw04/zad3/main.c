#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define on(signal, block)\
void on_##signal(int sig, siginfo_t* info, void* __) { block; _exit(0); }
#define no(signal) {\
	struct sigaction act = {.sa_flags=SA_SIGINFO,.sa_sigaction=on_##signal};\
	sigaction(signal, &act, NULL); }
#define test if (fork() == 0) { 
#define end ;exit(0); } else wait(NULL);

on(SIGSEGV, printf("si_addr (%p)\n", info->si_addr));
on(SIGINT, printf("si_uid (%u)\n", info->si_uid));
on(SIGFPE, printf("si_code (%u)\n", info->si_code));

int main() {
	no(SIGSEGV);
	no(SIGINT);
	no(SIGFPE);

	int zero = 0;
	test int x = 1 / zero end
	test {
		int* memory_violation = (int*) 12345; 
		*memory_violation = 213;
	} end
	test raise(SIGINT) end
}
