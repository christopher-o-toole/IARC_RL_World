# Installation
## Ubuntu 16.04

### Install Dependencies

Install the necessary command line utilities:

```
sudo apt-get install curl git ccache gawk
sudo apt-get install python-pip
pip install --upgrade pip
sudo apt-get remove python-pip
```

### ArduPilot Installation

Start by installing some Python dependencies for Ardupilot:
```
sudo -E env "PATH=$PATH" pip2 install -U setuptools future lxml matplotlib opencv-python scipy numpy sklearn serial pymavlink MAVProxy
```
Clone the latest stable release of ArduPilot (3.5.5 on 2/28/2018):
```
cd ~
git clone https://github.com/ArduPilot/ardupilot
cd ardupilot
get checkout Copter-3.5.5
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
cd ardupilot/ArduCopter
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
echo 'export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:/usr/share/gazebo-9/models' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:/usr/lib/x86_64-linux-gnu/gazebo-9/plugins' >> ~/.bashrc
echo 'source /usr/share/gazebo-9/setup.sh' >> ~/.bashrc
source ~/.bashrc
```

### IARC Reinforcement Learning World Installation

Clone the repo:
```
cd ~
git clone https://github.com/christopher-o-toole/IARC_RL_World.git
cd IARC_RL_World
```
Compile the plugins:
```
cd GameScoreManager
mkdir build
cd build
cmake .. 
make -j4

cd ../Roomba
mkdir build
cd build
cmake .. 
make -j4

cd ../TimeManager
mkdir build
cd build
cmake .. 
make -j4

cd ../WorldController
mkdir build
cd build
cmake .. 
make -j4
```
Add the world to your Gazebo environment:
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
Setup an environment to run Gazebo in:
```
echo 'export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:/usr/share/gazebo-9/models' >> ~/.bashrc
echo 'export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:/usr/lib/x86_64-linux-gnu/gazebo-9/plugins' >> ~/.bashrc
echo 'source /usr/share/gazebo-9/setup.sh' >> ~/.bashrc
source ~/.bashrc
```

### IARC Reinforcement Learning World Installation

