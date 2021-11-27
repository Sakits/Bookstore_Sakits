cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

add_executable(code src/main.cpp
        src/B+Tree.hpp src/book.hpp src/Bookstore.hpp src/users_manager.hpp)