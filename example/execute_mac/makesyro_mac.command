#!/usr/bin/env sh
SYRO_SRC_FILE="02 Kick 3.wav"
SYRO_NO='2'
SYRO_TARGET_FILE='./syro_stream.wav'

SYRO_PATH=./
cd `dirname $0`
$SYRO_PATH/syro_volcasample_example $SYRO_TARGET_FILE s${SYRO_NO}c:${SYRO_PATH}/"${SYRO_SRC_FILE}"
echo 'Press Enter'
read Wait
