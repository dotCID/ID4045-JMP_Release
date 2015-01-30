#!/bin/bash
g++ server.cpp -o server.out -I. -L.
chmod a+x ./runServer.sh
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ln -s DIR/runServer.sh ~/Desktop/PAK_Prototype_Server