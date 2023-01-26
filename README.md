Readme for RPiKeyerTerm software

![Example image](./RPiKeyerTerm.png)

NOTE: See KeyIt.png for an example of a simple transistor switch to use 
with the radio's CW key line.

REQUIRED LIBRARIES IN Raspberry Pi OS to run OR to build from source.

````
sudo apt install gpiod libgpiod-dev libgpiod-doc qtbase5-dev
````

To build from source:
````
cd ~
mkdir src
cd src
git clone https://github.com/guitarpicva/RPiKeyerTerm.git
cd RPiKeyerTerm
mkdir build
cd build
qmake ..
make -j4
````
If all goes well, run with ./RPiKeyerTerm &

From the "build" folder, create an application installation area:
````
mkdir ~/RPiKeyerTerm
cp RPiKeyerTerm ~/RPiKeyerTerm
cd ~/RPiKeyerTerm
touch RPiKeyerTerm.sh
chmod +x RPiKeyerTerm.sh
nano RPiKeyerTerm.sh
````
Make the contents of this file to be:

````
#!/bin/bash
cd ~/RPiKeyerTerm
./RPiKeyerTerm &
exit 0
````

Save the file with Ctrl-O <Enter> Ctrl-X
