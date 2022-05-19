mkdir -p build
cd build
cmake .. -DCMAKE_PREFIX_PATH=/opt/local
make
cd ..

mkdir -p results
mkdir -p plots
mkdir -p images
mkdir -p archive
mkdir -p gen
