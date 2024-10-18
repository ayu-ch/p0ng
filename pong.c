#include <ncurses.h>
#include <unistd.h>

int main() {
    initscr();
    noecho();
    cbreak();
    timeout(0);
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int paddle_height = 10;
    int p1_y = max_y / 2 - paddle_height / 2;
    int p2_y = p1_y;
    int ball_x = max_x / 2, ball_y = max_y / 2;
    int dx = 1, dy = 1;

    while (1) {
        clear();

        int ch = getch();
        if (ch == 'q') break;
        if (ch == 'w' && p1_y > 1) p1_y--;
        if (ch == 's' && p1_y + paddle_height < max_y - 1) p1_y++;
        if (ch == KEY_UP && p2_y > 1) p2_y--;
        if (ch == KEY_DOWN && p2_y + paddle_height < max_y - 1) p2_y++;

        ball_x += dx;
        ball_y += dy;

        if (ball_y <= 1 || ball_y >= max_y - 2) dy *= -1;

        if (ball_x == 3 && ball_y >= p1_y && ball_y <= p1_y + paddle_height) dx *= -1;
        if (ball_x == max_x - 4 && ball_y >= p2_y && ball_y <= p2_y + paddle_height) dx *= -1;

        if (ball_x <= 0 || ball_x >= max_x - 1) {
            ball_x = max_x / 2;
            ball_y = max_y / 2;
        }

        for (int i = 0; i < paddle_height; i++) {
            mvaddch(p1_y + i, 2, '|');
            mvaddch(p2_y + i, max_x - 3, '|');
        }

        mvaddch(ball_y, ball_x, 'O');

        refresh();
        usleep(30000);
    }

    endwin();
    return 0;
}
