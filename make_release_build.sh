#/bin/env bash

rm ../BF2MC-Matchmaker-release/ -r
rm build/ -r

mkdir build
cd build
cmake ..
make -j 4

cmake --install . --prefix ../../BF2MC-Matchmaker-release

cd ../
rm ../BF2MC-Matchmaker-release/include/ -r
rm ../BF2MC-Matchmaker-release/lib/ -r
