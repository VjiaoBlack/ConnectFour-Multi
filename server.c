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

// TODO: make sure you only accept data when it's your turn!
// create a whose_turn variable or something...
// or pause after send, and just WAIT for receive, and do a non blocking void get char

int r, c;
char* board;

int rows = 0;
int cols = 0;

// this talks about who's first.
int isfirst_global;
int p1row;
int p1col;
int p2row;
int p2col;

// keeps track of whose turn it is.
int p1turn;

char output_buffer[256];

char p1msg[256];
char p2msg[256];
int p1_new_msg;
int p2_new_msg;

int intro_p1;
int intro_p2;

int players;

void *handle(int *pnewsock) {
    int connfd = *pnewsock;
    int n = 0;
    char recvBuff[256];

    // sees if this is the first client to connect
    int is_first = isfirst_global;
    if (is_first) {
        isfirst_global = 0;
    }
    while (1) {
    if (is_first) { // p1

        if (intro_p1) {
            strcpy(p2msg,"Welcome!\n");
            intro_p1 = 0;
            p2_new_msg = 1;
        }
        // wait for turn
        while (!p1turn && !p2_new_msg);
        printf("sent to p1: %s, new?:%d\n", p2msg, p2_new_msg);
        p2_new_msg = 0;

        // send board
        write(connfd,p2msg, 255);

        // wait for input
        while ((n = read(connfd, recvBuff, 255) > 0) && strlen(recvBuff) == 0) {
            printf("null\n");
            if (recvBuff[0] != 0)
                p1_new_msg = 1;
        }
        printf("%lu: %s\n", strlen(recvBuff), recvBuff);


        // set input to p1msg;
        strcpy(p1msg, recvBuff);

        p1turn = 0;

        p1_new_msg = 1;
    } else { // p2
        if (intro_p2) {
            // wait for p1's message to start.
            while (!p1_new_msg);
            intro_p2 = 0;
        }

        // wait for turn;
        while (p1turn && !p1_new_msg);


        printf("sent to p2: %s, new?:%d\n", p1msg, p1_new_msg);
        p1_new_msg  = 0;

        // send board
        write(connfd,p1msg,255);

        // wait for input
        while ((n = read(connfd, recvBuff, 255) > 0)  && strlen(recvBuff) == 0) {
            printf("null\n");
        }
        printf("%lu: %s\n", strlen(recvBuff), recvBuff);
            p2_new_msg = 1;

        // set input to p2msg;
        strcpy(p2msg, recvBuff);

        p1turn = 1;

    }



    }
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

    intro_p1 = 1;
    intro_p2 = 1;
    players = 0;

    for (r = 0; r < rows; r++) {
        for (c = 0; c < cols; c++) {
            board[c + r * cols] = 'O';
        }
    }

    // intialize global variables for the players
    isfirst_global = 1;

    // p1 goes first
    p1turn = 1;

    // p*msg should already be initialized cuz of the [256] that follows them;
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
        players++;

        printf("Got a connection from %s on port %d\n", inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));

        if (pthread_create(&thread, NULL, handle, &connfd) != 0) {
            fprintf(stderr, "Failed to create thread\n");
        }

        // wait 1 second before trying to get a new board.
        sleep(1);
    }







}

