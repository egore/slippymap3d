#include <iostream>
#include <sstream>
#include <cmath>
#include <map>
#include <boost/filesystem.hpp>

#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include <curl/curl.h>

CURL* curl = 0;

class Tile;

std::map<std::string, Tile*> tiles;

std::string tile_id(int zoom, int x, int y) {
    std::stringstream ss;
    ss << zoom << "/" << x << "/" << y;
    return ss.str();
}

Tile *get_tile(int zoom, int x, int y);

class Tile {
public:
    int zoom;
    int x;
    int y;
    GLuint texid;
    Tile* get(int x_diff, int y_diff) {
        return get_tile(zoom, x+x_diff, y+y_diff);
    }
    Tile* get_east() {
        return get(-1, 0);
    }
    Tile* get_north() {
        return get(0, -1);
    }
    Tile* get_south() {
        return get(0, 1);
    }
    Tile* get_west() {
        return get(1, 0);
    }
};

void load_image(Tile& tile);

Tile *get_tile(int zoom, int x, int y) {
    std::string id = tile_id(zoom, x, y);
    std::map<std::string, Tile*>::iterator tile_iter = tiles.find(id);
    if (tile_iter != tiles.end()) {
        return tile_iter->second;
    }
    Tile* tile = new Tile();
    tile->zoom = zoom;
    tile->x = x;
    tile->y = y;
    // TODO use dummy texture first and load async
    load_image(*tile);
    tiles[id] = tile;
    return tile;
}

int long2tilex(double lon, int z)  {
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int lat2tiley(double lat, int z) {
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

Tile* get_tile(int zoom, double latitude, double longitude) {
    int x = long2tilex(longitude, zoom);
    int y = lat2tiley(latitude, zoom);
    return get_tile(zoom, x, y);
}

std::string get_filename(Tile &tile) {
    std::stringstream filename;
    filename << tile.zoom << "/" << tile.x << '/' << tile.y << ".png";
    return filename.str();
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool download_image(Tile& tile) {
    std::stringstream dirname;
    dirname << "../" << tile.zoom << "/" << tile.x;
    std::string dir = dirname.str();
    boost::filesystem::create_directories(dir);
    std::string filename = get_filename(tile);
    std::string url = "http://localhost/osm_tiles/" + filename;
    std::string file = "../" + filename;
    FILE* fp = fopen(file.c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    CURLcode res = curl_easy_perform(curl);
    fclose(fp);
    if (res != CURLE_OK) {
        std::cerr << "Failed to download: " << url << std::endl;
        return false;
    } else {
        std::cout << "Downloaded " << file << std::endl;
        return true;
    }
}

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

void load_image(Tile& tile) {

    std::string filename = "../" + get_filename(tile);
    if (!boost::filesystem::exists(filename)) {
        if (!download_image(tile)) {
            tile.texid = 0;
            return;
        }
    }

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
        tile.texid = 0;
    }

    tile.texid = texid;
}

#define SIZE 150.0

void render(Tile * center_tile) {
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

        Tile* current = center_tile->get(left, top);
        for (int y = top; y < bottom; y++) {
            for (int x = left; x < right; x++) {
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

    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl" << std::endl;
        return 1;
    }

    // Create an OpenGL window
    SDL_Window* window = SDL_CreateWindow("slippymap3d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Load the file matching the given coordinates
    Tile* center_tile;

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


        center_tile = get_tile(16, 50.356718, 7.599485);
        render(center_tile);
        SDL_GL_SwapWindow(window);
    }

    for (std::pair<std::string, Tile*> tile : tiles) {
        delete tile.second;
    }
    tiles.clear();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    curl_easy_cleanup(curl);
    SDL_Quit();

    return 0;
}

