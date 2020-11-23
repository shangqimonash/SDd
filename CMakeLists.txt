cmake_minimum_required(VERSION 3.17)
project(SDd)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(OpenSSL REQUIRED)

# build Thrift dependencies
set(THRIFT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Thrift/Server.thrift)
set(THRIFT_LIB_PATH /usr/local/lib/)

link_directories(${THRIFT_LIB_PATH})

exec_program("thrift --gen cpp -o ${CMAKE_CURRENT_SOURCE_DIR} ${THRIFT_FILE}")

add_library(gen-cpp-thrift STATIC
        gen-cpp/SSEService.cpp
        gen-cpp/Server_constants.cpp
        gen-cpp/Server_types.cpp)

target_link_libraries(gen-cpp-thrift -lthrift)

# load SDd header
INCLUDE_DIRECTORIES(. Core Oblivious Util gen-cpp)

ADD_EXECUTABLE(AESTest Test/AESTest.cpp Util/CommonUtil.c)
ADD_EXECUTABLE(SDdTest Test/DeAmortizedTest.cpp Core/DeAmortizedBASServerHandler.cpp Core/DeAmortizedBASClientHandler.cpp
        Oblivious/OMAP.cpp Oblivious/AVLTree.cpp Oblivious/ORAM.cpp Oblivious/Bid.cpp
        Util/RAMStore.cpp Util/Types.cpp Util/Utilities.cpp Util/CommonUtil.c)
ADD_EXECUTABLE(SDdServer Core/SSEServer.cpp Core/DeAmortizedBASServerHandler.cpp Core/DeAmortizedBASClientHandler.cpp
        Oblivious/OMAP.cpp Oblivious/AVLTree.cpp Oblivious/ORAM.cpp Oblivious/Bid.cpp
        Util/RAMStore.cpp Util/Types.cpp Util/Utilities.cpp Util/CommonUtil.c
        gen-cpp)
ADD_EXECUTABLE(SDdClient main.cpp Core/SSEClient.cpp Util/Utilities.cpp Util/CommonUtil.c gen-cpp)

# link
TARGET_LINK_LIBRARIES(AESTest OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(SDdTest OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(SDdServer OpenSSL::Crypto gen-cpp-thrift)
TARGET_LINK_LIBRARIES(SDdClient OpenSSL::Crypto gen-cpp-thrift)