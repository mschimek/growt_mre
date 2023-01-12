cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
OMP_NUM_THREADS=32 ./build/main
