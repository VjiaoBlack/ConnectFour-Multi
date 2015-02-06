A very, very simple implementation of connect-4, multiplayer, in C.

player1 runs the server first, then player2 can join.




void *handle(int *pnewsock) {
    int connfd = *pnewsock;
    int n = 0;
    char recvBuff[1024];

    // sees if this is the first client to connect
    int is_first = isfirst_global;
    if (is_first) {
        isfirst_global = 0;
        p1fd = *pnewsock;
    } else {
        p2fd = *pnewsock;
    }

    while(1) {
        // sends board
        // for (r = 0; r < rows; r++) {
        //     for (c = 0; c < cols; c++) {
        //         write(connfd, &board[c + r * cols], 1);
        //     }
        //     write(connfd, "\n", 1);
        // }

        // gets data from sockets, prints to stdout of server.

        // if (game_begin && is_first) {

        //     write(connfd, "Waiting for 2nd player...<Enter>\n", 64);

        //     while(players < 2);

        //     write(connfd, "Game start!\n",32);

        //     p1turn = 1;
        //     game_begin = 0;
        //     continue;
        // } else


        // sends messages to buffers.
        if (game_begin_p1 && is_first) {
            write(connfd, "You are p1, game start!\n", 32);
            game_begin_p1 = 0;
            continue;
        }
        if (game_begin_p2 && !is_first) {
            write(connfd, "You are p2, game start!\n", 32);
            game_begin_p2 = 0;
            continue;
        }

        if (is_first) {
            if (p2_new_msg) {

                snprintf(output_buffer, 256, "p2: %s", p2msg);
                printf("sent to p1 %s", output_buffer);

                write(connfd, output_buffer, 256);

                p2_new_msg = 0;


            }

            // while(!p1turn); // stops program while isnt its turn

        } else {
            if (p1_new_msg) {

                snprintf(output_buffer, 256, "p1: %s", p1msg);

                printf("sent to p2 |%s", output_buffer);
                write(connfd, output_buffer, 256);

                p1_new_msg = 0;


            }


            // while(p1turn); // stops program while is turn.
        }

        write(connfd, "## server:Waiting for input\n", 32);
        if ((n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
            // prints out stuff to server screen
            // recvBuff[n] = 0;
            // note: recvBuff ends with \n due to how our programs works
            if (is_first) {
                strcpy(p1msg,  recvBuff);
                printf("p1 sends: %s", p1msg);
                p1_new_msg = 1;
                p1turn = 0;

            } else {
                strcpy(p2msg, recvBuff);
                printf("p2 sends: %s", p2msg);
                p2_new_msg = 1;
                p1turn = 1;
            }
        }






    }

    // return NULL;
}
