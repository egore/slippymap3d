#ifndef _SM3D_TILE_H_
#define _SM3D_TILE_H_

#include <string>
#include <map>

#include <GL/gl.h>

class Tile {
public:
    int zoom;
    int x;
    int y;
    GLuint texid;
    Tile* get(int x_diff, int y_diff);
    Tile* get_east();
    Tile* get_north();
    Tile* get_south();
    Tile* get_west();
    std::string get_filename();
};

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
private:
    static TileFactory* _instance;
    TileFactory() {}
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
