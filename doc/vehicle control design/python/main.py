import numpy as np
# import serial.tools.list_ports
from matplotlib import pyplot as plt, ticker as mticker
import numpy.fft
from numpy import loadtxt
from scipy import signal
from scipy.signal import savgol_filter, lfilter, butter

from Localization import Localization


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


def velocity_controller_design():
    y = loadtxt("data1.txt")
    wLeft = y[:, 0]
    wRight = y[:, 1]
    # w_filtered = y[:, 2]
    n = len(wLeft)
    Ts = 100 / pow(10, 6)
    x = np.arange(n) * Ts

    wLeft[wLeft == np.inf] = 0

    w_savgolf = savgol_filter(wLeft, 51, 3)
    w_lowass = butter_lowpass_filter(wLeft, 110, 1 / Ts, order=1)

    wlast = w_savgolf[n - 1]
    tau = np.interp(wlast * 0.63, wLeft, x)
    fcut = 1 / tau
    OmegaCut = fcut / np.pi
    print(tau, " s")
    print(OmegaCut, " rad/sec")
    print(fcut, " Hz")

    b = 349
    a = 15.87
    num = [0, 0, b]
    den = [0, 1, a]
    T = signal.lti(num, den)
    t, w_sim = signal.step(T)
    # axs[0].plot(t, yt*12.44, color='green')
    # axs[0].set(xlabel='t[s]', ylabel='Amplitude')
    # axs[2].title('Step response for 1. Order Lowpass')

    fig, axs = plt.subplots(2)
    fig.suptitle('Transient Time Response')
    axs[0].plot(x, wLeft, color='blue')
    # axs[0].plot(x, w_savgolf, color='red')
    # axs[0].plot(x, w_lowass, color='green')
    axs[0].plot(x, wRight, color='green')
    # axs[0].plot(t, w_sim, color='red')
    axs[0].set(xlabel='t[s]', ylabel='\u03C9 [radians/second]')
    axs[0].grid()
    axs[0].axis(xmin=0, xmax=max(x), ymin=0, ymax=max(wLeft) + 2)

    # Fs = 8000
    # f = 1000
    # N = 8000
    # x = np.arange(N)
    # y = np.sin(2 * np.pi * f * x / Fs)

    WLeft, omega = fft(wLeft, Ts)
    WRight, omega = fft(wRight, Ts)
    Wsavgol, omega = fft(w_savgolf, Ts)
    Wlowpass, omega = fft(w_lowass, Ts)

    # y = data
    # plt.plot(x, y)
    # plt.xlabel('sample(n)')
    # plt.ylabel('voltage(V)')
    # plt.show()
    axs[1].semilogx(omega, WLeft, color='blue')
    axs[1].semilogx(omega, Wsavgol, color='red')
    axs[1].semilogx(omega, Wlowpass, color='green')
    axs[1].xaxis.set_minor_formatter(mticker.ScalarFormatter())
    axs[1].grid()
    axs[1].set(xlabel='\u03C9 [radians/second]', ylabel='dB')
    plt.show()


if __name__ == '__main__':
     velocity_controller_design()

     #a = Localization()

#    omega_0 = 15
#    r = 4.5
#    d_end = 50
#    d_start = 30
#    fi_end = d_end / r  # end distance
#    fi_start = d_start / r  # start deaccelerate at distance
#    alfa = -pow(omega_0, 2) / (2 * (fi_end - fi_start))
#    print(alfa)
#    d = np.arange(start=30, stop=50 + 0.1, step=0.1)
#    w = np.sqrt(pow(omega_0, 2) + 2 * alfa * (d/r - fi_start))
#    print(d)
#    print(w)
