import numpy as np
#import serial.tools.list_ports
from matplotlib import pyplot as plt, ticker as mticker
import numpy.fft
from numpy import loadtxt
from scipy.signal import savgol_filter, lfilter, butter


def butter_lowpass(cutoff, fs, order=1):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)


def butter_lowpass_filter(data, cutoff, fs, order=1):
    b, a = butter_lowpass(cutoff, fs, order=order)
    print("b", b)
    print("a", a)
    y = lfilter(b, a, data)
    return y


def fft(data, Ts):
    fs = 1 / Ts
    N = len(data)
    yfft = numpy.fft.fft(data, N) / N
    NP = int(numpy.floor(N / 2))
    yfftp = 2.0 * yfft[1:NP + 1]
    yfftabs = 20.0 * numpy.log10(abs(yfftp))
    f = numpy.linspace(0, fs / 2, NP)
    omega = f * 2 * np.pi
    return yfftabs, omega


if __name__ == '__main__':
    y = loadtxt("data1.txt")
    w = y[:, 1]
    n = len(w)
    Ts = 100 / pow(10, 6)
    x = np.arange(n) * Ts

    w[w == np.inf] = 0

    w_savgolf = savgol_filter(w, 51, 3)
    w_lowass = butter_lowpass_filter(w, 110, 1/Ts, order=1)

    fig, axs = plt.subplots(2)
    fig.suptitle('Transient Time Response')
    axs[0].plot(x, w, color='blue')
    axs[0].plot(x, w_savgolf, color='red')
    axs[0].plot(x, w_lowass, color='green')
    axs[0].set(xlabel='t[s]', ylabel='\u03C9 [radians/second]')
    axs[0].grid()
    axs[0].axis(xmin=0, xmax=max(x), ymin=0, ymax=max(w) + 2)

    # Fs = 8000
    # f = 1000
    # N = 8000
    # x = np.arange(N)
    # y = np.sin(2 * np.pi * f * x / Fs)

    wlast = w_savgolf[n - 1]

    tau = np.interp(wlast * 0.63, w, x)
    fcut = 1 / tau
    OmegaCut = fcut / np.pi
    print(tau, " s")
    print(OmegaCut, " rad/sec")
    print(fcut, " Hz")

    W, omega = fft(w, Ts)
    Wsavgol, omega = fft(w_savgolf, Ts)
    Wlowpass, omega = fft(w_lowass, Ts)


    # y = data
    # plt.plot(x, y)
    # plt.xlabel('sample(n)')
    # plt.ylabel('voltage(V)')
    # plt.show()
    axs[1].semilogx(omega, W, color='blue')
    axs[1].semilogx(omega, Wsavgol, color='red')
    axs[1].semilogx(omega, Wlowpass, color='green')
    axs[1].xaxis.set_minor_formatter(mticker.ScalarFormatter())
    axs[1].grid()
    axs[1].set(xlabel='\u03C9 [radians/second]', ylabel='dB')
    plt.show()
data1.txt