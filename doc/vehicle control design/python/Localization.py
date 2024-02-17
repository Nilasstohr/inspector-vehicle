import json

import numpy as np
from matplotlib import pyplot as plt


class Localization:
    def __init__(self):
        data_file_path = "/home/robot1/devrepo/inspector-vehicle/ros2_ws/build/robotvehicle_package/log.json"
        # with open(data_file_path, 'r') as file:
        #   filedata = file.read()
        # Replace the target string
        # filedata = filedata.replace('""inf""', '"inf"')
        # Write the file out again
        # with open(data_file_path, 'w') as file:
        #    file.write(filedata)
        data = open(data_file_path)
        data = json.load(data)
        data = data['data']
        n = len(data)
        x_buf = []
        y_buf = []

        for i in range(0, n - 1):
            odom = data[i]['odom']
            x_buf = np.append(x_buf, odom['x'])
            y_buf = np.append(y_buf, odom['y'])

        xm = np.array([40, 200, 118.5])
        ym = np.array([40, 52.5, 89.5])
        plt.scatter(xm, ym)
        plt.plot(x_buf, y_buf)
        plt.grid()
        plt.ylabel('some numbers')
        plt.show()
