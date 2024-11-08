#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_LENGTH 4
#define PORT 8080

int client_fd;

void send_int(int val) {
    write(client_fd, &val, sizeof(int));
}

int recv_paddle_pos() {
    int p2_y;
    read(client_fd, &p2_y, sizeof(int));
    return p2_y;
}

void draw(int ball_x, int ball_y, int p1_y, int p2_y, int score1, int score2) {
    clear();
    mvprintw(0, WIDTH/2 - 5, "%d : %d", score1, score2);

    for (int i = 0; i < PADDLE_LENGTH; i++) {
        mvaddch(p1_y + i, 2, '|');
        mvaddch(p2_y + i, WIDTH - 3, '|');
    }

    mvaddch(ball_y, ball_x, 'O');
    refresh();
}

int main() {
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

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = INADDR_ANY;
    host_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr));
    listen(sockfd, 1);
    printw("Waiting for client to connect...\n");
    refresh();
    client_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

    if (client_fd < 0) {
        endwin();
        perror("Accept failed");
        exit(1);
    }
    while (1) {

        int ch = getch();
        if (ch == 'w' && p1_y > 1) p1_y--;
        if (ch == 's' && p1_y + PADDLE_LENGTH < HEIGHT - 1) p1_y++;

        p2_y = recv_paddle_pos();


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

        send_int(p1_y);
        send_int(ball_x);
        send_int(ball_y);
        send_int(score1);
        send_int(score2);

        draw(ball_x, ball_y, p1_y, p2_y, score1, score2);

        usleep(30000);
    }

    endwin();
    close(client_fd);
    close(sockfd);

    return 0;
}
