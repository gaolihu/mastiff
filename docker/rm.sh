#!/bin/bash

NGINX_SVR_NAME=mstfsvr

docker stop $NGINX_SVR_NAME
docker rm $NGINX_SVR_NAME
