import numpy as np
from ament_copyright.parser import is_empty_line


class RobotData:
    poses = None
    scan  = None
    lines_matched = None
    lines_unmatched = None
    lines_map = None
    pose_x_last = None
    pose_y_last = None
    index_poses = None
    index_scan  = None
    index_lines_matched = 0
    index_lines_unmatched = 0
    index_lines_map = 0
    grid_map_matrix = None
    reset_performed = False

    def __init__(self):
        self.init()

    def init(self):
        self.poses = np.zeros(shape=(10000, 3))
        self.scan = np.zeros(shape=(10000, 3))
        self.lines_matched = np.zeros(shape=(10000, 4))
        self.lines_unmatched = np.zeros(shape=(10000, 4))
        self.lines_map = np.zeros(shape=(10000, 4))
        self.index_poses = 0
        self.index_scan = 0

    def update(self, data_string):
        lines = data_string.split("\n")
        if "reset" in lines[0]:
            self.init()
            self.reset_performed = True
            return
        path_detected = False
        pose_detected = False
        scan_detected = False
        matched_lines_detected= False
        unmatched_lines_detected = False
        map_lines_detected = False
        grid_map_matrix = np.empty([0, 250])
        for line in lines:
            if "path" in line:
                path_detected = True
                continue
            if "pose" in line:
                pose_detected = True
                path_detected = False
                continue
            if "scan" in line:
                self.index_scan=0
                scan_detected = True
                pose_detected = False
                continue
            if "matchedlines" == line:
                self.index_lines_matched = 0
                matched_lines_detected = True
                scan_detected = False
                continue
            if "unmatchedlines" == line:
                self.index_lines_unmatched = 0
                unmatched_lines_detected = True
                matched_lines_detected = False
                continue
            if "maplines" == line:
                self.index_lines_map = 0
                map_lines_detected = True
                matched_lines_detected = False
                unmatched_lines_detected = False
                continue
            if path_detected:
                p = np.fromstring(line, dtype=int, sep=' ')
                x = p[0]
                y = p[1]
                grid_map_matrix [x, y] = -5
            elif pose_detected:
                self.update_pose(line)
            elif scan_detected:
                self.update_scan(line)
            elif matched_lines_detected:
                self.update_matched_lines(line)
            elif unmatched_lines_detected:
                self.update_unmatched_lines(line)
            elif map_lines_detected:
                self.update_map_lines(line)
            else:
                r = np.fromstring(line, dtype=float, sep=' ')
                grid_map_matrix = np.vstack((grid_map_matrix , r))
        self.grid_map_matrix = grid_map_matrix.T

    def update_pose(self,line):
        pose = line.split()
        x = float(pose[0])
        y = float(pose[1])
        theta = float(pose[2])
        self.poses[self.index_poses][:] = np.array([x, y, theta])
        self.index_poses = self.index_poses + 1
        self.pose_x_last = x
        self.pose_y_last = y

    def update_scan(self, line):
        if not line:
            return
        scan = line.split()
        alfa = float(scan[0])
        xp = float(scan[1])
        yp = float(scan[2])
        self.scan[self.index_scan][:] = np.array([alfa, xp, yp])
        self.index_scan = self.index_scan + 1

    def update_matched_lines(self, line):
        if not line:
            return
        line = line.split()
        x1 = float(line[0])
        y1 = float(line[1])
        x2 = float(line[2])
        y2 = float(line[3])
        self.lines_matched[self.index_lines_matched][:] = np.array([x1, y1, x2, y2])
        self.index_lines_matched = self.index_lines_matched + 1

    def update_unmatched_lines(self, line):
        if not line:
            return
        line = line.split()
        x1 = float(line[0])
        y1 = float(line[1])
        x2 = float(line[2])
        y2 = float(line[3])
        self.lines_unmatched[self.index_lines_unmatched][:] = np.array([x1, y1, x2, y2])
        self.index_lines_unmatched = self.index_lines_unmatched + 1

    def update_map_lines(self, line):
        if not line:
            return
        line = line.split()
        x1 = float(line[0])
        y1 = float(line[1])
        x2 = float(line[2])
        y2 = float(line[3])
        self.lines_map[self.index_lines_map][:] = np.array([x1, y1, x2, y2])
        self.index_lines_map = self.index_lines_map + 1

    def get_grid_map(self):
        return self.grid_map_matrix

    def get_x_poses(self):
        return self.poses[0:self.index_poses, 0]

    def get_y_poses(self):
        return self.poses[0:self.index_poses, 1]

    def get_theta_poses(self):
        return self.poses[0:self.index_poses, 2]

    def get_x_pose_last(self):
        return self.pose_x_last

    def get_y_pose_last(self):
        return self.pose_y_last

    def get_scan_point(self,i):
        return self.scan[i, :]

    def get_scan_points_size(self):
        return self.index_scan

    def get_x_scan(self):
        return self.scan[0:self.index_scan, 1]

    def get_y_scan(self):
        return self.scan[0:self.index_scan, 2]

    def has_matched_lines(self):
        return self.index_lines_matched >0

    def get_matched_lines_size(self):
        return self.index_lines_matched

    def get_matched_line_by_index(self, i):
        x1 = self.lines_matched[i,0]
        y1 = self.lines_matched[i,1]
        x2 = self.lines_matched[i,2]
        y2 = self.lines_matched[i,3]
        return x1,y1,x2,y2

    def has_unmatched_lines(self):
        return self.index_lines_unmatched >0

    def get_unmatched_lines_size(self):
        return self.index_lines_unmatched

    def get_unmatched_line_by_index(self, i):
        x1 = self.lines_unmatched[i,0]
        y1 = self.lines_unmatched[i,1]
        x2 = self.lines_unmatched[i,2]
        y2 = self.lines_unmatched[i,3]
        return x1,y1,x2,y2

    def has_map_lines(self):
        return self.index_lines_map >0

    def get_map_lines_size(self):
        return self.index_lines_map

    def get_map_line_by_index(self, i):
        x1 = self.lines_map[i,0]
        y1 = self.lines_map[i,1]
        x2 = self.lines_map[i,2]
        y2 = self.lines_map[i,3]
        return x1,y1,x2,y2

    def has_reset(self):
        return self.reset_performed

    def clear_reset(self):
        self.reset_performed=False



