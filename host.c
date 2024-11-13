#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "host.h"
#include "shared.h"

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_LENGTH 4
#define PORT 8080

void run_host() {
    initscr();
    noecho();
    cbreak();
    timeout(0);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    int p1_y = HEIGHT / 2 - PADDLE_LENGTH / 2;
    int p2_y = p1_y;
    int ball_x = WIDTH / 2, ball_y = HEIGHT / 2;
    int dx = 1, dy = 1;
    int score1 = 0, score2 = 0;

    struct sockaddr_in host_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int joiner_fd;
    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = INADDR_ANY;
    host_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr));
    listen(sockfd, 1);
    printw("Waiting for joiner to connect...\n");
    refresh();
    joiner_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

    if (joiner_fd < 0) {
        endwin();
        perror("Accept failed");
        exit(1);
    }
    while (1) {

        int ch = getch();
        if (ch == 'w' && p1_y > 1) p1_y--;
        if (ch == 's' && p1_y + PADDLE_LENGTH < HEIGHT - 1) p1_y++;

        p2_y = recv_pos(joiner_fd);


        ball_x += dx;
        ball_y += dy;

        if (ball_y <= 1 || ball_y >= HEIGHT - 2) dy *= -1;

        if (ball_x == 3 && ball_y >= p1_y && ball_y <= p1_y + PADDLE_LENGTH) dx *= -1;
        if (ball_x == WIDTH - 4 && ball_y >= p2_y && ball_y <= p2_y + PADDLE_LENGTH) dx *= -1;

        if (ball_x <= 0) {
            score2++;
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
        }
        if (ball_x >= WIDTH - 1) {
            score1++;
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
        }

        send_pos(joiner_fd, p1_y);
        send_pos(joiner_fd, ball_x);
        send_pos(joiner_fd, ball_y);
        send_pos(joiner_fd, score1);
        send_pos(joiner_fd, score2);

        draw(ball_x, ball_y, p1_y, p2_y, score1, score2);

        usleep(30000);
    }

    endwin();
    close(joiner_fd);
    close(sockfd);
}
