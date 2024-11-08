#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_LENGTH 4
#define PORT 8080
#define host_IP "127.0.0.1" 

int host_fd;

void send_int(int val) {
    write(host_fd, &val, sizeof(int));
}

int recv_int() {
    int val;
    int n = read(host_fd, &val, sizeof(int));
    if (n <= 0) {
        endwin();
        perror("Connection lost");
        exit(1);
    }
    return val;
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

    int ball_x = WIDTH / 2, ball_y = HEIGHT / 2;
    int p1_y = HEIGHT / 2 - PADDLE_LENGTH / 2;
    int p2_y = HEIGHT / 2 - PADDLE_LENGTH / 2;
    int score1 = 0, score2 = 0;

    struct sockaddr_in host_addr;
    host_fd = socket(AF_INET, SOCK_STREAM, 0);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, host_IP, &host_addr.sin_addr);

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

        send_int(p2_y);

        p1_y   = recv_int();
        ball_x = recv_int();
        ball_y = recv_int();
        score1 = recv_int();
        score2 = recv_int();

        draw(ball_x, ball_y, p1_y, p2_y, score1, score2);
        usleep(30000);
    }

    endwin();
    close(host_fd);
    return 0;
}
