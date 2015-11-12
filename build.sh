rm -rf build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/usr/lib/cmake/Qt5 ../src
make
cp frs ../frs
