import sys
import matplotlib.pyplot as plt
import numpy as np
import re

def rect_signal(tau, t_shift, timeline, U):
    return [U if i <= tau + t_shift and i >= 0 + t_shift  else 0  for i in timeline]

#cos params
T = 1
ALPHA = 0.25

timeline = np.arange(-30, 30, 0.1)

#impulse response
g = [(np.cos(np.pi*ALPHA*t)/T) / (1 - ((2*ALPHA*t) / T)**2) for t in timeline]

TAU = 10

signal = rect_signal(TAU, -TAU/2, timeline, 1)

conv = np.convolve(signal, g, mode="same")

plt.subplot(3, 1, 1)
plt.plot(timeline, g)

plt.subplot(3, 1, 2)
plt.plot(timeline, signal)

plt.subplot(3, 1, 3)
plt.plot(timeline, conv)
plt.show()