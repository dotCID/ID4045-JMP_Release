#!/bin/bash
g++ Interface.cpp `pkg-config --libs gtk+-2.0` `pkg-config --cflags gtk+-2.0` -o interface.out -I. -L.
chmod a+x ./runInterface.sh
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ln -s DIR/runInterface.sh ~/Desktop/PAK_Prototype_Interface