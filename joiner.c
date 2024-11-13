#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "joiner.h"
#include "shared.h"

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_LENGTH 4
#define PORT 8080
#define HOST_IP "127.0.0.1" 

void run_joiner() {
    initscr();
    noecho();
    cbreak();
    timeout(0);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    int ball_x = WIDTH / 2, ball_y = HEIGHT / 2;
    int p1_y = HEIGHT / 2 - PADDLE_LENGTH / 2;
    int p2_y = HEIGHT / 2 - PADDLE_LENGTH / 2;
    int score1 = 0, score2 = 0;

    struct sockaddr_in host_addr;
    int host_fd;
    host_fd = socket(AF_INET, SOCK_STREAM, 0);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST_IP, &host_addr.sin_addr);

    printw("Connecting to host...\n");
    refresh();

    if (connect(host_fd, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0) {
        endwin();
        perror("connect");
        exit(1);
    }

    while (1) {
        int ch = getch();
        if (ch == 'w' && p2_y > 1) p2_y--;
        if (ch == 's' && p2_y + PADDLE_LENGTH < HEIGHT - 1) p2_y++;

        send_pos(host_fd, p2_y);

        p1_y   = recv_pos(host_fd);
        ball_x = recv_pos(host_fd);
        ball_y = recv_pos(host_fd);
        score1 = recv_pos(host_fd);
        score2 = recv_pos(host_fd);

        draw(ball_x, ball_y, p1_y, p2_y, score1, score2);
        usleep(30000);
    }

    endwin();
    close(host_fd);
}
