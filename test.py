import numpy as np
from matplotlib import pyplot as plt
from scipy.signal import find_peaks
import sys

def ZC_gen(root: int, Nzc: int) -> np.ndarray:
    d_u = np.zeros(Nzc, dtype=np.complex128)

    for n in range(1, Nzc):
        nd = float(n)
        ud = float(root)

        if Nzc % 2 == 0:
            d_u[n] = np.exp(-1j * np.pi * ud * nd * nd / Nzc)
        else:
            d_u[n] = np.exp(-1j * np.pi * ud * nd * (nd + 1) / Nzc)

    return d_u


import numpy as np

def ZC_corr(samples: np.ndarray, ZC: np.ndarray) -> np.ndarray:
    corr_func = []

    B = 0.0
    for i in range(len(ZC)):
        B += np.abs(ZC[i])**2

    for k in range(len(samples) - len(ZC) + 1):
        R = 0 + 0j
        A = 0.0

        for i in range(len(ZC)):
            R += samples[k + i] * np.conj(ZC[i])
            A += np.abs(samples[k + i])**2

        coeff = np.sqrt(A * B)
        corr_func.append(np.abs(R) / coeff if coeff != 0 else 0.0)

    return np.array(corr_func)

# =============================================================== PARAMS ===================================================================================
FFT_size = 128
CP_size = 24

# =============================================================== READ AND SLICE ===================================================================================
signal = np.fromfile("signal.pcm", dtype=np.int16)
complex_signal = signal[0::2] + 1j * signal[1::2]

start = int(5e6+17780)
end = int(start + 1920*3)

slice_signal = complex_signal[start:end]

plt.plot(np.real(slice_signal))
plt.plot(np.imag(slice_signal))
plt.show()

# =============================================================== GENERATE ZADOV-CHU ===================================================================================
zc = ZC_gen(25, FFT_size)
zc = np.fft.ifft(zc, FFT_size)

# =============================================================== PSS CORRELATION ===================================================================================
zc_corr = ZC_corr(slice_signal, zc)
plt.plot(zc_corr)
plt.show()

# =============================================================== FIND PEAKS ===================================================================================
pss_peaks = find_peaks(zc_corr, 0.7)
pss_peaks = pss_peaks[0]

if(len(pss_peaks) != 2):
    print("ERROR: len(pss_peaks) != 2!")
    sys.exit(1)

print(f"ZC PEAKS: {pss_peaks}")

# =============================================================== SLICE SIGNAL ===================================================================================
start = pss_peaks[0] + FFT_size
end = pss_peaks[1] - CP_size

post_pss_signal = slice_signal[start:end]

print(f"POST FRAME_SYNC SIGNAL LEN: {len(post_pss_signal)}")



