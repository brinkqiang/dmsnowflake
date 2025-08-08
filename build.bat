
rmdir /S /Q build
mkdir build 2> nul
cd build

cmake -A x64 -DCMAKE_BUILD_TYPE=relwithdebinfo ..
cmake --build . --config relwithdebinfo -- /m:%NUMBER_OF_PROCESSORS%
cd ..

rem pause