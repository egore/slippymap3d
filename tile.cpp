#include <sstream>
#include <cmath>

#include "tile.h"
#include "loader.h"

Tile* Tile::get(int x_diff, int y_diff) {
    return TileFactory::instance()->get_tile(zoom, x+x_diff, y+y_diff);
}

Tile* Tile::get_east() {
    return get(-1, 0);
}

Tile* Tile::get_north() {
    return get(0, -1);
}

Tile* Tile::get_south() {
    return get(0, 1);
}

Tile* Tile::get_west() {
    return get(1, 0);
}

std::string Tile::get_filename() {
    std::stringstream filename;
    filename << this->zoom << "/" << this->x << '/' << this->y << ".png";
    return filename.str();
}

TileFactory* TileFactory::_instance = nullptr;

TileFactory::~TileFactory() {
    for (std::pair<std::string, Tile*> tile : tiles) {
        delete tile.second;
    }
    tiles.clear();
}

int long2tilex(double lon, int z) {
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int lat2tiley(double lat, int z) {
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

double tilex2long(int x, int z) {
    return x / pow(2.0, z) * 360.0 - 180;
}

double tiley2lat(int y, int z) {
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

Tile* TileFactory::get_tile(int zoom, double latitude, double longitude) {
    int x = long2tilex(longitude, zoom);
    int y = lat2tiley(latitude, zoom);
    return get_tile(zoom, x, y);
}

Tile* TileFactory::get_tile(int zoom, int x, int y) {
    std::string id = tile_id(zoom, x, y);
    std::map<std::string, Tile*>::iterator tile_iter = tiles.find(id);
    if (tile_iter != tiles.end()) {
        return tile_iter->second;
    }
    Tile* tile = new Tile();
    tile->zoom = zoom;
    tile->x = x;
    tile->y = y;
    tile->texid = dummy;
    Loader::instance()->load_image(*tile);
    tiles[id] = tile;
    return tile;
}

std::string TileFactory::tile_id(int zoom, int x, int y) {
    std::stringstream ss;
    ss << zoom << "/" << x << "/" << y;
    return ss.str();
}
