import math
import threading
import rclpy
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from rclpy.node import Node
import RobotData
from std_msgs.msg import String

class MinimalSubscriber(Node):
    robot_data = None
    data_ready = False

    def __init__(self):
        super().__init__('minimal_subscriber')
        self.subscription = self.create_subscription(
            String,
            'topic_grid_map',
            self.subscriber_callback,
            10)
        self.subscription  # prevent unused variable warning
        self.robot_data = RobotData.RobotData()
        threading.Thread(target=self.render_thread).start()

    def subscriber_callback(self, string):
        self.robot_data.update(string.data)
        if self.robot_data.has_reset():
            self.robot_data.clear_reset()
            return
        self.data_ready = True

    def render_thread(self):
        self.ani = FuncAnimation(plt.gcf(),
                     func=self.render_plot_timer_callback,interval=10,
                                 init_func=self.setup_plot_window, cache_frame_data=False)
        plt.show()

    def render_plot_timer_callback(self, i):
       if self.data_ready:
           self.setup_plot_window()
           x = self.robot_data.get_x_poses()
           y = self.robot_data.get_y_poses()
           #print("%.2f %.2f" % (self.robot_data.get_x_pose_last(),self.robot_data.get_y_pose_last()))
           c1 = plt.Circle((self.robot_data.get_x_pose_last(), self.robot_data.get_y_pose_last()), 18.5, color='red', fill=False)
           c2 = plt.Circle((self.robot_data.get_x_pose_last(), self.robot_data.get_y_pose_last()), 1, color='red')
           #c = plt.Circle((40,40), 18.5, color='red', fill=False)
           plt.gca().add_patch(c1)
           plt.gca().add_patch(c2)
           plt.imshow(self.robot_data.grid_map_matrix, origin='lower')
           plt.plot(x,y)
           plt.scatter(self.robot_data.get_x_scan(),self.robot_data.get_y_scan(),color='gray',s=3)
           #for i in range(0,self.robot_data.get_scan_points_size()):
           #    scan_point = self.robot_data.get_scan_point(i)
           #    xd = scan_point[1]
           #    yd = scan_point[2]
           #    x_last = self.robot_data.get_x_pose_last()
           #    y_last = self.robot_data.get_y_pose_last()
           #    plt.plot([x_last,xd],[y_last, yd],color='gray',linestyle = 'dotted')
           #plt.colorbar()
           self.data_ready = False

    def setup_plot_window(self):
        plt.cla()
        xm = np.array([40, 202, 119.5])
        ym = np.array([40, 54.5, 90.5])
        plt.scatter(xm, ym, s=50, color='green')
        plt.axis([0, 250, 0, 250])


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