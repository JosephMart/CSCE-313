cmake_minimum_required(VERSION 3.8)
project(mp3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        dataserver.C
        reqchannel.C
        reqchannel.H
        semaphore.H
        simpleclient.C)

add_executable(mp3 ${SOURCE_FILES})