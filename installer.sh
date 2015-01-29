#!/bin/bash

add-apt-repository universe
apt-get update
apt-get install ekiga g++ libgtk2.0-dev build-essential git pkg-config arduino arduino-core -y

git clone https://github.com/dotcid/ID4045-JMP_Complete-Code
cd Complete-Code/Interface
chmod a+x ./interfaceInstaller.sh
./interfaceInstaller.sh

cd ../Server
g++ server.cpp -o server.out -I. -L.
ln -s server.out ~/Desktop/PAK_Prototype_Server

cd ../Arduino
mkdir ~/sketchbook
mkdir ~/sketchbook/libraries
cp PAK_Arduino_Code.ino ~/sketchbook/
cp ./libraries/* ~/sketchbook/libraries