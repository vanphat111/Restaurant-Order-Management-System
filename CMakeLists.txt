cmake_minimum_required(VERSION 3.10)

project("Restaurant order manager client")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(nha-hang-client ${SOURCES})

target_link_libraries(nha-hang-client PRIVATE mysqlcppconn OpenSSL::Crypto)
