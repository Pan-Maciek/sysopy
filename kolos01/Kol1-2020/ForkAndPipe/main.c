#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * Funkcja 'spawn_child' powinna wpierw utworzyc proces
 * potomny. Proces macierzysty powinien nastepnie:
 *   - skopiowac deskryptor plikow 'out_fd' na deskryptor
 *     standardowego wyjscia,
 *   - zamknac deskryptor 'out_fd'.
 *
 * Proces potomny powinien:
 *   - skopiowac deskryptor plikow 'in_fd' na deskryptor
 *     standardowego wejscia,
 *   - zamknac deskryptor 'in_fd'.
 *
 * W procesie potomnym funkcja 'spawn_child' powinna
 * zwrocic wartosc 0. W procesie macierzystym funkcja
 * 'spawn_child' powinna zwrocic PID potomka.
 *
 */
pid_t spawn_child(int in_fd, int out_fd){
  int pid = fork();
  if (pid == -1)
    return -1;

  if (pid == 0) { // child
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);
    return 0;
  } else { // parent
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
    return pid;
  }
}


int main(void) {
    int fds[2];

    srand(0);
    pipe(fds);
    pid_t pid = spawn_child(fds[0], fds[1]);
    if(pid){
        if(fcntl(fds[1], F_GETFD) != -1) return 0;
        printf("spawned child %d\n", pid);
        printf("magic = %d\n", rand() % 8192);
    } else {
        if(fcntl(fds[0], F_GETFD) != -1) return 0;
        printf("Child: my pid %d\n", getpid());
        char *line = NULL;
        size_t n = getline(&line, &n, stdin);
        printf("Received from parent:\t%s", line);
        n = getline(&line, &n, stdin);
        printf("Received from parent:\t%s", line);
        free(line);
    }

    return 0;
}
