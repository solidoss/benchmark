# benchmark

Benchmark SolidFrame against different libraries 

## On Linux
```bash
cmake -DCMAKE_BUILD_TYPE=maintain -DEXTERNAL_PATH=~/work/external -DSolidFrame_DIR=~/work/solidframe/build/maintain ../../
```

```bash
cmake -DCMAKE_BUILD_TYPE=release -DEXTERNAL_PATH=~/work/external -DSolidFrame_DIR=~/work/solidframe/build/release ../../
```

### Results
On a 12 gen i7, WSL Fedora 36:

Test project /home/vipal/work/benchmark/build/release
      Start  1: SolidEcho100_1000
 1/37 Test  #1: SolidEcho100_1000 ................   Passed    0.30 sec
      Start  2: SolidEcho200_1000
 2/37 Test  #2: SolidEcho200_1000 ................   Passed    0.61 sec
      Start  3: SolidEcho400_1000
 3/37 Test  #3: SolidEcho400_1000 ................   Passed    1.11 sec
      Start  4: SolidRelayEcho100_1000
 4/37 Test  #4: SolidRelayEcho100_1000 ...........   Passed    1.05 sec
      Start  5: SolidRelayEcho200_1000
 5/37 Test  #5: SolidRelayEcho200_1000 ...........   Passed    2.02 sec
      Start  6: SolidRelayEcho400_1000
 6/37 Test  #6: SolidRelayEcho400_1000 ...........   Passed    3.57 sec
      Start  7: BoostEcho100_1000
 7/37 Test  #7: BoostEcho100_1000 ................   Passed    5.48 sec
      Start  8: BoostEcho200_1000
 8/37 Test  #8: BoostEcho200_1000 ................   Passed    5.90 sec
      Start  9: BoostEcho400_1000
 9/37 Test  #9: BoostEcho400_1000 ................   Passed    6.45 sec
      Start 10: BoostRelayEcho100_1000
10/37 Test #10: BoostRelayEcho100_1000 ...........   Passed    6.15 sec
      Start 11: BoostRelayEcho200_1000
11/37 Test #11: BoostRelayEcho200_1000 ...........   Passed    6.61 sec
      Start 12: BoostRelayEcho400_1000
12/37 Test #12: BoostRelayEcho400_1000 ...........   Passed    7.13 sec
      Start 13: Boost1k
13/37 Test #13: Boost1k ..........................   Passed    0.01 sec
      Start 14: Cereal1k
14/37 Test #14: Cereal1k .........................   Passed    0.01 sec
      Start 15: CerealPortable1k
15/37 Test #15: CerealPortable1k .................   Passed    0.01 sec
      Start 16: SolidV3_1k
16/37 Test #16: SolidV3_1k .......................   Passed    0.01 sec
      Start 17: Boost10k
17/37 Test #17: Boost10k .........................   Passed    0.06 sec
      Start 18: Cereal10k
18/37 Test #18: Cereal10k ........................   Passed    0.05 sec
      Start 19: CerealPortable10k
19/37 Test #19: CerealPortable10k ................   Passed    0.05 sec
      Start 20: SolidV3_10k
20/37 Test #20: SolidV3_10k ......................   Passed    0.07 sec
      Start 21: Boost100k
21/37 Test #21: Boost100k ........................   Passed    0.59 sec
      Start 22: Cereal100k
22/37 Test #22: Cereal100k .......................   Passed    0.49 sec
      Start 23: CerealPortable100k
23/37 Test #23: CerealPortable100k ...............   Passed    0.48 sec
      Start 24: SolidV3_100k
24/37 Test #24: SolidV3_100k .....................   Passed    0.68 sec
      Start 25: SolidPlainNoComp_100_1
25/37 Test #25: SolidPlainNoComp_100_1 ...........   Passed    0.53 sec
      Start 26: SolidPlainNoComp_100_2
26/37 Test #26: SolidPlainNoComp_100_2 ...........   Passed    1.07 sec
      Start 27: SolidPlainNoComp_100_3
27/37 Test #27: SolidPlainNoComp_100_3 ...........   Passed    0.87 sec
      Start 28: SolidPlainNoComp_100_4
28/37 Test #28: SolidPlainNoComp_100_4 ...........   Passed    2.99 sec
      Start 29: SolidPlainNoComp_100_5
29/37 Test #29: SolidPlainNoComp_100_5 ...........   Passed    4.84 sec
      Start 30: SolidPlainNoComp_100_6
30/37 Test #30: SolidPlainNoComp_100_6 ...........   Passed    5.43 sec
      Start 31: SolidPlainComp_100_6
31/37 Test #31: SolidPlainComp_100_6 .............   Passed    5.45 sec
      Start 32: GrpcPlainNoComp_100_1
32/37 Test #32: GrpcPlainNoComp_100_1 ............   Passed    5.86 sec
      Start 33: GrpcPlainNoComp_100_2
33/37 Test #33: GrpcPlainNoComp_100_2 ............   Passed    6.39 sec
      Start 34: GrpcPlainNoComp_100_3
34/37 Test #34: GrpcPlainNoComp_100_3 ............   Passed    6.26 sec
      Start 35: GrpcPlainNoComp_100_4
35/37 Test #35: GrpcPlainNoComp_100_4 ............   Passed    7.93 sec
      Start 36: GrpcPlainNoComp_100_5
36/37 Test #36: GrpcPlainNoComp_100_5 ............   Passed    9.78 sec
      Start 37: GrpcPlainNoComp_100_6
37/37 Test #37: GrpcPlainNoComp_100_6 ............   Passed   11.16 sec

100% tests passed, 0 tests failed out of 37

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
# build zlib
$ cd ~/home/grpc/third_party/zlib
$ mkdir .build
$ cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/home/external64
$ cmake --build . --config release --target install
# build protobuf
$ cd ~/home/grpc/third_party/protobuf/cmake
$ mkdir .build
$  cmake .. -G"Visual Studio 15 2017 Win64" -Dprotobuf_MSVC_STATIC_RUNTIME=false -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=~/home/external64/
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

After succesfull build, may need to copy the needed dlls to grpc test folder
```bash
$ cd mpipc/grpc/Release
$ cp "/c/Program Files/Common Files/microsoft shared/ClickToRun/"api-ms-win-crt-*-l1-1-0.dll .
 ```