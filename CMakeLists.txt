project(slippymad3d)
cmake_minimum_required(VERSION 2.8)

# By default look for modules (e.g. Findsdl2.cmake) in the cmake-folder
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# We need C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -std=gnu++0x")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# SDL
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# SDL_image
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# CURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# boost
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
include_directories(${Boost_INCLUDE_DIRS})

# All source files from the current directory will be used
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${OPENGL_gl_LIBRARY} ${CURL_LIBRARY} ${Boost_LIBRARIES})
