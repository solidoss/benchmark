# benchmark

Benchmark SolidFrame against different libraries 
```bash
cmake -DCMAKE_BUILD_TYPE=maintain -DEXTERNAL_PATH=~/work/external -DSolidFrame_DIR=~/work/solidframe/build/maintain ../../
```

```bash
cmake -DCMAKE_BUILD_TYPE=release -DEXTERNAL_PATH=~/work/external -DSolidFrame_DIR=~/work/solidframe/build/release ../../
```

## On Windows

First need to build grpc
Be carefull not to have any openssl headers and libraries in path.
E.g. C:\Strawberry\c\include\openssl and C:\Strawberry\c\lib


```bash
# clone the grpc repos
$ git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
$ git submodule update --init
# build grpc using cmake with every dependency as module
$ mkdir .build
$ cd .build
$ cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/home/external64
# next will build and install - only protobuf will be installed!!
$ cmake --build . --config release --target install
# build will fail on boringssl
$ vim ../third_party/boringssl/CMakeLists.txt
# ... and remove all occurences of "-WX"
$ cmake --build . --config release --target install
$ cd ~/home/external64/
$ cp -r ../grpc/third_party/boringssl/include/openssl/ include/
$ cp ../grpc/.build/third_party/boringssl/ssl/Release/ssl.lib lib/libssl.lib
$ cp ../grpc/.build/third_party/boringssl/crypto/Release/crypto.lib lib/libcrypto.lib
# build c-ares
$ curl -L -O "https://c-ares.haxx.se/download/c-ares-1.14.0.tar.gz"
$ tar -xzf c-ares-1.14.0.tar.gz
$ mkdir -p c-ares-1.14.0/.build
$ cd c-ares-1.14.0/.build
$ cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/home/external64
$ cmake --build . --config release --target install
$ cd ~/home/grpc/.build
$ rm -rf *
$ cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/home/external64
$ cmake-gui ..
# using cmake-gui configure modules as package: zlib, c-ares, protobuf, borinssl
# press configure then set c-ares path to: C:\Users\vipal\home\external64\lib\cmake\c-ares
# press configure again then generate
# close cmake-gui and do:
$ cmake --build . --config release --target install
```

```bash
 cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=release -DEXTERNAL_PATH=~/home/external64 -DSolidFrame_DIR=~/home/solidframe/build/release ../../
 ```