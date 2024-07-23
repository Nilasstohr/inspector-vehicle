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

        grid_string = string.data.split("\n")
        m = np.empty([0, 250])
        for row in grid_string:
            r = np.fromstring(row, dtype=float, sep=' ')
            m = np.vstack((m, r))
        # print(m)
        m = m.T
        plt.imshow(m, origin='lower')
        plt.axis([0, 250, 0, 150])
        plt.colorbar()
        plt.show()


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
