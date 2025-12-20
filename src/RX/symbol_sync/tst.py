import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import resample_poly, lfilter

def qpsk_receiver(rx_samples, sps=10):
    """
    Полный QPSK приёмник: CFO -> MF -> Gardner -> downsample -> Costas -> демодуляция.
    rx_samples : complex64 numpy array
    sps        : samples per symbol
    """
    # --- 1. Грубая частотная компенсация (CFO 4th power) ---
    def coarse_freq_comp(samples):
        # 4th power для QPSK
        N = len(samples)
        x4 = samples**4
        angle = np.angle(np.sum(x4 * np.exp(-1j * np.arange(N))))
        freq_offset = angle / (4 * N)
        corrected = samples * np.exp(-1j * freq_offset * np.arange(N))
        return corrected

    rx_cfo = coarse_freq_comp(rx_samples)
    plt.figure()
    plt.title("После грубой частотной компенсации")
    plt.scatter(np.real(rx_cfo), np.imag(rx_cfo))
    plt.grid(True)
    plt.show()

    # --- 2. Matched Filter (MF) ---
    mf_taps = np.ones(sps) / sps
    rx_mf = lfilter(mf_taps, 1.0, rx_cfo)
    plt.figure()
    plt.title("После matched filter")
    plt.scatter(np.real(rx_mf), np.imag(rx_mf))
    plt.grid(True)
    plt.show()

    # --- 3. Символьная синхронизация (Gardner) ---
    def gardner(samples, sps):
        mu = 0.0
        out = []
        i = sps
        while i + sps < len(samples):
            out.append(samples[int(i + mu)])
            e = ((samples[int(i + sps)] - samples[int(i)]) * np.conj(samples[int(i + sps/2)]))
            mu += sps + 0.01 * np.real(e)
            i += int(np.floor(mu))
            mu -= np.floor(mu)
        return np.array(out)

    rx_gardner = gardner(rx_mf, sps)
    plt.figure()
    plt.title("После Gardner (символьная синхронизация)")
    plt.scatter(np.real(rx_gardner), np.imag(rx_gardner))
    plt.grid(True)
    plt.show()

    # --- 4. Downsample до 1 SPS ---
    rx_1sps = rx_gardner[::sps]
    plt.figure()
    plt.title("После downsample до 1 SPS")
    plt.scatter(np.real(rx_1sps), np.imag(rx_1sps))
    plt.grid(True)
    plt.show()

    # --- 5. Costas loop (точная фазовая синхронизация) ---
    def costas_qpsk(symbols, alpha=0.01, beta=0.001):
        phase = 0.0
        freq = 0.0
        out = np.zeros_like(symbols, dtype=np.complex64)
        for i, s in enumerate(symbols):
            z = s * np.exp(-1j * phase)
            out[i] = z
            err = np.sign(z.real) * z.imag - np.sign(z.imag) * z.real
            freq += beta * err
            phase += freq + alpha * err
            phase = np.mod(phase, 2*np.pi)
        return out

    rx_costas = costas_qpsk(rx_1sps)
    plt.figure()
    plt.title("После Costas loop")
    plt.scatter(np.real(rx_costas), np.imag(rx_costas))
    plt.grid(True)
    plt.show()

    # --- 6. Демодуляция QPSK (битовая) ---
    def qpsk_demod(symbols):
        bits = []
        for s in symbols:
            b0 = 0 if s.real > 0 else 1
            b1 = 0 if s.imag > 0 else 1
            bits.extend([b0, b1])
        return np.array(bits)

    rx_bits = qpsk_demod(rx_costas)
    return rx_bits, rx_costas

# --- Пример использования ---
# rx_samples = (rx_i + 1j * rx_q)  # numpy complex64 array
# bits, symbols = qpsk_receiver(rx_samples, sps=10)

rx = np.fromfile(f"rx2.pcm", dtype=np.int16)
samples_rx = (rx[0::2] + 1j * rx[1::2])
bits, symbols = qpsk_receiver(samples_rx, sps=10)

