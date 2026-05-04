import numpy as np
# import serial.tools.list_ports
from matplotlib import pyplot as plt, ticker as mticker
import numpy.fft
from numpy import loadtxt
from scipy import signal
from scipy.signal import savgol_filter, lfilter, butter

N = 3200 # counts per revolution

t_count = 93.8

v_rad_s  = 2 * np.pi * pow(10, 6) / (N * t_count)

print(v_rad_s, " rad/s")

v_rad_s = 10;
t_uq  = 2*np.pi*pow(10,6)/(N*v_rad_s)

print(t_uq, " us")
print(t_uq/1000, " ms")
