import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import resample_poly

def gardner1(s, SPS=10):
    Kp, BnTs = 4, 0.01
    zeta = np.sqrt(2) / 2
    theta = (BnTs / 10) / (zeta + 1 / (4 * zeta))

    denom = (1 + 2 * zeta * theta + theta**2) * Kp
    K1 = -4 * zeta * theta / denom
    K2 = -4 * theta**2 / denom

    p2 = offset = 0
    offset_list, err_list = [], []

    for i in range(len(s)//SPS - 1):
        n = offset + SPS * i
        e = ((s[n + SPS].real - s[n].real) * s[n + SPS//2].real +
             (s[n + SPS].imag - s[n].imag) * s[n + SPS//2].imag)

        p2 = (p2 + e * (K1 + K2)) % 1
        offset = int(round(p2 * SPS))

        offset_list.append(offset)
        err_list.append(e)

    mean_err = []
    right = 0
    for y in range(2):
        for i in range(right - SPS//2, right + SPS//2 + 1):
            errs = []
            for x in range(0, len(s) - SPS*2 - i, SPS):
                of = x + i + SPS
                e = ((s[of + SPS].real - s[of].real) * s[of + SPS//2].real +
                     (s[of + SPS].imag - s[of].imag) * s[of + SPS//2].imag)
                errs.append(e)
            mean_err.append(np.mean(errs))
        right = int(np.argmin(np.abs(mean_err)))
        if y == 0:
            mean_err.clear()

    return (
        right,
        np.asarray(offset_list, int),
        np.asarray(err_list, float),
        np.asarray(mean_err, float)
    )

# def gardner2(complex_symbols_after_convolve, SPS = 10):
#     K1 = 0; K2 = 0; p1 = 0; p2 = 0; e = 0; offset = 0; Kp = 4
#     BnTs = 0.01; 
#     zeta = np.sqrt(2)/2
#     teta = ((BnTs)/10)/(zeta + 1/(4*zeta))
#     K1 = (-4*zeta*teta)/((1 + 2*zeta*teta + teta**2)*Kp)
#     K2 = (-4*teta**2)/((1 + 2*zeta*teta + teta**2)*Kp)
#     s = complex_symbols_after_convolve
#     offset_list = []
#     err_list = []
#     mean_err = []
#     for i in range(0, len(s)//SPS-1):
#         n = offset
#         e  = (np.real(s[n + SPS + SPS*i]) - np.real(s[n + SPS*i])) * np.real(s[n + SPS//2 + SPS*i])
#         e += (np.imag(s[n + SPS + SPS*i]) - np.imag(s[n + SPS*i])) * np.imag(s[n + SPS//2 + SPS*i])
#         p1 = e*K1
#         p2 += p1 + e * K2
#         p2 %= 1
#         offset = int(np.round(p2*SPS))

#         offset_list.append(offset)
#         err_list.append(e)
    
#     right = 0
#     for y in range(0, 2):
#         for i in range(right - SPS//2, right + SPS//2+1):
#             temp = []
#             for x in range(0, len(s) - SPS*2-i, SPS):
#                 of = x + i
#                 e  = (np.real(s[of + SPS + SPS]) - np.real(s[of + SPS])) * np.real(s[of + SPS//2 + SPS])
#                 e += (np.imag(s[of + SPS + SPS]) - np.imag(s[of + SPS])) * np.imag(s[of + SPS//2 + SPS])
#                 temp.append(e)
#             mean_err.append(np.mean(temp))
#             temp.clear()
#         right = int(np.round(np.argmin(np.abs(mean_err))))
#         if y != 1:
#             mean_err.clear()

#     offset_list = np.array(offset_list, dtype=int)
#     err_list = np.array(err_list, dtype=float)
#     mean_err = np.array(mean_err, dtype=float)

#     return right, offset_list, err_list, mean_err

# def gardner3(s, SPS=10):
#     Kp, BnTs = 0.02, 0.1
#     zeta = np.sqrt(2) / 2
#     teta = (BnTs / 10) / (zeta + 1 / (4 * zeta))
#     den = (1 + 2 * zeta * teta + teta**2) * Kp

#     K1 = -4 * zeta * teta / den
#     K2 = -4 * teta**2 / den

#     p2 = 0
#     offset = SPS//2
#     offset_list, err_list, mean_err = [], [], []

#     for i in range(len(s)//SPS - 1):
#         n = offset + SPS * i
#         e = ((np.real(s[n + SPS]) - np.real(s[n])) * np.real(s[n + SPS//2]) +
#              (np.imag(s[n + SPS]) - np.imag(s[n])) * np.imag(s[n + SPS//2]))

#         p2 = (p2 + e * (K1 + K2)) % 1
#         offset = int(np.round(p2 * SPS))

#         offset_list.append(offset)
#         err_list.append(e)

#     right = 0
#     for y in range(2):
#         for i in range(right - SPS//2, right + SPS//2 + 1):
#             errs = [((np.real(s[x+i+2*SPS]) - np.real(s[x+i+SPS])) *
#                      np.real(s[x+i+SPS+SPS//2]) +
#                      (np.imag(s[x+i+2*SPS]) - np.imag(s[x+i+SPS])) *
#                      np.imag(s[x+i+SPS+SPS//2]))
#                     for x in range(0, len(s) - 2*SPS - i, SPS)]
#             mean_err.append(np.mean(errs))

#         right = int(np.round(np.argmin(np.abs(mean_err))))
#         if y == 0:
#             mean_err.clear()

#     return (right,
#             np.asarray(offset_list, int),
#             np.asarray(err_list, float),
#             np.asarray(mean_err, float))

# For QPSK
def phase_detector_4(sample):
    if sample.real > 0:
        a = 1.0
    else:
        a = -1.0
    if sample.imag > 0:
        b = 1.0
    else:
        b = -1.0
    return a * sample.imag - b * sample.real

def costas(samples):
    N = len(samples)
    phase = 0
    freq = 0
    # These next two params is what to adjust, to make the feedback loop faster or slower (which impacts stability)
    alpha = 0.432
    beta = 0.02932
    out = np.zeros(N, dtype=np.complex64)
    for i in range(N):
        out[i] = samples[i] * np.exp(-1j*phase)
        error = phase_detector_4(out[i])

        # Advance the loop (recalc phase and freq offset)
        freq += (beta * error)
        phase += freq + (alpha * error)

        # Optional: Adjust phase so its always between 0 and 2pi, recall that phase wraps around every 2pi
        phase = phase % (2 * np.pi)

    return out


def symbol_sync_mm(samples, sps=10, interp=32, mu_init=0.1, gain=0.01):
    import numpy as np
    from scipy.signal import resample_poly

    samples_i = resample_poly(samples, interp, 1)

    sps_i = sps * interp  # samples per symbol after interpolation

    mu = mu_init
    i_in = int(2 * sps_i)   # стартуем не с нуля (для i-2)
    i_out = 2

    out = np.zeros(len(samples), dtype=np.complex64)
    out_rail = np.zeros_like(out)

    while (i_out < len(out) and
           i_in + sps_i < len(samples_i) and
           i_in - sps_i >= 0):

        out[i_out] = samples_i[int(i_in)]

        out_rail[i_out] = (
            (1.0 if out[i_out].real > 0 else -1.0) +
            1.0j * (1.0 if out[i_out].imag > 0 else -1.0)
        )

        x = (out_rail[i_out] - out_rail[i_out-2]) * np.conj(out[i_out-1])
        y = (out[i_out] - out[i_out-2]) * np.conj(out_rail[i_out-1])
        err = np.real(y - x)

        # Timing NCO
        i_in += sps_i + gain * err * interp

        i_out += 1

    return out[2:i_out]

rx = np.fromfile(f"sdr2sdr_qpsk.pcm", dtype=np.int16)
SPS = 10
samples_rx = (rx[0::2] + 1j * rx[1::2])

plt.figure()
plt.title("Сигнальное созвездие")
plt.scatter(np.real(samples_rx), np.imag(samples_rx))
plt.axhline()
plt.grid(True)
plt.axvline()

maxum = np.max(rx)
# mask = (np.abs(samples_rx) < 0.2 * maxum) & (np.abs(samples_rx) > 0.02 * maxum)
samples_rx = samples_rx
# samples_rx = costas_loop(samples_rx)
samples_mf = np.convolve(samples_rx, np.ones(SPS), mode="same")
signal = samples_mf
# right, offset_list, err_list, mean_err = gardner1(samples_mf, SPS)
symbols = symbol_sync_mm(signal, 10)
# offset_mask = np.array([v + SPS*i for i, v in enumerate(offset_list)])
# signal = samples_mf[offset_mask]
# signal /= np.sqrt(np.mean(np.abs(signal)**2))
symbols /= np.sqrt(np.mean(np.abs(symbols)**2))
plt.figure()
plt.title("Сигнальное созвездие")
plt.scatter(np.real(symbols), np.imag(symbols))
plt.axhline()
plt.grid(True)
plt.axvline()

cost = costas(signal)
plt.figure()
plt.title("Сигнальное созвездие")
plt.scatter(np.real(cost), np.imag(cost))
plt.axhline()
plt.grid(True)
plt.axvline()

# plt.figure()
# plt.title("Сигнал после согласованного фильтра")
# plt.subplot(2,1,1)
# plt.plot(np.abs(samples_rx))
# plt.grid()
# plt.subplot(2,1,2)
# plt.legend
# plt.plot(np.real(samples_rx))
# plt.plot(np.imag(samples_rx))
# plt.grid(True)
# plt.show()


# plt.figure()
# plt.title("Средняя ошибка к сдвигу от правильного сдвига")
# plt.axhline()
# plt.axvline()
# plt.grid()
# plt.scatter(np.arange(-SPS//2, SPS//2 + 1), mean_err)
# plt.plot(np.arange(-SPS//2, SPS//2 + 1), mean_err)

# plt.figure()
# plt.title("Ошибка и сдвиг к номеру символа")
# plt.grid()
# plt.plot(err_list, label='Error')
# plt.plot(offset_list, label='Offset')
# plt.legend()

plt.show()