import numpy as np

class RobotData:
    poses = None
    index = None
    grid_map_matrix = None
    def __init__(self):
        self.poses = np.zeros(shape=(10000, 3))
        self.index = 0

    def update(self, data_string):
        lines = data_string.split("\n")
        path_detected = False
        pose_detected = False
        grid_map_matrix = np.empty([0, 250])
        for line in lines:
            if "path" in line:
                path_detected = True
                continue
            if "pose" in line:
                pose_detected = True
                path_detected = False
                continue
            if path_detected:
                p = np.fromstring(line, dtype=int, sep=' ')
                x = p[0]
                y = p[1]
                grid_map_matrix [x, y] = -5
            elif pose_detected:
                self.update_pose(line)
                break
            else:
                r = np.fromstring(line, dtype=float, sep=' ')
                grid_map_matrix = np.vstack((grid_map_matrix , r))
        self.grid_map_matrix = grid_map_matrix.T

    def update_pose(self,line):
        pose = line.split()
        x = float(pose[0])
        y = float(pose[1])
        theta = float(pose[2])
        self.poses[self.index][:] = np.array([x, y, theta])
        self.index = self.index + 1

    def get_grid_map(self):
        return self.grid_map_matrix

    def get_x_poses(self):
        return self.poses[0:self.index, 0]

    def get_y_poses(self):
        return self.poses[0:self.index, 1]