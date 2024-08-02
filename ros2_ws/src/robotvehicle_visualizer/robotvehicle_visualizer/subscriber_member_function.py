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
                grid_map_matrix[x, y] = 0.5
            else:
                r = np.fromstring(line, dtype=float, sep=' ')
                grid_map_matrix = np.vstack((grid_map_matrix, r))
        return grid_map_matrix.T


def main(args=None):
    a = np.matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9]])

    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
