#ifndef _SM3D_INPUT_H_
#define _SM3D_INPUT_H_

#include <SDL2/SDL.h>

#define MAX_TILT (65)

#define Y_16 (0.000012)
#define X_16 (0.000019)

struct s_viewport_state {
    double _angle1 = 0.0;
    double _angle2 = 0.0;
};

extern struct s_viewport_state viewport_state;

extern void handle_mouse_motion(SDL_MouseMotionEvent &motion);
extern void handle_mouse_button_down(SDL_MouseButtonEvent &button);
extern void handle_mouse_button_up(SDL_MouseButtonEvent &button);

#endif
