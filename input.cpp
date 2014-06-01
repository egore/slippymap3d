#include <algorithm>

#include "input.h"
#include "global.h"

struct s_input_state {
    bool left_mouse_down = false;
    bool right_mouse_down = false;
    bool middle_mouse_down = false;
} input_state;

struct s_viewport_state viewport_state;

double start_angle1 = 0.0;
double start_angle2 = 0.0;

double angle(int p1x, int p1y, int p2x, int p2y) {
    float xDiff = p2x - p1x;
    float yDiff = p2y - p1y;
    return -atan2(yDiff, xDiff) * (180 / M_PI);
}

void handle_mouse_motion(SDL_MouseMotionEvent &motion) {
    if (input_state.left_mouse_down) {
        viewport_state._angle1 = angle(motion.x, motion.y, (window_state.width / 2), (window_state.height / 2)) - start_angle1;
    }
    if (input_state.right_mouse_down) {
        viewport_state._angle2 = std::max((window_state.height / 2) - motion.y, 0) * MAX_TILT / (window_state.height / 2);
    }
    if (input_state.middle_mouse_down) {
        long double _cos = std::cos(viewport_state._angle1 * M_PI / 180);
        long double _sin = std::sin(viewport_state._angle1 * M_PI / 180);
        player_state.latitude += Y_16 * (motion.yrel * _cos + motion.xrel * _sin);
        player_state.longitude -= X_16 * (motion.xrel * _cos - motion.yrel * _sin);
    }
}

void handle_mouse_button_down(SDL_MouseButtonEvent &button) {
    if (button.button == 1) {
        input_state.left_mouse_down = true;
        start_angle1 = angle(button.x, button.y, (window_state.width / 2), (window_state.height / 2)) - viewport_state._angle1;
    } else if (button.button == 3) {
        input_state.right_mouse_down = true;
        start_angle2 = angle(button.x, button.y, (window_state.width / 2), (window_state.height / 2)) - viewport_state._angle2;
    } else if (button.button == 2) {
        input_state.middle_mouse_down = true;
    }
}

void handle_mouse_button_up(SDL_MouseButtonEvent &button) {
    if (button.button == 1) {
        input_state.left_mouse_down = false;
    } else if (button.button == 3) {
        input_state.right_mouse_down = false;
    } else if (button.button == 2) {
        input_state.middle_mouse_down = false;
    }
}
