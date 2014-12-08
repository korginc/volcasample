#!/bin/sh

# This is a wrapper script for the syro_volcasample_exsample sample loader.
#
# Copyright © 2014 Per Andersson <avtobiff@foo.nu>
# This file is licensed under the same terms as volca sample SDK.

set -e

DEBUG=false

SCRIPT_PATH="$(readlink -f $0)"
SCRIPT_NAME="$(basename $SCRIPT_PATH)"
SCRIPT_DIR="$(dirname $SCRIPT_PATH)"

ARCH=$(arch)
SYRO_CMD="$SCRIPT_DIR/syro_volcasample_example.$ARCH"
SYRO_TARGET_FILE="syro_stream.wav"

CONVERT_ALREADY=false
PLAY_SYRO_STREAM=ask


usage() {
cat << EOF
Usage: $SCRIPT_NAME [options] source_file

Options:
  -c,  --convert                  don't ask for permission before converting.
  -d,  --dont-play                don't play resulting syro stream.
  -h,  --help                     print this help and exit.
  -n,  --sample-number=NUMBER     set syro stream sample number to NUMBER,
                                  valid range is 0-99. [required]
  -o,  --output=FILE              output syro stream to FILE. [optional]
  -p,  --play                     play resulting syro stream with aplay,
                                  without asking. This automatically removes
                                  output syro stream file after playing.
EOF
}

TEMP=$(getopt --options cdhn:o:p \
              --long convert,dont-play,help,sample-number:,output:,play \
              --name makesyro_gnulinux.sh -- "$@")

eval set -- "$TEMP"

# no options supplied
[ $# -le 1 ] && { usage; exit 1; }

# extract source file name
for SYRO_SRC_FILE; do : ; done

# parse options
while true; do
    case "$1" in
        -c|--convert)
            CONVERT_ALREADY=true
            shift
            ;;
        -d|--dont-play)
            PLAY_SYRO_STREAM=false
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -n|--sample-number)
            SYRO_NO="$2"
            shift 2
            ;;
        -o|--output)
            SYRO_TARGET_FILE="$2"
            shift 2
            ;;
        -p|--play)
            PLAY_SYRO_STREAM=true
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            # not internal error, just source file name in options
            [ "$SYRO_SRC_FILE" = "$2" ] && continue
            usage
            exit 1
            ;;
    esac
done

# validate input

# no source file supplied
if [ "$SYRO_SRC_FILE" = "--" ]; then
    echo "$SCRIPT_NAME: missing source file"
    usage
    exit 1
fi

# source file does not exist
if [ ! -f "$SYRO_SRC_FILE" ]; then
    echo "$SCRIPT_NAME: supplied source file does not exist"
    exit 1
fi

# sample number is required
if [ -z "$SYRO_NO" ]; then
    echo "$SCRIPT_NAME: sample number is required"
    usage
    exit 1
fi

# sample number is not a number or outside [0, 99] range
if [ "$SYRO_NO" -eq "$SYRO_NO" 2>/dev/null ]; then
    if [ "$SYRO_NO" -lt 0 ] || [ "$SYRO_NO" -gt 99 ]; then
        echo "$SCRIPT_NAME: supplied sample number is outside 0-99 range"
        exit 1
    fi
else
    echo "$SCRIPT_NAME: supplied sample number is not a number"
    usage
    exit 1
fi

if [ "$DEBUG" = "true" ]; then
    echo "convert without asking: $CONVERT_ALREADY"
    echo "sample number: $SYRO_NO"
    echo "output: $SYRO_TARGET_FILE"
    echo "play: $PLAY_SYRO_STREAM"
    echo "source: $SYRO_SRC_FILE"
fi

# convert, don't ask if user supplied --convert
if [ "$CONVERT_ALREADY" = "true" ]; then
    convert=y
else
    printf "Convert input file $SYRO_SRC_FILE to $SYRO_TARGET_FILE? [Y/n] "
    read convert
fi

if [ "$convert" = "" ] || [ "$convert" = "y" ] || [ "$convert" = "Y" ]; then
    echo "$SCRIPT_NAME: converting $SYRO_SRC_FILE to $SYRO_TARGET_FILE..."
else
    exit
fi

# execute conversion
cd "$SCRIPT_DIR"
$SYRO_CMD \
    "$SYRO_TARGET_FILE" \
    s${SYRO_NO}c:"${SYRO_SRC_FILE}"


# play syro stream with aplay

# don't ask user if they already supplied --dont-play or --play options
if [ "$PLAY_SYRO_STREAM" = "true" ]; then
    echo "$SCRIPT_NAME: playing $SYRO_TARGET_FILE with aplay..."
    aplay $SYRO_TARGET_FILE
    echo "$SCRIPT_NAME: removing $SYRO_TARGET_FILE..."
    rm -f $SYRO_TARGET_FILE
    exit
fi

[ "$PLAY_SYRO_STREAM" = "false" ] && exit

# ask user if they want the result played
printf "Play resulting syro stream with aplay? [Y/n] "
read play
if [ "$play" = "" ] || [ "$play" = "y" ] || [ "$play" = "Y" ]; then
    echo "$SCRIPT_NAME: playing $SYRO_TARGET_FILE with aplay..."
else
    echo "Aborting play of syro stream..."
    exit
fi

aplay "$SYRO_TARGET_FILE"
