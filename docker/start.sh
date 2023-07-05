#!/bin/bash

NGINX_SVR_NAME=mstfsvr
#NGINX_BASE_DIR=/home/lihu/Desktop/nginx
NGINX_BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

echo "working directory: $NGINX_BASE_DIR"

function run_2() {
    echo "starting docker"
    docker run \
        --name $NGINX_SVR_NAME \
        -d -it \
        -p 80:80 \
        --restart always \
        --privileged=true \
        -v $NGINX_BASE_DIR/logs/:/var/log/nginx/ \
        -v $NGINX_BASE_DIR/html/:/usr/share/nginx/html/ \
        -v $NGINX_BASE_DIR/conf/default.conf:/etc/nginx/conf.d/default.conf \
        -v $NGINX_BASE_DIR/conf/nginx.conf:/etc/nginx/nginx.conf \
        -v $NGINX_BASE_DIR/files/:/third_party/ \
        $NGINX_SVR_NAME
}

function run_1() {
    echo "setting docker env"
    docker run \
        --name $NGINX_SVR_NAME \
        -d -it \
        -p 80:80 \
        --privileged=true \
        -v $NGINX_BASE_DIR/logs/:/var/log/nginx/ \
        -v $NGINX_BASE_DIR/files/:/third_party/ \
        $NGINX_SVR_NAME

    docker cp $NGINX_SVR_NAME:/etc/nginx/conf.d/default.conf $NGINX_BASE_DIR/conf/default.conf
    docker cp $NGINX_SVR_NAME:/etc/nginx/nginx.conf $NGINX_BASE_DIR/conf/nginx.conf
    docker cp $NGINX_SVR_NAME:/usr/share/nginx/html $NGINX_BASE_DIR
}

if [ $# -eq 0 ]; then
    echo "param input 1 or 2"
    exit
fi

if [ $1 = "1" ]; then
    run_1
elif [ $1 = "2" ]; then
    run_2
else
    echo "param only 1 / 2"
fi
