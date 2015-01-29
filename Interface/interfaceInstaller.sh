#!/bin/bash
g++ Interface.cpp `pkg-config --libs gtk+-2.0` `pkg-config --cflags gtk+-2.0` -o interface.out -I. -L.
chmod a+x ./runInterface.sh
ln -s runInterface.sh ~/Desktop/PAK_Prototype_Interface