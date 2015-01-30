#!/bin/bash
# from http://www.cyberciti.biz/tips/shell-root-user-check-script.html:
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

add-apt-repository universe
apt-get update
apt-get install ekiga g++ libgtk2.0-dev build-essential git pkg-config arduino arduino-core -y

git clone https://github.com/dotcid/ID4045-JMP_Release
cd ID4045-JMP_Release/Interface
chmod a+x ./interfaceInstaller.sh
./interfaceInstaller.sh

cd ../Server
chmod a+x ./serverInstaller.sh
./serverInstaller.sh

cd ../Arduino
mkdir ~/sketchbook
mkdir ~/sketchbook/PAK_Arduino_Code
mkdir ~/sketchbook/libraries
cp PAK_Arduino_Code.ino ~/sketchbook/PAK_Arduino_Code
cp -r ./libraries/* ~/sketchbook/libraries