WIERI
=====
---
Simple Wiegand 26 Protocol Tester for Raspberry Pi.


INSTALLATION GUIDE
==================
---
Basic updates
-------------
- `$ sudo apt-get update`
- `$ sudo apt-get upgrade`


Clone project
-------------
- `$ git clone https://github.com/spac3-monk3y/wieri.git`
- Move to project's folder
 - `$ cd wieri`


Install CMake
-------------
- `$ sudo apt-get install cmake`


Install dependencies
----------------------------

__1- Download simpleini__
- Move to `libs` folder
 - `$ cd libs`
- `$ git clone https://github.com/brofield/simpleini.git`

__2- Install [WiringPI](http://wiringpi.com/download-and-install/)__


Compile
-------
- `$ cd wieri`
- `$ cmake ./`
- `$ make`

Configure
---------
- `conf.ini`

Run
---
- `sudo ./wieri`
