#ifndef SHARED_H
#define SHARED_H

void send_pos(int fd, int val);
int recv_pos(int fd);
void draw(int ball_x, int ball_y, int p1_y, int p2_y, int score1, int score2);

#endif

