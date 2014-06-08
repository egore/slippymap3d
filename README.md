slippymap3d
===========

Building
--------

slippymap3d uses CMake to compile the code. To build the program you need at
least

* C++11 capable compiler
* SDL 2.x with SDL_image
* CURL
* OpenGL
* boost filesystem, system and thread

To compile the program you can either use the following commands

```
cd slippymap3d
mkdir build
cd build
cmake ..
make
```

Alternatively the ninja build system is supported as well

```
cd slippymap3d
mkdir build
cd build
cmake -GNinja ..
ninja
```

Using official tiles
--------------------

Starting the program will try to download tiles from "http://localhost/osm_tiles/" to
*the current directory*. If you want to use the official tiles from OpenStreetMap (note
that you need to follow the tile usage policy) replace the URL in loader.cpp with e.g.
"http://a.tile.openstreetmap.org/".

Navigation
----------

* Left mouse button: rotate the map
* Right mouse button: tile the map
* Middle mouse button: move the map
