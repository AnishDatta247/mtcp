#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include "msocket.h"

void print_loading_bar(long bytes, int done)
{
    printf("\rData received:  \033[32m[%ld] bytes\033[0m\n", bytes);
    if (!done)
        printf("\033[F");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    if (argc != 6 || strcmp(argv[2], argv[4]) == 0)
    {
        printf("Usage: %s <ip2> <port2> <ip1> <port1> <filename.txt>\n", argv[0]);
        exit(1);
    }

    char *ip2 = argv[1];
    int port2 = atoi(argv[2]);
    char *ip1 = argv[3];
    int port1 = atoi(argv[4]);
    char *filename = argv[5];

    key_t key = ftok("initmsocket.c", 100);
    int usermut = semget(key, 1, IPC_CREAT | 0666);
    semctl(usermut, 0, SETVAL, 0);

    struct sembuf vop;
    vop.sem_num = 0;
    vop.sem_flg = 0;
    vop.sem_op = 1;

    sleep(2);

    int sockfd = m_socket(AF_INET, SOCK_MTP, 0, getpid());
    if (sockfd < 0)
    {
        perror("m_socket");
        exit(1);
    }
    printf("Socket created\n");

    if (m_bind(sockfd, ip2, port2, ip1, port1) == -1)
    {
        perror("m_bind");
        exit(1);
    }
    printf("Bind done\n\n");

    semop(usermut, &vop, 1);

    FILE *file;
    char *newfilename;
    newfilename = (char *)malloc(strlen(filename) + 5);
    strcpy(newfilename, "COPY");
    strcat(newfilename, filename);
    // strcat(newfilename, ".new");
    file = fopen(newfilename, "wb");

    char buf[1025];
    int bytes_read;

    int cn = 0;
    int total_read = 0;
    while (1)
    {
        // printf("Sleeping...\n");
        sleep(1);
        // memset(buf, '\0', strlen(buf));
        if ((bytes_read = m_recvfrom(sockfd, buf, sizeof(buf) - 1, 0)))
        {
            if (bytes_read < 0)
            {
                continue;
            }
            total_read += bytes_read;
            buf[bytes_read] = '\0';

            int gotlast = 0;
            if (buf[bytes_read - 1] == '.')
            {
                gotlast = 1;
                buf[bytes_read - 1] = '\0';
            }
            print_loading_bar(total_read, gotlast);
            int bytes_written = fwrite(buf, 1, strlen(buf), file);
            if (bytes_written < 0)
            {
                perror("fwrite");
                exit(1);
            }
            cn++;
            if (gotlast)
            {

                break;
            }
        }
    }

    printf("Data transfer complete\n");

    fclose(file);

    return 0;
}