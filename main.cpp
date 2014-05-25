#include <iostream>
#include <sstream>
#include <cmath>

#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

int long2tilex(double lon, int z)  {
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int lat2tiley(double lat, int z) {
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

std::string get_filename(int zoom, double latitude, double longitude) {
    std::stringstream filename;
    filename << "../16/" << long2tilex(7.599485, 16) << '/' << lat2tiley(50.356718, 16) << ".png";
    return filename.str();
}

bool left_mouse_down = false;

double angle(int p1x, int p1y, int p2x, int p2y) {
    float xDiff = p2x - p1x;
    float yDiff = p2y - p1y;
    return -atan2(yDiff, xDiff) * (180 / M_PI);
}

double _angle = 0.0;
double start_angle = 0.0;

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
                    _angle = angle(event.motion.x, event.motion.y, 512, 384) - start_angle;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == 1) {
                    left_mouse_down = true;
                    start_angle = angle(event.button.x, event.button.y, 512, 384) - _angle;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == 1) {
                    left_mouse_down = false;
                }
                break;
            default:
                break;
        }
    }
    return true;
}

int load_image(std::string filename) {
    SDL_Surface *texture = IMG_Load(filename.c_str());
    GLuint texid;

    char tmp[4096];
    getcwd(tmp, 4096);
    std::cout << "Loading texture " << filename << " from directory " << tmp << ' ';

    if (texture) {
        if (SDL_MUSTLOCK(texture)) {
            SDL_LockSurface(texture);
        }

        GLenum texture_format;
        if (texture->format->BytesPerPixel == 4) {
            if (texture->format->Rmask == 0x000000ff) {
                texture_format = GL_RGBA;
            } else {
                texture_format = GL_BGRA;
            }
        } else if (texture->format->BytesPerPixel == 3) {
            if (texture->format->Rmask == 0x000000ff) {
                texture_format = GL_RGB;
            } else {
                texture_format = GL_BGR;
            }
        } else {
            std::cout << "INVALID (" << SDL_GetPixelFormatName(texture->format->format);
            SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_BGR24);
            SDL_Surface* tmp = SDL_ConvertSurface(texture, format, 0);
            texture_format = GL_BGR;
            if (SDL_MUSTLOCK(texture)) {
                SDL_UnlockSurface(texture);
            }
            SDL_FreeSurface(texture);
            texture = tmp;
            if (SDL_MUSTLOCK(texture)) {
                SDL_LockSurface(texture);
            }
            std::cout << " -> " << SDL_GetPixelFormatName(texture->format->format) << ") ";
        }

        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D, texid);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->w, texture->h, 0, texture_format, GL_UNSIGNED_BYTE, texture->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (SDL_MUSTLOCK(texture)) {
            SDL_UnlockSurface(texture);
        }
        SDL_FreeSurface(texture);

        std::cout << "SUCCESS" << std::endl;
    } else {
        std::cout << "FAILED" << std::endl;
        return -1;
    }

    return texid;
}

#define SIZE 110.0

void render(GLuint texid) {
    // Clear with black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-512, 512, 384, -384, -1, 1);

    // Render the slippy map parts
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texid);
    glRotated(_angle, 0.0, 0.0, -1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0); glVertex3f(-SIZE, SIZE, 0);
        glTexCoord2f(1.0, 1.0); glVertex3f(SIZE, SIZE, 0);
        glTexCoord2f(1.0, 0.0); glVertex3f(SIZE, -SIZE, 0);
        glTexCoord2f(0.0, 0.0); glVertex3f(-SIZE, -SIZE, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
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
    std::string filename = get_filename(16, lat2tiley(50.356718, 16), long2tilex(7.599485, 16));
    int texid = load_image(filename);
    if (texid < 0) {
        goto out;
    }

    while(true) {
        if (!poll()) {
            break;
        }

        render(texid);
        SDL_GL_SwapWindow(window);
    }

out:
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

