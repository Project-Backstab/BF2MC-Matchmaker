#/bin/env bash

rm -r ../BF2MC-Matchmaker-release/ --exclude='../BF2MC-Matchmaker-release/data/log'
rm -r build/
rm data/log/*

mkdir build
cd build
cmake ..
make -j 4

cmake --build . --target doc

cmake --install . --prefix ../../BF2MC-Matchmaker-release

cd ../../BF2MC-Matchmaker-release

rm -r include/
rm -r lib/

