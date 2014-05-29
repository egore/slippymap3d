#ifndef _SM3D_LOADER_H_
#define _SM3D_LOADER_H_

#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio/io_service.hpp>

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
    void open_image(Tile& tile);
private:
    static Loader* _instance;
    Loader() {
        work = new boost::asio::io_service::work(ioService);
        for (int i = 0; i < 5; i++) {
            pool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
        }
    }
    Loader(const Loader&) {}
    ~Loader() {
        ioService.stop();
        pool.join_all();
        delete work;
    }

    boost::thread_group pool;
    boost::asio::io_service ioService;
    boost::asio::io_service::work * work;

    void download_image(Tile* tile);

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
