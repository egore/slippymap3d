#include <sstream>
#include <boost/filesystem.hpp>
#include <SDL2/SDL_image.h>

#include "loader.h"

Loader* Loader::_instance = nullptr;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool Loader::download_image(Tile& tile) {
    std::stringstream dirname;
    dirname << "../" << tile.zoom << "/" << tile.x;
    std::string dir = dirname.str();
    boost::filesystem::create_directories(dir);
    std::string filename = tile.get_filename();
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

void Loader::load_image(Tile& tile) {

    std::string filename = "../" + tile.get_filename();
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
