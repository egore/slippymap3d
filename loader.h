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

#ifndef _SM3D_LOADER_H_
#define _SM3D_LOADER_H_

#include <iostream>

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
    Loader();
    Loader(const Loader&) {}
    ~Loader();

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
