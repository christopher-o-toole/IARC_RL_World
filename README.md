# Installation
## Ubuntu 16.04

### Install Dependencies

First, install some necessary command line utilities and development libraries:
```
sudo apt-get update
sudo apt-get install -y build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev libboost-all-dev
sudo apt-get install -y curl git ccache gawk
sudo apt-get install -y python-pip
pip install --upgrade pip
sudo apt-get remove python-pip
```

#### Boost Installation
Version 1.65 of the Boost libraries for C++ is necessary for the compilation of the ArduPilot Gazebo plugin. To start off the boost installation process, unpack the source code for Boost in your home directory:
```
cd ~
wget https://cfhcable.dl.sourceforge.net/project/boost/boost/1.65.0/boost_1_65_0.tar.bz2
tar -xvjf boost_1_65_0.tar.bz2
cd ~/boost_1_65_0
```
Now build the Boost libraries from source:
```
./bootstrap.sh --prefix=/usr/local
./b2 -j4
```
If Boost was built succcessfully, complete the installation with the following commands:
```
sudo ./b2 -j4 install
sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf'
sudo ldconfig
```

### ArduPilot Installation

Start by installing some Python dependencies for ArduPilot:
```
sudo -E env "PATH=$PATH" pip2 install -U setuptools future lxml matplotlib opencv-python scipy numpy sklearn serial pymavlink MAVProxy
```
Clone the latest stable release of ArduPilot (3.5.5 on 2/28/2018):
```
cd ~
git clone https://github.com/ArduPilot/ardupilot
cd ardupilot
git checkout Copter-3.5.5
git submodule update --init --recursive
```
Add some directories to your system's path:
```
echo 'export PATH=$PATH:$HOME/ardupilot/Tools/autotest' >> ~/.bashrc
echo 'export PATH=/usr/lib/ccache:$PATH' >> ~/.bashrc
source ~/.bashrc
```
Move into the directory for ArduCopter
```
cd ~/ardupilot/ArduCopter
```
Compile ArduPilot
```
sim_vehicle.py -w
```

### Gazebo Installation

Install the latest stable version of Gazebo:
```
curl -ssL http://get.gazebosim.org | sh
sudo apt-get install libgazebo9-dev
```
Setup an environment to run Gazebo in:
```
echo 'source /usr/share/gazebo-9/setup.sh' >> ~/.bashrc
echo 'export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:/usr/share/gazebo-9/models' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:/usr/lib/x86_64-linux-gnu/gazebo-9/plugins' >> ~/.bashrc
source ~/.bashrc
```

### IARC Reinforcement Learning World Installation

#### Intel RealSense for Gazebo Installation
Clone the repo and build the RealSense plugin for Gazebo:
```
cd ~
git clone https://github.com/intel/gazebo-realsense.git
cd gazebo-realsense
mkdir build
cd build
cmake ..
make -j4
```
Add the Intel RealSense model and plugins to the Gazebo environment:
```
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/gazebo-realsense/build/gzrs' >> ~/.bashrc
echo 'export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:~/gazebo-realsense/models' >> ~/.bashrc
source ~/.bashrc
```
#### ArduPilot for Gazebo Installation
Build and install the ArduPilot plugin for Gazebo:
```
cd ~
git clone https://github.com/christopher-o-toole/ardupilot_gazebo.git
cd ardupilot_gazebo
mkdir build
cd build
cmake ..
make -j4
sudo make install
```
Add the ArduPilot drone models to your Gazebo environment:
```
echo 'export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:~/ardupilot_gazebo/gazebo_models' >> ~/.bashrc
source ~/.bashrc
```
#### Gazebo World Installation

Clone the repo:
```
cd ~
git clone https://github.com/christopher-o-toole/IARC_RL_World.git
cd IARC_RL_World
```
Compile the plugins:
```
cd GameScoreManager
rm -rf build
mkdir build
cd build
cmake .. 
make -j4

cd ../../Roomba
rm -rf build
mkdir build
cd build
cmake .. 
make -j4

cd ../../TimeManager
rm -rf build
mkdir build
cd build
cmake .. 
make -j4

cd ../../WorldController
rm -rf build
mkdir build
cd build
cmake .. 
make -j4

```
Add the world's assets to your Gazebo environment:
```
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/IARC_RL_World/WorldController/build' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/IARC_RL_World/TimeManager/build' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/IARC_RL_World/GameScoreManager/build' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/IARC_RL_World/Roomba/build' >> ~/.bashrc
echo 'export GAZEBO_RESOURCE_PATH=${GAZEBO_RESOURCE_PATH}:~/IARC_RL_World/Assets/materials/textures' >> ~/.bashrc
source ~/.bashrc
```
Try the world out!
```
cd ~/IARC_RL_World
gazebo --verbose IARC_RL_World.world
```

If all goes well, your world should look something like this:

![example](https://raw.githubusercontent.com/christopher-o-toole/IARC_RL_World/master/example.png)
