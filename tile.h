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

#ifndef _SM3D_TILE_H_
#define _SM3D_TILE_H_

#include <string>
#include <map>

#include <GL/gl.h>

/**
 * @brief storage class for a tile
 */
class Tile {
public:
    int zoom;
    int x;
    int y;
    GLuint texid;
    Tile(int zoom, int x, int y, GLuint texid);
    Tile* get(int x_diff, int y_diff);
    Tile* get_east();
    Tile* get_north();
    Tile* get_south();
    Tile* get_west();
    std::string get_filename();
};

extern int long2tilex(double lon, int z);

extern int lat2tiley(double lat, int z);

extern double tilex2long(int x, int z);

extern double tiley2lat(int y, int z);

/**
 * Determines the longitude size of a tile at given zoom
 */
extern double lonsize(int z);

extern double latsize(double lat, int z);

class TileFactory {
private:
    std::map<std::string, Tile*> tiles;
public:
    static TileFactory* instance() {
        static CGuard g;
        if (!_instance) {
            _instance = new TileFactory();
        }
        return _instance;
    }
    Tile* get_tile(int zoom, double latitude, double longitude);
    Tile *get_tile(int zoom, int x, int y);
    GLuint get_dummy() {
        return dummy;
    }
private:
    static TileFactory* _instance;
    GLuint dummy;
    TileFactory() {
        glGenTextures(1, &this->dummy);
        glBindTexture(GL_TEXTURE_2D, this->dummy);

        unsigned char empty[3] = {0, 0, 0};
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, empty);
    }
    TileFactory(const TileFactory&) {}
    ~TileFactory();
    std::string tile_id(int zoom, int x, int y);

    class CGuard {
    public:
        ~CGuard() {
            if (TileFactory::_instance != nullptr) {
                delete TileFactory::_instance;
                TileFactory::_instance = nullptr;
            }
        }
    };
    friend class CGuard;
};

#endif
