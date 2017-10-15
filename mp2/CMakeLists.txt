cmake_minimum_required(VERSION 3.8)
project(mp2)

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS  "-pthread")

set(SOURCE_FILES
        dataserver.C
        mutex.C
        mutex.H
        mutex_guard.H
        reqchannel.C
        reqchannel.H
        semaphore.H
        simpleclient.C)

add_executable(mp2 ${SOURCE_FILES})