## Cài Cmake
```
sudo apt install build-essential
sudo apt update
sudo apt install cmake
```

## Cài cJSON
```
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON

mkdir build
cd build
cmake ..
make
sudo make install
```

## build project
```
từ thư mục gốc của project
mkdir build
cd build
cmake ../src
make
```
# chay project
```
Từ thư mục gốc của project
cd build

chạy client
./json_test

chạy server
./server
```
