cmake_minimum_required(VERSION 3.8)
project(mp3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        dataserver.cc
        reqchannel.cc
        reqchannel.H
        semaphore.H semaphore.cc client.cc BoundedBuffer.h
        mutex.cc mutex.H BoundedBufferUnitTests.cc Response.cpp Response.h)

add_executable(mp3 ${SOURCE_FILES})