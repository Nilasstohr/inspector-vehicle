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
import numpy as np

import rclpy
from rclpy.node import Node

from std_msgs.msg import String
from matplotlib import pyplot as plt
from sensor_msgs.msg import LaserScan


class MonitorNode(Node):
    poses = None
    index = None

    def __init__(self):
        super().__init__('monitor_node')
        self.poses = np.zeros(shape=(10000, 3))
        self.index = 0
        # self.subscription = self.create_subscription(
        #    LaserScan,
        #    'scan',
        #    self.listener_callback,
        #    10)

        self.subscription = self.create_subscription(
            String,
            'topic_pose_string',
            self.listener_callback_pose_string,
            10)

        self.subscription  # prevent unused variable warning

    # def listener_callback(self, msg):
    #    self.get_logger().info('I------------ "%d"' % msg.ranges[0])

    def listener_callback_pose_string(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.data)
        if msg.data != "end":
            pose = msg.data.split()
            x = float(pose[0])
            y = float(pose[1])
            theta = float(pose[2])
            self.poses[self.index][:] = np.array([x, y, theta])
            self.index = self.index + 1
        else:
            self.index = self.index - 1
            x = self.poses[0:self.index, 0]
            y = self.poses[0:self.index, 1]
            xm = np.array([40, 202, 119])
            ym = np.array([40, 54, 90.5])
            plt.scatter(xm, ym, s=50, color='green')
            plt.plot(x, y)
            plt.axis([0, 250, 0, 150])
            plt.grid()
            plt.ylabel('some numbers')
            plt.show()
            self.index = 0

def main(args=None):
    rclpy.init(args=args)

    monitor_node = MonitorNode()

    rclpy.spin(monitor_node)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    monitor_node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
