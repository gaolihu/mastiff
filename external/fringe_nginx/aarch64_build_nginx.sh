TOP_DIR=`pwd`
WORK_DIR=/mastiff/nginx/
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


tar xfz $NGINX_PACK -C $TOP_DIR

###### build nginx ######
cd $NGINX_PATH

ls auto/cc/name

sed -i 's|exit 1|#|g' auto/cc/name
sed -i 's|ngx_feature_run=yes|ngx_feature_run=no|g' auto/cc/name
sed -i 's|ngx_feature_run=yes|ngx_feature_run=no|g' auto/os/linux
sed -i 's|ngx_feature_run=yes|ngx_feature_run=no|g' auto/unix
sed -i 's|ngx_size=`$NGX_AUTOTEST`|ngx_size=4|g' auto/types/sizeof
sed -i '187 a ngx_feature_run=no' auto/cc/conf

sed -i '108s|\\\$(DESTDIR)||g' auto/install
sed -i '115s|cp|mkdir -p $NGX_PREFIX$NGX_SBIN_PATH \&\& cp|g' auto/install
sed -i '157s|html|mastiff/nginx/html|g' auto/install
sed -i '158s|NGX_PREFIX|NGX_PREFIX/mastiff/nginx/|g' auto/install
sed -i '157,158s|\\\$(DESTDIR)||g' auto/install
sed -i 's|\\\$(DESTDIR)|\$NGX_PREFIX|g' auto/install
sed -i 's188|\\\$(DESTDIR)|\$NGX_PREFIX|g' auto/install

./configure \
    --with-zlib=$ZLIB_PATH \
    --with-pcre=$PCRE_PATH \
    --with-pcre-opt="-I$ZLIB_INSTALL/include -L$ZLIB_INSTALL/lib -lz" \
    --with-openssl=$OPENSSL_PATH \
    --with-openssl-opt="no-asm no-async shared zlib-dynamic no-ssl3-method no-ssl2 -DOPENSSL_USE_IPV6  -DOPENSSL_NO_DYNAMIC_ENGINE --cross-compile-prefix=aarch64-linux-gnu- -I$ZLIB_INSTALL/include -L$ZLIB_INSTALL/lib -lz && sed -i 's|-m64||g' Makefile" \
    --prefix=$TOP_DIR/build_aarch64 \
    --sbin-path=$WORK_DIR/bin/ \
    --modules-path=$WORK_DIR/lib/ \
    --conf-path=$WORK_DIR/etc/nginx.conf \
    --error-log-path=$WORK_DIR/log/nginx.error.log \
    --http-log-path=$WORK_DIR/log/nginx.access.log \
    --pid-path=$WORK_DIR/run/nginx.pid \
    --lock-path=$WORK_DIR/run/nginx.lock \
    --user=root \
    --group=root \
    --http-client-body-temp-path=$WORK_DIR/run/client_temp \
    --http-proxy-temp-path=$WORK_DIR/run/proxy_temp \
    --http-fastcgi-temp-path=$WORK_DIR/run/fastcgi_temp \
    --http-uwsgi-temp-path=$WORK_DIR/run/uwsgi_temp \
    --http-scgi-temp-path=$WORK_DIR/run/scgi_temp \
    --without-http_gzip_module \
    --with-debug \
    --with-http_addition_module \
    --with-http_auth_request_module \
    --with-http_dav_module \
    --with-http_degradation_module \
    --with-http_flv_module \
    --add-module=$NGX_HTTP_FLV_PATH \
    --without-http_upstream_zone_module \
    --with-http_gunzip_module \
    --with-http_gzip_static_module \
    --with-http_mp4_module \
    --with-http_realip_module \
    --with-http_secure_link_module \
    --with-http_slice_module \
    --with-http_ssl_module \
    --with-http_stub_status_module \
    --with-http_sub_module \
    --with-http_v2_module \
    --with-mail=dynamic \
    --with-mail_ssl_module \
    --with-pcre-jit \
    --without-stream_geo_module \
    --crossbuild=aarch64-linux-gnu \
    --with-cc=$CC \
    --with-cpp=$CXX \
    --with-cc-opt="-O2 -D_GNU_SOURCE"

# 1, fix pcre cross compile
sed -i 's|configure --disable-shared  --enable-jit|configure --disable-shared --enable-unicode-properties --enable-pcre16 --enable-pcre32 --enable-jit --enable-pcregrep-libz --target=arm-linux --host=aarch64-linux-gnu CC=$(CC) CXX=$(CXX)|g' objs/Makefile

make && make install

pwd
###### build nginx ######
