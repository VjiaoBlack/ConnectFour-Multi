#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

int r, c;
char* board;

int rows = 0;
int cols = 0;


int isfirst_global;
int p1row;
int p1col;
int p2row;
int p2col;

char output_buffer[256];

char p1msg[256];
char p2msg[256];
int p1_new_msg;
int p2_new_msg;

void *handle(int *pnewsock) {
    int connfd = *pnewsock;
    int n = 0;
    char recvBuff[1024];

    // sees if this is the first client to connect
    int is_first = isfirst_global;
    if (is_first) {
        isfirst_global = 0;
    }

    while(1) {
        // sends board
        // for (r = 0; r < rows; r++) {
        //     for (c = 0; c < cols; c++) {
        //         write(connfd, &board[c + r * cols], 1);
        //     }
        //     write(connfd, "\n", 1);
        // }

        if (is_first) {
            if (p2_new_msg) {

                snprintf(output_buffer, 256, "-- p2 says: %sp1 >", p2msg);
                write(connfd, output_buffer, 256);

                p2_new_msg = 0;
            } else {
                write(connfd, "p1 > ", 20);
            }
        } else {
            if (p1_new_msg) {

                snprintf(output_buffer, 256, "-- p1 says: %sp2 >", p1msg);
                write(connfd, output_buffer, 256);

                p1_new_msg = 0;
            } else {
                write(connfd, "p2 > ", 20);
            }
        }


        // gets input from boards
        if ( (n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
            // prints out stuff!!!
            // recvBuff[n] = 0;
            // note: recvBuff ends with \n due to how our programs works
            if (is_first) {
                strcpy(p1msg,  recvBuff);
                printf("p1 sends: %s", p1msg);
                p1_new_msg = 1;

            } else {
                strcpy(p2msg, recvBuff);
                printf("p2 sends: %s", p2msg);
                p2_new_msg = 1;

            }


        }

    }

    // return NULL;
}

int main(int argc, char *argv[]) {


    pthread_t thread;

    // setup game
    if (argc != 3) {
        printf("\nInvalid number of arguements.\n");
        printf("Usage: ./server <rows> <cols>\n\n");
        exit(1);
    }

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);

    board = (char*) malloc(sizeof(char) * cols * rows);

    for (r = 0; r < rows; r++) {
        for (c = 0; c < cols; c++) {
            board[c + r * cols] = 'O';
        }
    }

    // intialize global variables for the players
    isfirst_global = 1;
    // p*msg should be initialized cuz of the [256] that follows them;

    p1_new_msg = 0;
    p2_new_msg = 0;

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 2 player only
    listen(listenfd, 2);

    while(1) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        printf("Got a connection from %s on port %d\n", inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));

        if (pthread_create(&thread, NULL, handle, &connfd) != 0) {
            fprintf(stderr, "Failed to create thread\n");
        }

        // wait 1 second before trying to get a new board.
        sleep(1);
    }







}

