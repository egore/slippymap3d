#ifndef _SM3D_LOADER_H_
#define _SM3D_LOADER_H_

#include <iostream>

#include <curl/curl.h>

#include "tile.h"

class Loader {
public:
    static Loader* instance() {
        static CGuard g;
        if (!_instance) {
            _instance = new Loader();
        }
        return _instance;
    }

    void load_image(Tile& tile);
private:
    static Loader* _instance;
    Loader() {
        curl = curl_easy_init();
        if (curl == nullptr) {
            std::cerr << "Failed to initialize curl" << std::endl;
        }
    }
    Loader(const Loader&) {}
    ~Loader() {
        if (curl != nullptr) {
            curl_easy_cleanup(curl);
        }
    }

    bool download_image(Tile& tile);

    CURL* curl = nullptr;

    class CGuard {
    public:
        ~CGuard() {
            if (Loader::_instance != nullptr) {
                delete Loader::_instance;
                Loader::_instance = nullptr;
            }
        }
    };
    friend class CGuard;
};

#endif
