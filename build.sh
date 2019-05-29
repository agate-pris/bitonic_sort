#!/bin/bash
mkdir -p build
cd build && \
cmake .. && \
cmake --build . && \
ctest 2>&1
