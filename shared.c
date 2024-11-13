#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include "shared.h"

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_LENGTH 4

void send_pos(int fd, int val) {
    write(fd, &val, sizeof(int));
}

int recv_pos(int fd) {
    int val;
    int n = read(fd, &val, sizeof(int));
    if (n <= 0) {
        endwin();
        perror("Connection lost");
        exit(1);
    }
    return val;
}

void draw(int ball_x, int ball_y, int p1_y, int p2_y, int score1, int score2) {
    clear();

    for (int y = 0; y < HEIGHT; y++) {
        mvaddch(y, 0, '|');        
        mvaddch(y, WIDTH - 1, '|');
    }
    for (int x = 0; x < WIDTH; x++) {
        mvaddch(1, x, '-');
        mvaddch(HEIGHT - 1, x, '-');
    }

    for (int y = 2; y < HEIGHT - 1; y += 2) {
        mvaddch(y, WIDTH / 2, ':');
    }

    char score_str[20];
    sprintf(score_str, " %d   :   %d ", score1, score2);
    mvprintw(0, (WIDTH - strlen(score_str)) / 2, "%s", score_str);

    for (int i = 0; i < PADDLE_LENGTH; i++) {
        mvaddch(p1_y + i, 2, '#');
        mvaddch(p2_y + i, WIDTH - 3, '#');
    }

    mvaddch(ball_y, ball_x, 'O');

    refresh();
}

