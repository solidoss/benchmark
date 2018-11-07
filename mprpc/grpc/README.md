
Compile gRPC with:

```bash
$ git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
$ cd grpc
$ git submodule update --init
$ mkdir .build
$ cd .build
$ cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DgRPC_SSL_PROVIDER=package -DCMAKE_INSTALL_PREFIX:STRING=/home/vip/work/external -DCMAKE_BUILD_TYPE=Release ../
$ make -j3 install
```
