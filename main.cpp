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

#include <iostream>

#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "tile.h"
#include "loader.h"
#include "input.h"
#include "global.h"

#define SIZE (150.0)
#define TILE_SIZE_LAT_16 (0.00350434d/2)
#define TILE_SIZE_LON_16 (0.00549316d/2)


/**
 * @brief poll for events
 * @return true, if the program should end
 */
bool poll() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                }
                break;
            case SDL_MOUSEMOTION:
                handle_mouse_motion(event.motion);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handle_mouse_button_down(event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                handle_mouse_button_up(event.button);
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        window_state.width = event.window.data1;
                        window_state.height = event.window.data2;
                        break;
                }
                break;
            default:
                break;
        }
    }
    return true;
}

void render(int zoom, double latitude, double longitude) {
    Tile* center_tile = TileFactory::instance()->get_tile(zoom, latitude, longitude);

    // Clear with black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(window_state.width / 2), (window_state.width / 2), (window_state.height / 2), -(window_state.height / 2), -1000, 1000);

    // Rotate and and tilt the world geometry
    glRotated(viewport_state._angle2, 1.0, 0.0, 0.0);
    glRotated(viewport_state._angle1, 0.0, 0.0, -1.0);

    // Render the slippy map parts
    glEnable(GL_TEXTURE_2D);

        // Top left coordinate of the current tile
        double tile_latitude = tiley2lat(center_tile->y, zoom);
        double tile_longitude = tilex2long(center_tile->x, zoom);

        // Offset of the current tile from the center of the screen
        double lat_diff = (SIZE/2) + ((latitude - tile_latitude) * SIZE / TILE_SIZE_LAT_16);
        double lon_diff = (SIZE/2) + ((tile_longitude - longitude) * SIZE / TILE_SIZE_LON_16);

        glPushMatrix();
            glTranslated(lon_diff, lat_diff, 0);

            static const int top = -6;
            static const int left = -6;
            static const int bottom = 6;
            static const int right = 6;

            // Start 'left' and 'top' tiles from the center tile and render down to 'bottom' and
            // 'right' tiles from the center tile
            Tile* current = center_tile->get(left, top);
            for (int y = top; y < bottom; y++) {
                for (int x = left; x < right; x++) {

                    // If the texid is set to zero the download was finished successfully and
                    // the tile can be rendered now properly
                    if (current->texid == 0) {
                        Loader::instance()->open_image(*current);
                    }

                    // Render the tile itself at the correct position
                    glPushMatrix();
                        glTranslated(x*SIZE*2, y*SIZE*2, 0);
                        glBindTexture(GL_TEXTURE_2D, current->texid);
                        glBegin(GL_QUADS);
                            glTexCoord2f(0.0, 1.0); glVertex3f(-SIZE, SIZE, 0);
                            glTexCoord2f(1.0, 1.0); glVertex3f(SIZE, SIZE, 0);
                            glTexCoord2f(1.0, 0.0); glVertex3f(SIZE, -SIZE, 0);
                            glTexCoord2f(0.0, 0.0); glVertex3f(-SIZE, -SIZE, 0);
                        glEnd();
                    glPopMatrix();
                    current = current->get_west();
                }
                current = current->get(-(std::abs(left) + std::abs(right)), 1);
            }
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Draw the players avatar at the center of the screen
    glColor3d(1.0, 0.5, 0.0);
    glBegin(GL_TRIANGLES);
        glVertex3f(-10,  15, 1);
        glVertex3f( 10,  15, 1);
        glVertex3f(  0, -10, 1);
    glEnd();
    glColor3d(1.0, 1.0, 1.0);
}

int main(int argc, char **argv) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Could not initialize SDL video: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an OpenGL window
    SDL_Window* window = SDL_CreateWindow("slippymap3d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Could not create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_GetWindowSize(window, &window_state.width, &window_state.height);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long base_time = spec.tv_sec * 1000 + round(spec.tv_nsec / 1.0e6);
    int frames = 0;
    while(true) {
        if (!poll()) {
            break;
        }
        frames++;

        clock_gettime(CLOCK_REALTIME, &spec);
        long time_in_mill = spec.tv_sec * 1000 + round(spec.tv_nsec / 1.0e6);
        if ((time_in_mill - base_time) > 1000.0) {
            std::cout << frames * 1000.0 / (time_in_mill - base_time) << " fps" << std::endl;
            base_time = time_in_mill;
            frames=0;
        }

        render(16, player_state.latitude, player_state.longitude);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

