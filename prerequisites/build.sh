#!/usr/bin/env bash

printUsage()
{
    echo
    echo "Usage:"
    echo
    echo "./prepare_extern.sh [--all] [--boost] [--openssl] [--boringssl] [--force-download] [--debug] [--64bit] [-h|--help]"
    echo
    echo "Examples:"
    echo
    echo "Build all external dependencies:"
    echo "$ ./prepare_extern.sh"
    echo
    echo "Build all supported dependencies:"
    echo "$ ./prepare_extern.sh --all"
    echo
    echo "Build only boost:"
    echo "$ ./prepare_extern.sh --boost"
    echo
    echo "Build only boost with debug simbols and force download the archive:"
    echo "$ ./prepare_extern.sh --boost --force-download -d"
    echo
    echo "Build boost and openssl:"
    echo "$ ./prepare_extern.sh --boost --openssl"
    echo
}

BOOST_ADDR="https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.bz2"
OPENSSL_ADDR="https://github.com/openssl/openssl/releases/download/openssl-3.5.0/openssl-3.5.0.tar.gz"
CARES_ADDR="https://github.com/c-ares/c-ares/releases/download/v1.34.4/c-ares-1.34.4.tar.gz"
PROTOBUF_ADDR="https://github.com/protocolbuffers/protobuf/releases/download/v30.2/protobuf-30.2.tar.gz"
CAPNPROTO_ADDR="https://capnproto.org/capnproto-c++-1.1.0.tar.gz" #"https://github.com/capnproto/capnproto/archive/refs/tags/v1.0.1.tar.gz"
ABSEIL_ADDR="https://github.com/abseil/abseil-cpp/releases/download/20250127.1/abseil-cpp-20250127.1.tar.gz"
GRPC_ADDR="https://github.com/grpc/grpc/archive/refs/tags/v1.72.0.tar.gz"
BORINGSSL_ADDR="https://github.com/google/boringssl/archive/refs/tags/0.20250114.0.tar.gz"
SYSTEM=
BIT64=

downloadArchive()
{
    local url="$1"
    local arc="$(basename "${url}")"
    #wget --no-check-certificate -O $arc $url
    #wget -O $arc $url
    
    if [ -z "$2" ]
        then
        echo "Downloading: [$arc] from [$url]"
        curl -L -O $url
        else
        echo "Downloading: [$2] from [$url]"
        curl -L -o $2 $url
    fi
    
}

extractTarBz2()
{
    tar -xjf "$1"
}

extractTarGz()
{
    tar -xzf "$1"
}


buildBoost()
{
    echo
    echo "Building boost..."
    echo
    BOOST_DIR=`ls . | grep "boost" | grep -v "tar"`
    echo "Cleanup previous builds..."
    rm -rf $BOOST_DIR
    rm -rf include/boost
    rm -rf lib/libboost*

    echo
    echo "Prepare the boost archive..."
    echo
    BOOST_ARCH=`find . -name "boost_*.tar.bz2" | grep -v "old/"`

    if [ -z "$BOOST_ARCH" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $BOOST_ARCH old/
        echo "No boost archive found - try download: $BOOST_ADDR"
        downloadArchive $BOOST_ADDR
        BOOST_ARCH=`find . -name "boost_*.tar.bz2" | grep -v "old/"`
    fi

    echo
    echo "Extracting boost [$BOOST_ARCH]..."
    echo

    extractTarBz2 $BOOST_ARCH
    BOOST_DIR=`ls . | grep "boost" | grep -v "tar"`
    echo
    echo "Making boost [$BOOST_DIR]..."
    echo

    cd "$BOOST_DIR"
    
    
    if [ $DEBUG ] ; then
        VARIANT_BUILD="debug"
    else
        VARIANT_BUILD="release"
    fi
    
    
    if		[ "$SYSTEM" = "FreeBSD" ] ; then
        sh bootstrap.sh --with-toolset=clang
        ./b2 toolset=clang --layout=system  --prefix="$EXT_DIR" --exec-prefix="$EXT_DIR" link=static threading=multi $VARIANT_BUILD install
    elif	[ "$SYSTEM" = "Darwin" ] ; then
        sh bootstrap.sh
        ./b2 --layout=system  --prefix="$EXT_DIR" --exec-prefix="$EXT_DIR" link=static threading=multi $VARIANT_BUILD install
        echo
    elif    [[ "$SYSTEM" =~ "MINGW" ]]; then
        if [ $BIT64 ]; then
            BOOST_ADDRESS_MODEL="64"
        else
            BOOST_ADDRESS_MODEL="32"
        fi
        
        ./bootstrap.bat
        ./b2 --abbreviate-paths --hash address-model="$BOOST_ADDRESS_MODEL" variant="$VARIANT_BUILD" link=static threading=multi --prefix="$EXT_DIR" install
    else
        sh bootstrap.sh
        ./b2 --layout=system  --prefix="$EXT_DIR" --exec-prefix="$EXT_DIR" link=static threading=multi $VARIANT_BUILD install
        echo
    fi
    
    cd ..
    if [ ! $NOCLEANUP ]; then
        rm -rf "$BOOST_DIR"
    fi
    
    echo
    echo "Done BOOST!"
    echo
}

buildAbseil()
{
    WHAT="abseil"
    ADDR_NAME=$ABSEIL_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    rm -rf $OLD_DIR

    ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME
        ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    mkdir build && cd build
    cmake -DABSL_BUILD_TESTING=OFF -DABSL_USE_GOOGLETEST_HEAD=OFF -DCMAKE_INSTALL_PREFIX="$EXT_DIR" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_LIBDIR="lib" ..
    cmake --build . --target all -j8
    cmake --install .
}

buildOpenssl()
{
    WHAT="openssl"
    ADDR_NAME=$OPENSSL_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    rm -rf $OLD_DIR
    rm -rf include/openssl
    rm -rf lib/libssl*
    rm -rf lib64/libssl*
    rm -rf lib/libcrypto*
    rm -rf lib64/libcrypto*
    rm -rf ssl_

    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME
        ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    if		[ "$SYSTEM" = "FreeBSD" ] ; then
        if [ $DEBUG ] ; then
            CC=cc ./config --prefix="$EXT_DIR" --openssldir="ssl_"
        else
            CC=cc ./config --prefix="$EXT_DIR" --openssldir="ssl_"
        fi
    elif	[ "$SYSTEM" = "Darwin" ] ; then
        if [ $DEBUG ] ; then
            ./Configure --prefix="$EXT_DIR" --openssldir="ssl_" darwin64-x86_64-cc
        else
            ./Configure --prefix="$EXT_DIR" --openssldir="ssl_" darwin64-x86_64-cc
        fi
    elif    [[ "$SYSTEM" =~ "MINGW" ]]; then
        if [ $BIT64 = true ]; then
            OPENSSL_TARGET="VC-WIN64A"
        else
            OPENSSL_TARGET="VC-WIN32"
        fi

        wperl Configure $OPENSSL_TARGET --prefix="$EXT_DIR" --openssldir="ssl_"  no-shared
    else
        if [ $DEBUG ] ; then
            ./config --prefix="$EXT_DIR" --openssldir="ssl_"
        else
            ./config --prefix="$EXT_DIR" --openssldir="ssl_"
        fi
    fi
    if    [[ "$SYSTEM" =~ "MINGW" ]]; then
        nmake && nmake install_sw
    else
        make && make install_sw
    fi
    
    cd ..
    
    echo "Copy test certificates to ssl_ dir..."
    
    cp $DIR_NAME/demos/tunala/*.pem ssl_/certs/.
    
    echo
    echo "Done $WHAT!"
    echo
}

function buildCAres
{
    WHAT="c-ares"
    ADDR_NAME=$CARES_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    
    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME
        ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    if		[ "$SYSTEM" = "FreeBSD" ] ; then
        echo "TODO..."
    elif	[ "$SYSTEM" = "Darwin" ] ; then
        echo "TODO..."
    elif    [[ "$SYSTEM" =~ "MINGW" ]]; then
        if [ "$BIT64" = true ]; then
            echo "TODO..."
        else
            echo "TODO..."
        fi
        mkdir .build
        cd .build
        cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$EXT_DIR"
    else
        mkdir .build
        cd .build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$EXT_DIR"
    fi
    if    [[ "$SYSTEM" =~ "MINGW" ]]; then
        cmake --build . --config release -j8
        cmake --build . --config release --target install
    else
        cmake --build . --config release -j8
        cmake --build . --config release --target install
    fi
    
    cd ..
    
    echo
    echo "Done $WHAT!"
    echo
}

function buildCapNProto
{
    WHAT="capnproto"
    ADDR_NAME=$CAPNPROTO_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds... ${OLD_DIR}"
    echo

    rm -rf ${OLD_DIR}
    rm -rf lib/libcapnp*

    
    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME
        ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    if		[ "$SYSTEM" = "FreeBSD" ] ; then
        echo "TODO..."
    elif	[ "$SYSTEM" = "Darwin" ] ; then
        echo "TODO..."
    elif    [[ "$SYSTEM" =~ "MINGW" ]]; then
        if [ "$BIT64" = true ]; then
            echo "TODO..."
        else
            echo "TODO..."
        fi
        #mkdir .build
        #cd .build
        #cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$EXT_DIR"
    else
        #./configure --prefix="$EXT_DIR"
        #make -j8 install
        mkdir build && cd build
        cmake -DCMAKE_INSTALL_PREFIX="$EXT_DIR" -DCMAKE_PREFIX_PATH="$EXT_DIR" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_LIBDIR="lib" ..
        cmake --build . --target all -j8
        cmake --install .
    fi
    
    cd ..
    
    echo
    echo "Done $WHAT!"
    echo
}

function buildBoringSSL
{
    WHAT="boringssl"
    ADDR_NAME=$BORINGSSL_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    rm -rf $OLD_DIR

    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME $WHAT.tar.gz
        ARCH_NAME=`find . -name "$WHAT.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX="$EXT_DIR" -DCMAKE_PREFIX_PATH="$EXT_DIR" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_LIBDIR="lib" ..
    cmake --build . --target all -j8
    cmake --install .
    
    cd ..
    
    cp -r include/openssl "$EXT_DIR/include"
    
    cd "$EXT_DIR"
}

buildProtobuf()
{
    WHAT="protobuf"
    ADDR_NAME=$PROTOBUF_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    rm -rf $OLD_DIR
    rm -rf include/google/protobuf
    rm -rf lib/libprotobuf*

    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME
        ARCH_NAME=`find . -name "$WHAT-*.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    mkdir build && cd build
    cmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX="$EXT_DIR" -DCMAKE_PREFIX_PATH="$EXT_DIR" -Dprotobuf_ABSL_PROVIDER="package" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_LIBDIR="lib" ..
    cmake --build . --target all -j8
    cmake --install .
    
    cd ..
    
    echo
    echo "Done $WHAT!"
    echo
}


buildGrpc()
{
    WHAT="grpc"
    ADDR_NAME=$GRPC_ADDR
    echo
    echo "Building $WHAT..."
    echo

    OLD_DIR=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Cleanup previous builds..."
    echo

    rm -rf $OLD_DIR

    echo
    echo "Prepare the $WHAT archive..."
    echo

    ARCH_NAME=`find . -name "$WHAT.tar.gz" | grep -v "old/"`
    if [ -z "$ARCH_NAME" -o -n "$DOWNLOAD" ] ; then
        mkdir old
        mv $ARCH_NAME old/
        echo "No $WHAT archive found or forced - try download: $ADDR_NAME"
        downloadArchive $ADDR_NAME $WHAT.tar.gz
        ARCH_NAME=`find . -name "$WHAT.tar.gz" | grep -v "old/"`
    fi
    
    echo "Extracting $WHAT [$ARCH_NAME]..."
    extractTarGz $ARCH_NAME

    DIR_NAME=`ls . | grep "$WHAT" | grep -v "tar"`
    echo
    echo "Making $WHAT [$DIR_NAME]..."
    echo

    cd $DIR_NAME
    
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX="$EXT_DIR" -DCMAKE_PREFIX_PATH="$EXT_DIR" -DgRPC_ABSL_PROVIDER="package" -DgRPC_CARES_PROVIDER="package" -DgRPC_PROTOBUF_PROVIDER="package" -DgRPC_SSL_PROVIDER="package" -DgRPC_RE2_PROVIDER="package" -DgRPC_ZLIB_PROVIDER="package" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_LIBDIR="lib" ..
    cmake --build . --target all -j8
    cmake --install .
    
    cd ..
    
    echo
    echo "Done $WHAT!"
    echo
}

EXT_DIR="`pwd`"

BUILD_BOOST_MIN=
BUILD_BOOST_FULL=
BUILD_OPENSSL=
BUILD_BORINGSSL=
BUILD_C_ARES=
BUILD_PROTOBUF=
BUILD_CAPNPROTO=
BUILD_ABSEIL=
BUILD_GRPC=

BUILD_SOMETHING=

ARCHIVE=
DOWNLOAD=
DEBUG=
HELP=


while [ "$#" -gt 0 ]; do
    CURRENT_OPT="$1"
    UNKNOWN_ARG=no
    HELP=
    case "$1" in
    --all)
        BUILD_BOOST_FULL="yes"
        BUILD_OPENSSL="yes"
        BUILD_SOMETHING="yes"
        ;;
    --boost)
        BUILD_BOOST_FULL="yes"
        BUILD_SOMETHING="yes"
        ;;
    --openssl)
        BUILD_OPENSSL="yes"
        BUILD_SOMETHING="yes"
        ;;
    --boringssl)
        BUILD_BORINGSSL="yes"
        BUILD_SOMETHING="yes"
        ;;
    --cares)
        BUILD_C_ARES="yes"
        BUILD_SOMETHING="yes"
        ;;
    --capnp)
        BUILD_CAPNPROTO="yes"
        BUILD_SOMETHING="yes"
        ;;
    --abseil)
        BUILD_ABSEIL="yes"
        BUILD_SOMETHING="yes"
        ;;
    --protobuf)
        BUILD_PROTOBUF="yes"
        BUILD_SOMETHING="yes"
        ;;
    --grpc)
        BUILD_GRPC="yes"
        BUILD_SOMETHING="yes"
        ;;
    --debug)
        DEBUG="yes"
        ;;
    --force-download)
        DOWNLOAD="yes"
        ;;
    --system)
        shift
        SYSTEM="$1"
        ;;
    --64bit)
        BIT64=true
        ;;
    -h|--help)
        HELP="yes"
        BUILD_SOMETHING="yes"
        ;;
    *)
        HELP="yes"
        ;;
    esac
    shift
done

if [ "$HELP" = "yes" ]; then
    printUsage
    exit
fi

if [[ -z "${SYSTEM}" ]]; then
    SYSTEM=$(uname)
fi


echo "Extern folder: $EXT_DIR"
echo "System: $SYSTEM"

if [[ -z "${BUILD_SOMETHING}" ]]; then
    BUILD_BOOST_FULL="yes"
    BUILD_OPENSSL="yes"
fi

if [ $BUILD_OPENSSL ]; then
    buildOpenssl
fi

if [ $BUILD_BORINGSSL ]; then
    buildBoringSSL
fi

if [ $BUILD_C_ARES ]; then
    buildCAres
fi

if [ $BUILD_CAPNPROTO ]; then
    buildCapNProto
fi

if [ $BUILD_PROTOBUF ]; then
    buildProtobuf
fi

if [ $BUILD_ABSEIL ]; then
    buildAbseil
fi

if [ $BUILD_GRPC ]; then
    buildGrpc
fi

if [ $BUILD_BOOST_FULL ]; then
    buildBoost
else
    if [ $BUILD_BOOST_MIN ]; then
        buildBoost
    fi
fi


#if [ -d lib64 ]; then
#    cd lib
#
#    for filename in ../lib64/*
#    do
#    echo "SimLink to $filename"
#    ln -s $filename .
#    done;
#fi

if [ $ARCHIVE ]; then
    cd $EXT_DIR
    cd ../
    CWD="`basename $EXT_DIR`"
    echo $CWD

    echo "tar -cjf $CWD.tar.bz2 $CWD/include $CWD/lib $CWD/lib64 $CWD/bin $CWD/sbin $CWD/share"
    tar -cjf $CWD.tar.bz2 $CWD/include $CWD/lib $CWD/bin $CWD/sbin $CWD/share
fi

echo
echo "DONE!"
