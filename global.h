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

#ifndef _SM3D_GLOBAL_H_
#define _SM3D_GLOBAL_H_

/**
 * @brief the size of a tile in pixels
 */
#define TILE_SIZE (150.0)
#define TILE_SIZE_LAT_16 (0.00350434/2) /* TODO hardcoding this does not work due to merkator projection */
/**
 * Determines the longitude size of a tile at zoom 16
 */
#define TILE_SIZE_LON_16 (0.0054931640625/2)

/**
 * @brief holds the state of the window's width and height
 */
struct s_window_state {
    /**
     * @brief width of the window
     */
    int width;
    /**
     * @brief height of the window
     */
    int height;
};

/**
 * @brief hold the players current position
 */
struct s_player_state {
    double latitude = 50.356718;
    double longitude = 7.599485;
    int zoom = 16;
};

extern struct s_window_state window_state;
extern struct s_player_state player_state;

#endif
