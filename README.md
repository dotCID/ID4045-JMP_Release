# ID4045-JMP_Release
The complete release code for the JMP project of team PAK, including installation scripts

# Software installation instructions for the PAK JMP project prototype

The software for this prototype contains of three parts: the server code, the interface code, and the Arduino code. This software runs on Ubuntu Linux (and probably will on all Debian-based operating systems) 14.04.1 LTS, of which a “live CD” is provided. Please note that this system loses all data upon reboot. The Arduino code requires the latest version of the Arduino compiler. In case the installed version does not work, please download the latest version from the Arduino website. The code provided is already installed on the Arduino that is in the prototype; it does not need updating.

## Installation

Please open a terminal window and enter the following three commands:
```
ubuntu@ubuntu: ~$ wget https://raw.githubusercontent.com/dotCID/ID4045-JMP_Release/master/installer.sh
ubuntu@ubuntu: ~$ chmod a+x ./installer.sh
ubuntu@ubuntu: ~$ sudo ./installer.sh
```

This will download and install all necessary software, as well as build the executable files for your system. To run the server, please execute in a terminal:
```
ubuntu@ubuntu: ~$ ./ID4045-JMP_Release/Server/runServer.sh
```
To run the graphical interface to control the arm, please execute in a terminal:
```
ubuntu@ubuntu: ~$ ./ID4045-JMP_Release/Interface/runInterface.sh
```
These shell scripts will start the applications with the correct command line arguments for your convenience. They can be edited, or the server.out and interface.out files can be run directly. Please note that command line arguments for the server are mandatory, and for the interface are not; the connection settings of the interface can be altered via the interface itself while running.

In the interface, there are three available tabs. The default tab, “Sliders”, allows for control of the arm's rotation and extension, or of the horizontal and vertical location of the screen in relation to the base of the arm. The “Settings” tab allows the connection information to be changed, and the “Commands” tab contains buttons to send test commands to the Arduino.
