/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Christoph Brill
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _SM3D_INPUT_H_
#define _SM3D_INPUT_H_

#include <SDL2/SDL.h>

#include "global.h"

#define MAX_TILT (65)

struct s_viewport_state {
    double angle_rotate = 0.0;
    double angle_tilt = 0.0;
};

extern struct s_viewport_state viewport_state;

extern void handle_mouse_motion(SDL_MouseMotionEvent &motion);
extern void handle_mouse_button_down(SDL_MouseButtonEvent &button);
extern void handle_mouse_button_up(SDL_MouseButtonEvent &button);
extern void handle_mouse_wheel(SDL_MouseWheelEvent &wheel);

#endif
