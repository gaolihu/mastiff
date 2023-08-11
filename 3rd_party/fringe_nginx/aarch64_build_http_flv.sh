TOP_DIR=`pwd`
CC=`which aarch64-linux-gnu-gcc`
CC_PATH=`dirname $CC`

CC=`ls $CC_PATH/aarch64-linux-gnu-gcc`
CXX=`ls $CC_PATH/aarch64-linux-gnu-g++`
AR=`ls $CC_PATH/aarch64-linux-gnu-ar`

ZLIB_PACK=zlib-1.2.12.tar.gz
ZLIB_PATH=$TOP_DIR/zlib-1.2.12
ZLIB_INSTALL=$ZLIB_PATH/install

PCRE_PACK=pcre-8.45.tar.gz
PCRE_PATH=$TOP_DIR/pcre-8.45
PCRE_INSTALL=$PCRE_PATH/install

OPENSSL_PACK=openssl-1.1.1k.tar.gz
OPENSSL_PATH=$TOP_DIR/openssl-1.1.1k
OPENSSL_INSTALL=$OPENSSL_PATH/install

NGX_HTTP_FLV_PACK=nginx-http-flv-module-git.tgz
NGX_HTTP_FLV_PATH=$TOP_DIR/nginx-http-flv-module-git
NGX_HTTP_FLV_INSTALL=$NGX_HTTP_FLV_PATH/install

NGINX_PACK=nginx-1.20.2.tar.gz
NGINX_PATH=$TOP_DIR/nginx-1.20.2
NGINX_INSTALL=$NGINX_PATH/install

IFLAGS="-O2 -I$ZLIB_INSTALL/include -I$PCRE_INSTALL/include -I$OPENSSL_INSTALL/include -I$NGX_HTTP_FLV_INSTALL/include -I$NGINX_INSTALL/include "
XFLAGS="-O2 -I$ZLIB_INSTALL/include -I$PCRE_INSTALL/include -I$OPENSSL_INSTALL/include -I$NGX_HTTP_FLV_INSTALL/include -I$NGINX_INSTALL/include "
LFLAGS="-L$ZLIB_INSTALL/lib -L$PCRE_INSTALL/lib -L$OPENSSL_INSTALL/lib -L$NGX_HTTP_FLV_INSTALL/lib -L$NGINX_INSTALL/lib "

tar xfz $NGX_HTTP_FLV_PACK -C $TOP_DIR
