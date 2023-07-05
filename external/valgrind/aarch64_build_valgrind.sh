TOP_DIR=`pwd`
CC=`which aarch64-linux-gnu-gcc`
CC_PATH=`dirname $CC`

CC=`ls $CC_PATH/aarch64-linux-gnu-gcc`
CXX=`ls $CC_PATH/aarch64-linux-gnu-gcc`
AR=`ls $CC_PATH/aarch64-linux-gnu-ar`

CFLAGS="-O2"
CPPFLAGS="-O2"

VALGRIND_BUILD=$TOP_DIR/build_aarch64
VALGRIND_INSTALL=$VALGRIND_BUILD/install

if [ ! -d $VALGRIND_INSTALL ]; then
    mkdir $VALGRIND_INSTALL
fi

if [ ! -d $VALGRIND_BUILD ]; then
    mkdir $VALGRIND_BUILD
fi

###### build valgrind ######
./autogen.sh

cd $VALGRIND_BUILD

../configure \
    --prefix=$VALGRIND_INSTALL \
    --target=aarch64 \
    --build=i686-linux \
    --host=aarch64-linux-gnu \
    --program-prefix="" \
    --program-suffix="_mastiff" \
    CC=$CC \
    CXX=$CXX \
    AR=$AR \
    CFLAGS=$CFLAGS \
    CPPFLAGS=$CPPFLAGS

make && make install

cd $TOP_DIR
pwd
###### build nginx ######
