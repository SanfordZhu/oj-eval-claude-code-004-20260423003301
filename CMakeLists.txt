cmake_minimum_required(VERSION 3.10)
project(BookstoreManagement)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

set(SOURCES
    main.cpp
    user.cpp
    book.cpp
    log.cpp
    system.cpp
    utils.cpp
)

set(HEADERS
    user.h
    book.h
    log.h
    system.h
    utils.h
)

add_executable(code ${SOURCES} ${HEADERS})