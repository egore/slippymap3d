#include <iostream>

#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "tile.h"
#include "loader.h"

bool left_mouse_down = false;
bool right_mouse_down = false;

double angle(int p1x, int p1y, int p2x, int p2y) {
    float xDiff = p2x - p1x;
    float yDiff = p2y - p1y;
    return -atan2(yDiff, xDiff) * (180 / M_PI);
}

double _angle1 = 0.0;
double start_angle1 = 0.0;
double _angle2 = 0.0;
double start_angle2 = 0.0;

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
                if (left_mouse_down) {
                    _angle1 = angle(event.motion.x, event.motion.y, 512, 384) - start_angle1;
                }
                if (right_mouse_down) {
                    _angle2 = std::max(384 - event.motion.y, 0) * 70 / 384;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == 1) {
                    left_mouse_down = true;
                    start_angle1 = angle(event.button.x, event.button.y, 512, 384) - _angle1;
                } else if (event.button.button == 3) {
                    right_mouse_down = true;
                    start_angle2 = angle(event.button.x, event.button.y, 512, 384) - _angle2;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == 1) {
                    left_mouse_down = false;
                } else if (event.button.button == 3) {
                    right_mouse_down = false;
                }
                break;
            default:
                break;
        }
    }
    return true;
}

#define SIZE 150.0
#define TILE_SIZE_LAT_16 (0.00350434d/2)
#define TILE_SIZE_LON_16 (0.00549316d/2)

void render(int zoom, double latitude, double longitude) {
    Tile* center_tile = TileFactory::instance()->get_tile(zoom, latitude, longitude);

    // Clear with black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-512, 512, 384, -384, -1000, 1000);

    // Render the slippy map parts
    glEnable(GL_TEXTURE_2D);
        glRotated(_angle2, 1.0, 0.0, 0.0);
        glRotated(_angle1, 0.0, 0.0, -1.0);

        int top = -6;
        int left = -6;
        int bottom = 6;
        int right = 6;

        // Top left coordinate of the current tile
        double tile_latitude = tiley2lat(center_tile->y, zoom);
        double tile_longitude = tilex2long(center_tile->x, zoom);

        double lat_diff = (SIZE/2) + ((latitude - tile_latitude) * SIZE / TILE_SIZE_LAT_16);
        double lon_diff = (SIZE/2) + ((tile_longitude - longitude) * SIZE / TILE_SIZE_LON_16);
        //std::cout.precision(10);
        //std::cout << "Lat: " << latitude << " vs " << tile_latitude << " -> offset " << lat_diff << "px" << std::endl;
        //std::cout << "Lon: " << longitude << " vs " << tile_longitude << " -> offset " << lon_diff << "px" << std::endl;

        glPushMatrix();
            glTranslated(lon_diff, lat_diff, 0);
            Tile* current = center_tile->get(left, top);
            for (int y = top; y < bottom; y++) {
                for (int x = left; x < right; x++) {

                    if (current->texid == 0) {
                        Loader::instance()->open_image(*current);
                    }

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
    SDL_Window* window = SDL_CreateWindow("slippymap3d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Load the file matching the given coordinates

    double latitude = 50.356718;
    double longitude = 7.599485;

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

        render(16, latitude, longitude);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

