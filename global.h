#ifndef _SM3D_GLOBAL_H_
#define _SM3D_GLOBAL_H_

struct s_window_state {
    int width;
    int height;
};

struct s_player_state {
    double latitude = 50.356718;
    double longitude = 7.599485;
};

extern struct s_window_state window_state;
extern struct s_player_state player_state;

#endif
