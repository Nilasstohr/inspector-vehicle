#!/bin/bash
# example of using arguments to a script
source install/setup.bash
sudo chmod 666 /dev/ttyUSB0
gnome-terminal --tab --title="Clion" --command="bash -c '../../../clion-2022.2/bin/clion.sh; $SHELL'; 'while [[ \$response != q ]]; do read -n 1 -p [q]uit? response; echo; done'"
gnome-terminal --tab --title="PyCharm" --command="bash -c '../../../pycharm-community-2022.2.1/bin/pycharm.sh; $SHELL'; 'while [[ \$response != q ]]; do read -n 1 -p [q]uit? response; echo; done'"
gnome-terminal --tab --title="eclipse" --command="bash -c '../../../eclipse/cpp-2022-06/eclipse/eclipse; $SHELL'; 'while [[ \$response != q ]]; do read -n 1 -p [q]uit? response; echo; done'"
gnome-terminal --tab --title="LidarNode" --command="bash -c 'ros2 run rplidar_ros rplidarNode node; $SHELL'; 'while [[ \$response != q ]]; do read -n 1 -p [q]uit? response; echo; done'"
#echo $(pwd)   

