#!/bin/bash

# This script is used to launch a native mode stackline executable such as
# Contiki(-ng), RIOT, OpenThread...

# This script is not to be used directly from command line

DIR=`dirname $0`

. $DIR/../config.inc

[[ "$1" == "" ]] && echo "Usage: $0 <nativebin-to-launch> <args>" && exit 1

HOOK=`realpath $DIR/../$BINDIR/libwf_nativehook.so`

echo "Execution command: [LD_PRELOAD=$HOOK $*]"

LD_PRELOAD=$HOOK $*

echo "$1 exited"

sync
