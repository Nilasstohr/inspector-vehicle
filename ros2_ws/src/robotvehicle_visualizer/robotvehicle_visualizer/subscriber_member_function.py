# Copyright 2016 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import math

import rclpy
import numpy as np
import matplotlib.pyplot as plt
from rclpy.node import Node

from std_msgs.msg import String
from custom_interfaces.msg import OccupancyGrid


class MinimalSubscriber(Node):

    def __init__(self):
        super().__init__('minimal_subscriber')
        self.subscription = self.create_subscription(
            String,
            'topic_grid_map',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, string):
        self.get_logger().info('got grid map update')
        grid_map_matrix = self.get_gridmap_with_path(string)
        plt.imshow(grid_map_matrix, origin='lower')
        plt.axis([0, 250, 0, 250])
        plt.colorbar()
        plt.show()

    def get_gridmap_with_path(self, string):
        lines = string.data.split("\n")
        path_detected = False
        grid_map_matrix = np.empty([0, 250])
        for line in lines:
            if "path" in line:
                path_detected = True
                continue
            if path_detected:
                p = np.fromstring(line, dtype=int, sep=' ')
                x = p[0]
                y = p[1]
                grid_map_matrix[x, y] = -5
            else:
                r = np.fromstring(line, dtype=float, sep=' ')
                grid_map_matrix = np.vstack((grid_map_matrix, r))
        return grid_map_matrix.T


class ScanPoint:
    __x = None
    __y = None

    def __init__(self, alfa, p):
        self.__x = 40 + p * math.cos(alfa + 0)
        self.__y = 40 + p * math.sin(alfa + 0)

    def getPoint(self):
        return self.__x, self.__y

class PlayBackTesting:
    recording = []

    def __init__(self):
        self.load_records()
        self.play_records()

    def load_records(self):
        new_measurement = False
        file = open("../../../../doc/Measurements/realtime/record.txt", "r+")
        scan = None
        lines = file.readlines()
        for line in lines:
            if "new" in line:
                new_measurement = True
                if scan is not None:
                    self.recording.append(scan)
                continue
            if new_measurement:
                scan = []
                new_measurement = False
                continue
            angle, distance = self.get_values_from_line(line)
            if distance is not float("inf"):
                scan.append(ScanPoint(angle, distance))

    @staticmethod
    def get_values_from_line(line):
        values = np.fromstring(line, dtype=float, sep=',')
        return values[0], values[1]

    @staticmethod
    def get_array_from_scan(scan):
        x_array = np.empty(0)
        y_array = np.empty(0)
        for scan_point in scan:
            x, y = scan_point.getPoint()
            x_array = np.append(x_array, x)
            y_array = np.append(y_array, y)
        return x_array, y_array

    def play_records(self):
        for scan in self.recording:
            x, y = self.get_array_from_scan(scan)
            plt.scatter(x, y, s=50, color='green')
            #plt.plot(x, y)
            plt.axis([0, 250, 0, 150])
            plt.grid()
            plt.ylabel('some numbers')
            plt.show()



def main(args=None):
    #PlayBackTesting()
    rclpy.init(args=args)
    minimal_subscriber = MinimalSubscriber()
    rclpy.spin(minimal_subscriber)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
