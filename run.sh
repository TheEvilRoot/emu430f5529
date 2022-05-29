#!/usr/bin/env bash -e
cd cmake-build-release
cmake ..
cd ..
cmake --build cmake-build-release --target untitled 
cd cmake-build-release
./untitled &
