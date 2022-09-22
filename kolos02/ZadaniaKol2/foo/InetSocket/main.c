#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Funkcja 'ipconnect' przyjmuje:
 *   - soket domeny AF_INET: 'sock',
 *   - adres IP w postaci kropkowo dziesietnej: 'ip_addr',
 *   - numer portu w lokalnym porzadku bajtow: 'port'.
 *
 * Funkcja 'ipconnect' powinna polaczyc soket 'sock' z
 * adresem 'ip_addr' i portem 'port'. Funkcja powinna
 * zwrocic 0 w przypadku sukcesu oraz -1 w przypadku bledu
 * polaczenia.
 */
int ipconnect(int sock, char *ip_addr, short port) {
    // Uzupelnij cialo funkcji ipconnect zgodnie z
    // komentarzem powyzej.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &addr.sin_addr) <= 0) return -1;
    if (connect(sock, (struct sockaddr*) &addr, sizeof addr) < 0) return -1;
    return 0;
}

int main(int argc, char *argv[]) {
    int sock;
    int buf[32];
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if(ipconnect(sock, "127.0.0.1", 7474)) return -1;
    read(sock, buf, 32);
    write(STDOUT_FILENO, buf, 32);
    printf("\n");
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
