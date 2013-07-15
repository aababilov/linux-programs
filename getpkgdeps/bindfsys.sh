#!/bin/sh

for i in dev sys proc ; do
    if [ ! -d mysys/$i ]; then
        mkdir mysys/$i
    fi
    mount --rbind /$i mysys/$i
done