#!/bin/bash

# CMake -DCMAKE_BUILD_TYPE=Release와 동일한 최적화 옵션들
clang++ -std=c++20 benchmark.cpp \
        -I/opt/homebrew/include \
        -L/opt/homebrew/lib \
        -lbenchmark_main -lbenchmark -lpthread \
        -O3 \
        -DNDEBUG \
        -march=native \
        -mtune=native \
        -flto \
        -fomit-frame-pointer \
        -o benchmark.out

# 실행 (make -j 12 --silent -C build_release && build_release/unique_strings_cpp와 동일)
echo "빌드 완료"