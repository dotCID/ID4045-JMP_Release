#!/bin/bash
g++ server.cpp -o server.out -I. -L.
chmod a+x ./runServer.sh
ln -s runServer.sh ~/Desktop/PAK_Prototype_Server