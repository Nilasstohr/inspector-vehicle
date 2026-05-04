#!/bin/bash
gnome-terminal --tab --title="PyCharm"   --command="bash -c '../../../pycharm-community-2024.2.1/bin/pycharm.sh; $SHELL'; 'while [[ \$response != q ]]; do read -n 1 -p [q]uit? response; echo; done'"

