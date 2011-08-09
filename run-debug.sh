#!/bin/sh
# Q3Rally (debug) Unix Launcher

PLATFORM=`uname|sed -e s/_.*//|tr '[:upper:]' '[:lower:]'|sed -e 's/\//_/g'`
ARCH=`uname -m | sed -e s/i.86/i386/`
BUILD=debug

BIN=engine/build/$BUILD-$PLATFORM-$ARCH/q3rally.$ARCH

if [ ! -f $BIN ]; then
	echo "Game binary '$BIN' not found!"
	echo "Run 'make -C engine $BUILD' to build it."
	exit 1
fi

if [ ! -f baseq3r/vm/ui.qvm ]; then
	echo "QVMs not found!"
	echo "Run 'cp engine/build/$BUILD-$PLATFORM-$ARCH/baseq3r/vm/*.qvm baseq3r/vm/'"
	exit 1
fi

# Run the game
./$BIN +set fs_basepath "." +set vm_game 0 +set vm_cgame 0 +set vm_ui 0 $@

